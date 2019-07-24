#include "HTTPFilesystem.h"

#include "core/Core.h"

namespace xEngine {

HTTPFilesystem::HTTPFilesystem() {
  mg_mgr_init(&context_, nullptr);
}

HTTPFilesystem::~HTTPFilesystem() {
  mg_mgr_free(&context_);
}

DataPtr HTTPFilesystem::Read(Location location) {
  last_status_ = IOStatus::kFailed;
  auto path = location.path();
  polling_ = true;
  auto connection = mg_connect_http(&context_, Handle, path.c_str(), nullptr, nullptr);
  connection->user_data = this;
  while (polling_) mg_mgr_poll(&context_, 1000);
  mg_mgr_poll(&context_, 1000);
  return data_;
}

void HTTPFilesystem::Write(Location location, DataPtr data) {
  last_status_ = IOStatus::kFailed;
}

void HTTPFilesystem::Handle(mg_connection *connection, int event, void *eventData) {
  auto self = static_cast<HTTPFilesystem *>(connection->user_data);
  switch (event) {
    case MG_EV_CONNECT: {
      auto connectStatus = static_cast<int *>(eventData);
      if (*connectStatus != 0) {
        Log::GetInstance().Warning("HTTP connect fail of status(%d).\n", *connectStatus);
      }
      break;
    }
    case MG_EV_HTTP_REPLY: {
      connection->flags |= MG_F_CLOSE_IMMEDIATELY;
      self->polling_ = false;
      auto message = static_cast<http_message *>(eventData);
      if (message->resp_code == 200) {
        self->last_status_ = IOStatus::kSuccess;
        self->data_ = Data::Create(message->body.p, message->body.len);
      } else if (message->resp_code == 301) {
        auto location = mg_get_http_header(message, "Location");
        if (location) {
          eastl::string path(location->p, location->len);
          auto redirect = mg_connect_http(&self->context_, Handle, path.c_str(), nullptr, nullptr);
          redirect->user_data = self;
          self->polling_ = true;
        }
      } else {
        Log::GetInstance().Warning("HTTP response of status(%d).\n", message->resp_code);
      }
      break;
    }
    default:
      break;
  }
}

}
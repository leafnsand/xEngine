#ifndef XENGINE_ASSET_TEXTURE_H
#define XENGINE_ASSET_TEXTURE_H

#include "core/Core.h"

namespace xEngine {

class Texture {
 public:
  CREATE_FUNC_DECLARE(Texture)

  void set_texture_id(ResourceID value) { texture_id_ = value; }

  ResourceID texture_id() const { return texture_id_; }

  void set_sampler_id(ResourceID value) { sampler_id_ = value; }

  ResourceID sampler_id() const { return sampler_id_; }

 private:
  ResourceID texture_id_{kInvalidResourceID};
  ResourceID sampler_id_{kInvalidResourceID};
};

PTR_DECLARE(Texture)

}

#endif // XENGINE_ASSET_TEXTURE_H
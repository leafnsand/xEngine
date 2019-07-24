#if X_OPENGL

#include "OpenGLPipelineFactory.h"
#include "graphics/impl/OpenGL/OpenGLDefine.h"

#include "core/Log.h"

namespace xEngine {

void OpenGLPipelineFactory::Create(OpenGLPipeline &resource) {
  x_assert(resource.status() == ResourceStatus::kPending);
  resource.Loading();

  resource.Complete();
}

void OpenGLPipelineFactory::Destroy(OpenGLPipeline &resource) {
  x_assert(resource.status() == ResourceStatus::kFailed || resource.status() == ResourceStatus::kCompleted);
}

} // namespace xEngine

#endif // X_OPENGL
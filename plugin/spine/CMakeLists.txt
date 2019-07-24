add_library(xEngineSpine Spine.h Spine.cc)

target_link_libraries(xEngineSpine xEngine spine-c)

set_target_properties(xEngineSpine PROPERTIES FOLDER "plugin")
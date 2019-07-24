add_library(xEngineNuklear NuklearGUIConfig.h NuklearGUI.h NuklearGUI.cc)

target_link_libraries(xEngineNuklear xEngine)

set_target_properties(xEngineNuklear PROPERTIES FOLDER "plugin")
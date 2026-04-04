add_executable(mb.cpp-app-template main.cpp)
target_link_libraries(mb.cpp-app-template PRIVATE mb::cpp-app-template.core)
mb_devenv_set_target_defaults(mb.cpp-app-template)

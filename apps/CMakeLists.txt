set(_mb_app_root "${CMAKE_CURRENT_SOURCE_DIR}")
file(
    GLOB _mb_app_sources
    CONFIGURE_DEPENDS
    "${_mb_app_root}/*.cpp"
    "${_mb_app_root}/*.cc"
    "${_mb_app_root}/*.cxx"
)
add_executable(mb.cpp-app-template ${_mb_app_sources})
target_link_libraries(mb.cpp-app-template PRIVATE mb::cpp-app-template.core)
mb_devenv_set_target_defaults(mb.cpp-app-template)

if(MB_CPP_APP_TEMPLATE_BUILD_TESTS)
    add_test(NAME mb.cpp-app-template.smoke COMMAND mb.cpp-app-template)
endif()

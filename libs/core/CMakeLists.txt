add_library(mb.cpp-app-template.core STATIC)
add_library(mb::cpp-app-template.core ALIAS mb.cpp-app-template.core)
mb_devenv_set_target_defaults(mb.cpp-app-template.core)

file(
    GLOB_RECURSE _mb_core_headers
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)
target_sources(
    mb.cpp-app-template.core
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
            FILES ${_mb_core_headers}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/core.cpp"
)

if(NOT MB_CPP_APP_TEMPLATE_BUILD_TESTS)
    return()
endif()

include(mb-devenv-googletest)
mb_devenv_add_test(mb.cpp-app-template.test PRIVATE mb::cpp-app-template.core)

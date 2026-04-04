add_library(mb.cpp-app-template.core STATIC)
add_library(mb::cpp-app-template.core ALIAS mb.cpp-app-template.core)
mb_devenv_set_target_defaults(mb.cpp-app-template.core)

set(_mb_core_include "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(_mb_core_src "${CMAKE_CURRENT_SOURCE_DIR}/src")
file(
    GLOB_RECURSE _mb_core_headers
    CONFIGURE_DEPENDS
    "${_mb_core_include}/*.h"
    "${_mb_core_include}/*.hpp"
    "${_mb_core_include}/*.hh"
    "${_mb_core_include}/*.hxx"
    "${_mb_core_include}/*.inl"
)
file(
    GLOB_RECURSE _mb_core_sources
    CONFIGURE_DEPENDS
    "${_mb_core_src}/*.cpp"
    "${_mb_core_src}/*.cc"
    "${_mb_core_src}/*.cxx"
)
list(FILTER _mb_core_sources EXCLUDE REGEX [[\.test\.(cpp|cc|cxx)$]])
target_sources(
    mb.cpp-app-template.core
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS "${_mb_core_include}"
            FILES ${_mb_core_headers}
    PRIVATE ${_mb_core_sources}
)

if(NOT MB_CPP_APP_TEMPLATE_BUILD_TESTS)
    return()
endif()

include(mb-devenv-googletest)
mb_devenv_add_test(mb.cpp-app-template.test PRIVATE mb::cpp-app-template.core)

# CMake project `mb.cpp-app-template` — options `MB_CPP_APP_TEMPLATE_*`, C++ `mb::cpp_app_template`, headers under `include/mb/cpp-app-template/`.

cmake_minimum_required(VERSION 3.30...4.3)

project(
    mb.cpp-app-template # CMake Project Name, which is also the name of the top-level
    # targets (e.g., library, executable, etc.).
    DESCRIPTION "cpp-app-template library"
    LANGUAGES CXX
    VERSION 0.1.0
)

option(
    MB_CPP_APP_TEMPLATE_HEADER_ONLY
    "Build as a header-only (INTERFACE) library. When OFF, builds a static library with compiled sources under src/. Values: { ON, OFF }."
    OFF
)

# [CMAKE.SKIP_TESTS]
option(
    MB_CPP_APP_TEMPLATE_BUILD_TESTS
    "Enable building tests and test infrastructure. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

# [CMAKE.SKIP_EXAMPLES]
option(
    MB_CPP_APP_TEMPLATE_BUILD_EXAMPLES
    "Enable building examples. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

include(devenv/cmake/mb-devenv-install-library-config.cmake)

# From fetched https://github.com/devmarkusb/pre-commit (see fetchcontent-lockfile.json).
mb_pre_commit_setup()

if(MB_CPP_APP_TEMPLATE_HEADER_ONLY)
    add_library(mb.cpp-app-template INTERFACE)
else()
    add_library(mb.cpp-app-template STATIC)
endif()

add_library(mb::cpp-app-template ALIAS mb.cpp-app-template)

if(MB_CPP_APP_TEMPLATE_HEADER_ONLY)
    target_compile_definitions(
        mb.cpp-app-template
        INTERFACE MB_CPP_APP_TEMPLATE_HEADER_ONLY=1
    )
    target_sources(
        mb.cpp-app-template
        INTERFACE
            FILE_SET HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
    set(_mb_cpp_app_template_fs_scope INTERFACE)
else()
    target_compile_definitions(
        mb.cpp-app-template
        PUBLIC MB_CPP_APP_TEMPLATE_HEADER_ONLY=0
    )
    target_sources(
        mb.cpp-app-template
        PUBLIC FILE_SET HEADERS BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
    set(_mb_cpp_app_template_fs_scope PUBLIC)
endif()

set_target_properties(
    mb.cpp-app-template
    PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON
)

add_subdirectory(include/mb/cpp-app-template)

mb_devenv_install_library(mb.cpp-app-template)

if(MB_CPP_APP_TEMPLATE_BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(src)

if(MB_CPP_APP_TEMPLATE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

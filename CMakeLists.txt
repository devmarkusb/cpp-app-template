# CMake project `mb.cpp-app-template` — options `MB_CPP_APP_TEMPLATE_*`, C++ `mb::cpp_app_template`, helper lib under `libs/core/`.

cmake_minimum_required(VERSION 3.30...4.3)

project(
    mb.cpp-app-template
    DESCRIPTION "cpp-app-template example application"
    LANGUAGES CXX
    VERSION 0.1.0
)

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

list(PREPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/devenv/cmake")

if(MB_CPP_LIB_TEMPLATE_BUILD_TESTS)
    include(mb-devenv-ctest-root)
endif()
include(mb-devenv-defaults)
include(mb-devenv-install-library-config)
include(GNUInstallDirs)

# From fetched https://github.com/devmarkusb/pre-commit (see fetchcontent-lockfile.json).
mb_pre_commit_setup()

add_subdirectory(libs/core)

add_executable(mb.cpp-app-template apps/main.cpp)
target_link_libraries(mb.cpp-app-template PRIVATE mb::cpp-app-template.core)
mb_devenv_set_target_defaults(mb.cpp-app-template)

install(
    TARGETS mb.cpp-app-template
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

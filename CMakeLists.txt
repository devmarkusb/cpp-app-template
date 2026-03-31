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

include(GNUInstallDirs)

mb_pre_commit_setup()

if(MB_CPP_APP_TEMPLATE_BUILD_TESTS)
    enable_testing()
endif()

add_subdirectory(libs/core)

add_executable(mb.cpp-app-template apps/main.cpp)
target_link_libraries(mb.cpp-app-template PRIVATE mb::cpp-app-template.core)

install(
    TARGETS mb.cpp-app-template
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

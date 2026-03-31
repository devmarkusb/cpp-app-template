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

option(
    MB_CPP_APP_TEMPLATE_BUILD_QT_GUI
    "On macOS, use Qt6 for mb.cpp-app-template when Qt6 Widgets is found. Otherwise the terminal build is used."
    ON
)

set(_mb_use_qt_gui FALSE)
if(APPLE AND MB_CPP_APP_TEMPLATE_BUILD_QT_GUI)
    find_package(Qt6 6.4 COMPONENTS Widgets QUIET)
    if(Qt6_FOUND)
        set(_mb_use_qt_gui TRUE)
    else()
        message(
            STATUS
            "Qt6 Widgets not found — building terminal Snake for mb.cpp-app-template (brew install qt; set Qt6_DIR if needed)"
        )
    endif()
endif()

if(_mb_use_qt_gui)
    add_executable(
        mb.cpp-app-template
        apps/snake_qt/main.cpp
        apps/snake_qt/game_widget.cpp
        apps/snake_qt/snake_window.cpp
        apps/snake_qt/game_widget.hpp
        apps/snake_qt/snake_window.hpp
    )
    set_target_properties(
        mb.cpp-app-template
        PROPERTIES AUTOMOC ON MACOSX_RPATH ON
    )
    target_link_libraries(
        mb.cpp-app-template
        PRIVATE Qt6::Widgets mb::cpp-app-template.core
    )
    target_compile_features(mb.cpp-app-template PRIVATE cxx_std_23)
else()
    add_executable(mb.cpp-app-template apps/main.cpp)
    target_link_libraries(mb.cpp-app-template PRIVATE mb::cpp-app-template.core)
    if(APPLE)
        find_package(Curses REQUIRED)
        target_include_directories(
            mb.cpp-app-template
            PRIVATE ${CURSES_INCLUDE_DIR}
        )
        target_link_libraries(mb.cpp-app-template PRIVATE ${CURSES_LIBRARIES})
    endif()
endif()

install(
    TARGETS mb.cpp-app-template
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

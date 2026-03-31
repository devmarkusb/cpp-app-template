add_library(mb.cpp-app-template.core STATIC)
add_library(mb::cpp-app-template.core ALIAS mb.cpp-app-template.core)

set_target_properties(
    mb.cpp-app-template.core
    PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON
)

target_include_directories(
    mb.cpp-app-template.core
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

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
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/snake_game.cpp"
)

if(MB_CPP_APP_TEMPLATE_BUILD_TESTS)
    find_package(GTest REQUIRED)

    add_executable(mb.cpp-app-template.test)
    target_sources(
        mb.cpp-app-template.test
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/snake_game.test.cpp"
    )
    target_link_libraries(
        mb.cpp-app-template.test
        PRIVATE mb::cpp-app-template.core GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(mb.cpp-app-template.test)
endif()

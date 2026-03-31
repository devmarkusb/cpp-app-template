if(NOT MB_CPP_APP_TEMPLATE_HEADER_ONLY)
    file(GLOB_RECURSE sources CONFIGURE_DEPENDS "*.cpp")
    list(FILTER sources EXCLUDE REGEX "\\.test\\.cpp$")
    target_sources(mb.cpp-app-template PRIVATE ${sources})
endif()

if(NOT MB_CPP_APP_TEMPLATE_BUILD_TESTS)
    return()
endif()

find_package(GTest REQUIRED)

file(GLOB_RECURSE test_sources CONFIGURE_DEPENDS "*.test.cpp")

add_executable(mb.cpp-app-template.test)
target_sources(mb.cpp-app-template.test PRIVATE ${test_sources})
target_link_libraries(
    mb.cpp-app-template.test
    PRIVATE mb::cpp-app-template GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(mb.cpp-app-template.test)

set(ALL_EXAMPLES usage)

foreach(example ${ALL_EXAMPLES})
    add_executable(mb.cpp-app-template.examples.${example})
    target_sources(
        mb.cpp-app-template.examples.${example}
        PRIVATE ${example}.cpp
    )
    target_link_libraries(
        mb.cpp-app-template.examples.${example}
        PRIVATE mb::cpp-app-template
    )
endforeach()

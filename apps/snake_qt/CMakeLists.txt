find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

add_executable(
    mb-snake-qt
    main.cpp
    game_widget.cpp
    game_widget.hpp
    snake_window.cpp
    snake_window.hpp
)

target_link_libraries(
    mb-snake-qt
    PRIVATE Qt6::Widgets mb::cpp-app-template.core
)

target_compile_features(mb-snake-qt PRIVATE cxx_std_23)

if(APPLE)
    set_target_properties(mb-snake-qt PROPERTIES MACOSX_RPATH ON)
endif()

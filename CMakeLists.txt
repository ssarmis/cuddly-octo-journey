project( cuddly )

cmake_minimum_required( VERSION 3.19 )

link_directories(
    /opt/homebrew/Cellar/sdl2/2.0.14/lib
)

include_directories( 
    infernoGl/
    /opt/homebrew/Cellar/sdl2/2.0.14/include
)

add_executable( ${PROJECT_NAME} 
    infernoGl/gl.cpp
    src/window_bindings.cpp
    src/panel.cpp
    src/app.cpp
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-#warnings -Wno-switch -Wno-shift-negative-value -Wno-deprecated-declarations -Wno-format -Wno-write-strings -Wno-narrowing" )

target_link_libraries( ${PROJECT_NAME} 
    SDL2main
    SDL2
    c++
    "-framework OpenGl"
    "-framework Foundation"
)

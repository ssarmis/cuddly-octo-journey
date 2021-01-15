#!/bin/sh
# LINUX
# g++ -o app infernoGl/gl.cpp src/app.cpp \
#    -IinfernoGl -lGL -lSDL2main -lSDL2 -DDEBUG_BUILD -std=c++11 \
#    -Wno-#warnings -Wno-switch -Wno-shift-negative-value -Wno-deprecated-declarations -Wno-format -Wno-write-strings -Wno-narrowing -g 

# APPLE
clang -o app infernoGl/gl.cpp src/app.cpp \
    -L/opt/homebrew/Cellar/sdl2/2.0.14/lib -I/opt/homebrew/Cellar/sdl2/2.0.14/include \
    -IinfernoGl -lSDL2main -lSDL2 -DDEBUG_BUILD -lc++ -std=c++11 \
    -framework OpenGL \
    -framework Foundation \
    -Wno-#warnings -Wno-switch -Wno-shift-negative-value -Wno-deprecated-declarations -Wno-format -Wno-write-strings -Wno-narrowing -g -fsanitize=address -fomit-frame-pointer

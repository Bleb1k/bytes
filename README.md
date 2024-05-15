# bulid

if commands below don't work, make sure to look into [requirements for raylib](https://github.com/raysan5/raylib/#build-and-installation)


## linux
1. run `make all`
2. run `make run` 

**dev**

1. run `make dev` 

## windows
1. run `gcc -o build/dist/bytes.exe src/bytes.c -Wall -std=c2x -Wno-missing-braces -Wunused-result -O3 -I"external/raylib-5.0/src/external/glfw/include" -Ibuild/include -Lbuild/lib -lraylib -lm -lpthread -ldl -lopengl32 -lgdi32 -lwinmm && build/dist/bytes.exe`

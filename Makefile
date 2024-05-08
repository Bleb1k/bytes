EXECUTABLE = bytes

SRC_PATH = src
BUILD_PATH = build
EXTERNAL_PATH = external

INCLUDE_PATH = $(BUILD_PATH)/include
LIB_PATH = $(BUILD_PATH)/lib

DESTDIR = $(BUILD_PATH)/dist

CC = gcc
CFLAGS += -o $(DESTDIR)/$(EXECUTABLE) $(SRC_PATH)/$(EXECUTABLE).c
CFLAGS += -Wall -std=c2x -Wno-missing-braces -Wunused-result -O3
CFLAGS += -I$(INCLUDE_PATH) -L$(LIB_PATH) -lraylib -lm -lpthread -lGL -ldl -lrt -lX11

RAYLIB_PATH = $(EXTERNAL_PATH)/raylib-5.0
REMAKE_PATH = $(EXTERNAL_PATH)/remake


run:
	$(DESTDIR)/$(EXECUTABLE)

dev: build/remake
	$(BUILD_PATH)/libexec/remake -D$(RAYLIB_PATH) build/raylib -D$(SRC_PATH) build -R$(REMAKE_PATH)/src -Xrun

build: build/$(EXECUTABLE)
build/$(EXECUTABLE):
	mkdir -p $(DESTDIR)
	$(CC) $(CFLAGS)

build/raylib:
	mkdir -p $(LIB_PATH) $(INCLUDE_PATH)
	make all -C $(RAYLIB_PATH)/src RAYLIB_RELEASE_PATH=$(realpath $(BUILD_PATH))/lib
	cp --update $(RAYLIB_PATH)/src/raylib.h $(INCLUDE_PATH)/raylib.h
	cp --update $(RAYLIB_PATH)/src/raymath.h $(INCLUDE_PATH)/raymath.h
	cp --update $(RAYLIB_PATH)/src/rlgl.h $(INCLUDE_PATH)/rlgl.h

build/remake:
	mkdir -p $(BUILD_PATH)/libexec $(BUILD_PATH)/libexec
	make -C $(REMAKE_PATH) DESTDIR=$(realpath $(BUILD_PATH))/libexec

all: build/raylib build
	echo "bytes build success!"

clean:
	rm -rf build

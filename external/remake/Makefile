EXECUTABLE = remake

SRC_PATH = src
BUILD_PATH = build
EXTERNAL_PATH = external

INCLUDE_PATH = $(BUILD_PATH)/include
LIB_PATH = $(BUILD_PATH)/lib

DESTDIR = $(BUILD_PATH)/dist

CC = clang
CFLAGS += -o $(DESTDIR)/$(EXECUTABLE) $(SRC_PATH)/$(EXECUTABLE).c
CFLAGS += -Wall -std=c2x -Wno-missing-braces -Wunused-result -O3
# CFLAGS += -ls


build: build/$(EXECUTABLE)

run: build/$(EXECUTABLE)
	$(DESTDIR)/$(EXECUTABLE)
	
build/$(EXECUTABLE):
	mkdir -p $(DESTDIR)
	$(CC) $(CFLAGS)

clean:
	rm -rf build

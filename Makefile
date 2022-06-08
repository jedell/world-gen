CC = g++

CFLAGS = -std=c++2a `pkg-config --libs --cflags raylib`

TARGET = main

SRC_DIR = src

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/**/*.cpp)

all:
	$(CC) -o $(TARGET) $(CFLAGS) $(SRC_FILES)

clean:
	$(RM) -rf $(TARGET)  $(TARGET).dSYM
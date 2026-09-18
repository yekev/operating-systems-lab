CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
LDLIBS ?= -pthread

BUILD_DIR := build
TARGET := $(BUILD_DIR)/terminal-game

.PHONY: all clean

all: $(TARGET)

$(TARGET): src/terminal_game.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDLIBS)

$(BUILD_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all build run install

BUILD_DIR := ./bin
BINARY_NAME := termify
DEBUG_DIR := Debug

all: build

build:
	rm -rf $(BUILD_DIR) && ./scripts/build.sh

run:
	$(BUILD_DIR)/$(DEBUG_DIR)/$(BINARY_NAME)

install:
	./scripts/release.sh

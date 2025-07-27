.PHONY: all build

all: build

build:
	rm -rf ./bin && ./build.sh

run:
	./bin/termify

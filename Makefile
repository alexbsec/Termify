.PHONY: all build

all: build

build:
	rm -rf ./bin && ./scripts/build.sh

run:
	./bin/termify

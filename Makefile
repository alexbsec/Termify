# Makefile for termify

APP_NAME := termify

.PHONY: all build run ruclean tidy fmt test install

# Build the binary
build:
	go build -o bin/$(APP_NAME) cmd/main.go

# Run directly
run:
	go run ./cmd

# Format code
fmt:
	go fmt ./...

# Tidy up go.mod/go.sum
tidy:
	go mod tidy

# Run tests (future-proof)
test:
	go test -cover ./...

# Clean build artifacts
clean:
	rm -f $(APP_NAME)

# Install to GOPATH/bin or $GOBIN
install:
	go install ./...


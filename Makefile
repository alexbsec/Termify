# Makefile for termify

APP_NAME := termify

.PHONY: all build run clean tidy fmt test install

# Build the binary
build:
	go build -o bin/$(APP_NAME) cmd/main.go

# Run directly
run:
	go run .

# Format code
fmt:
	go fmt ./...

# Tidy up go.mod/go.sum
tidy:
	go mod tidy

# Run tests (future-proof)
test:
	go test ./...

# Clean build artifacts
clean:
	rm -f $(APP_NAME)

# Install to GOPATH/bin or $GOBIN
install:
	go install ./...


run: build
	@./main.out

build:
	@zig cc -o main.out main.c

# Phony targets
.PHONY: build run

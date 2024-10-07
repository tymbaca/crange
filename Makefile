run: build
	@./main.out

build:
	@zig cc -o main.out $(shell find ./** -name '*.c')

# Phony targets
.PHONY: build run

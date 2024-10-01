# Compile CPP code to object file, and then add the object file to the existing 
# tint library.
#
# This library can then be linked to SDL_gpu_shadercross.
# To acquire a precompiled version of tint for WASM, visit:
# https://github.com/klukaszek/tint-wasm
#
# Here you can find libtint.a which is a precompiled version of tint for WASM.
#
# You can also follow the instructions on the tint-wasm repository to compile
# tint from source and create your own libtint.a file if you want a non-WASM 
# version (or you don't trust it). You might have to make a few changes to 
# the final script to build Tint for non-WASM, but it should be relatively
# straightforward.
#
# Usage: Make
#
# To swap between building for WASM or non-WASM versions of Tint, simply replace the
# CC variable with either em++ or g++.

PROG = "tint-wasm-SDL-Installer"

# Compiler
CC = @em++

# Compiler flags
CFLAGS = -c -Wall -I./tint-wasm/

# library
LIB = ./tint-wasm/libtint.a

# Source files
SOURCES = tint_sdl.cpp

OUTPUT = -o tint_sdl.o

ifeq ($(wildcard libtint.a), libtint.a)
SKIP := 1
$(info $(PROG): Found libtint.a, skipping build process.)
else
SKIP := 0
$(info $(PROG): No libtint.a found, building tint-wasm library.)
endif

# Compile to object files
all:
	@if [ $(SKIP) -eq 0 ]; then \
		echo "$(PROG): Cloning tint-wasm library."; \
		git clone https://github.com/klukaszek/tint-wasm.git; \
		echo "$(PROG): Cloned tint-wasm repository."; \
		echo "$(PROG): This will be deleted after the build process."; \
		make build; \
		make archive; \
		make finish; \
		echo "$(PROG): Build successful."; \
	else \
		echo "$(PROG): No install needed."; \
	fi

build:
	$(CC) $(CFLAGS) $(SOURCES) $(OUTPUT) 

archive:
	@echo "$(PROG): Adding C wrapper to tint-wasm library."
	@ar rcs $(LIB) $(OUTPUT)
	@echo "$(PROG): Added tint_sdl.o to $(LIB)."
	@cp $(LIB) ./
	@echo "$(PROG): Copied library to -> $(PWD)"

finish:
	@rm -f *.o
	@echo "$(PROG): Removed object files."
	@rm -rf tint-wasm
	@echo "$(PROG): Removing tint-wasm directory."

# Clean up
# Remove any artifacts created by the build process.
clean: finish
	$(finish)
	@rm -f libtint.a
	@echo "$(PROG): Removed tint-wasm library."

.PHONY: all dependencies build archive finish clean

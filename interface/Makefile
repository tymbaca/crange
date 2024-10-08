COMPILER = clang
CFLAGS = -Wall
SOURCE_LIBS = -Ilib/
OSX_OUT = -o "bin/build_osx"
OSX_OPT = -Llib/ 
CFILES = src/*.c lib/*.c

run_osx: build_osx
	./bin/build_osx

build_osx:
	$(CC) $(CFLAGS) $(CFILES) $(SOURCE_LIBS) $(OSX_OUT) $(OSX_OPT)

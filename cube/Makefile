COMPILER = clang -Wall
SOURCE_LIBS = -Ilib/
OSX_OUT = -o "bin/build_osx"
OSX_OPT = -Llib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib.a
CFILES = src/*.c lib/*.c

run_osx: build_osx
	./bin/build_osx

build_osx:
	$(CC) $(CFILES) $(SOURCE_LIBS) $(OSX_OUT) $(OSX_OPT)

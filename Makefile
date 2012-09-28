CC = gcc
OBJECTS = main.o shaders.o
CFLAGS = -Wall -Wextra -ansi -pedantic
LDLIBS = -lglew32 -lfreeglut -lopengl32 -lglu32

all: jpspt

jpspt: $(OBJECTS)

clean:
	rm -f jpspt.exe $(OBJECTS)

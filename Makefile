CC = gcc
OBJECTS = main.o animation.o shaders.o entry.o
CFLAGS = -Wall -Wextra -ansi -pedantic
LDLIBS = -lglew32 -lfreeglut -lopengl32 -lglu32
RM = rm -f

.PHONY: all clean

all: jpspt

jpspt: $(OBJECTS)
	@echo LD $@
	@$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o jpspt

%.o: %.c
	@echo CC $@
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo CLEAN
	@$(RM) jpspt.exe $(OBJECTS)

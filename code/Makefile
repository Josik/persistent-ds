C       := gcc
CFLAGS  := -std=c99 -c
LFLAGS  := -std=c99 -Wall -pedantic
OBJECTS := main.o pclist.o pclistbuffer.o pclistelem.o prbtree.o prbtreenode.o

%.o : %.c
	$(C) $(CFLAGS) $< -o $@

all: a.out
	
a.out : $(OBJECTS)
	$(C) $(LFLAGS) $(OBJECTS)

clean:
	rm -rf $(OBJECTS) a.out

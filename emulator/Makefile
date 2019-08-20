CC=g++
CFLAGS=-Wall -o0 -ggdb
TARGET=v80
SRC=$(wildcard *.cc)
DEP=$(wildcard *.h)
OBJ=$(SRC:.cc=.o)

.PHONY: all clean

%.o: %.cc $(DEP)
	$(CC) -c $(CFLAGS) $< -o $@

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

clean:
	rm -f *.o $(TARGET)


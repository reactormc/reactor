CC = gcc
CFLAGS = -Wall -pedantic -std=gnu17 -g
SRC := $(shell find . -type f -regex ".*\.c")
LIB := $(shell find . -type f -regex ".*\.a")
LIB += -lunistring

OUT_NAME = reactor

all: $(OUT_NAME) $(OUT_NAME)_debug

$(OUT_NAME): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT_NAME) $(SRC) $(LIB) 

$(OUT_NAME)_debug: $(SRC) 
	$(CC) $(CFLAGS) -DDEBUG -o $(OUT_NAME)_debug $(SRC) $(LIB)

clean:
	rm -rf $(OUT_NAME) $(OUT_NAME)_debug *.dSYM

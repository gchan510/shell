CC = gcc
CFLAGS = -Wall
src = $(wildcard *.c)
obj = $(src:.c=.o)

main = shell

.PHONY: clean

all: $(main)

$(main): $(obj)
	$(CC) $(CFLAGS) -o shell $(obj) 

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(obj) shell

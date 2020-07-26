CFLAGS ?= -O2
CFLAGS += -Wall -Werror -Wformat=2 -Wshadow -Wpointer-arith \
	-Wcast-qual -Wstrict-aliasing=2 -Wwrite-strings -Wstack-protector \
	-Iinclude

all: pidentd

clean:
	rm -f pidentd

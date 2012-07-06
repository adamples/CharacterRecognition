# Wszystkie komunikaty o błędach, C99
CFLAGS+=-Wall
CFLAGS+=-std=gnu99

# Biblioteki
LIBS=cairo glib-2.0

# Biblioteka Cairo
CFLAGS+=`pkg-config --cflags $(LIBS)`
LDFLAGS+=`pkg-config --libs $(LIBS)`

TARGETS=train tsg

.PHONY: all clean

all: $(TARGETS)
	make all -C test

$(TARGETS): net.o

clean:
	rm -f *.o *.c~ *.h~ $(TARGETS)
	make clean -C test


# Wszystkie komunikaty o błędach, C99
CFLAGS+=-Wall
CFLAGS+=-std=gnu99

CFLAGS+=-I..

# Biblioteki
LIBS=cairo glib-2.0

# Biblioteka Cairo
CFLAGS+=`pkg-config --cflags $(LIBS)`
LDFLAGS+=`pkg-config --libs $(LIBS)`

TARGETS=test_1 test_2

.PHONY: all clean

all: $(TARGET)

clean:
	rm -f *.o *.c~ *.h~ $(TARGETS)

$(TARGETS): ../net.o

# Wszystkie komunikaty o błędach, C99
CFLAGS+=-Wall
CFLAGS+=-std=gnu99

CFLAGS+=-I..

# Biblioteki
LIBS=cairo glib-2.0 MagickWand

# Biblioteka Cairo
CFLAGS+=`pkg-config --cflags $(LIBS)`
LDFLAGS+=`pkg-config --libs $(LIBS)`

TARGETS=test_1 test_2 test_normalizer

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -f *.o *.c~ *.h~ $(TARGETS)

$(TARGETS): ../net.o ../img_utils.o

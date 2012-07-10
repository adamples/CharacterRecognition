# Wszystkie komunikaty o błędach, C99
CFLAGS+=-Wall
CFLAGS+=-std=gnu99

CFLAGS+=-I..

# Biblioteki
LIB_NAMES=glib-2.0 MagickWand

CFLAGS+=`pkg-config --cflags $(LIB_NAMES)`
LDLIBS+=`pkg-config --libs $(LIB_NAMES)`


TARGETS=test_1 test_2 test_normalizer

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -f *.o *.c~ *.h~ $(TARGETS)

$(TARGETS): ../net.o ../img_utils.o

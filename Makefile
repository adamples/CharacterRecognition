# Wszystkie komunikaty o błędach, C99
CFLAGS+=-Wall
CFLAGS+=-std=gnu99

# Biblioteki
LIB_NAMES=glib-2.0 MagickWand

CFLAGS+=`pkg-config --cflags $(LIB_NAMES)`
LDLIBS+=`pkg-config --libs $(LIB_NAMES)`

TARGETS=train test_net create_net io_data_gen io_data_inspect

.PHONY: all clean

all: $(TARGETS)
	make all -C test

$(TARGETS): net.o img_utils.o

clean:
	rm -f *.o *.c~ *.h~ $(TARGETS)
	rm -rf debug/*
	make clean -C test


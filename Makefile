GCC := gcc
CFLAGS_DBUS := $(shell pkg-config --cflags --libs dbus-1)
CFLAGS = -g -Wall -O ${CFLAGS_DBUS}

DEST =

.PHONY: clean

all: snotifd snotifc
o_files := $(patsubst %.c,%.o,$(wildcard *.c))

%: %.c
	${GCC} $< -o $@ -c ${CFLAGS}

snotifd: ${o_files}
	${GCC} -o $@ $@.o snotif.o ${CFLAGS}

snotifc: ${o_files}
	${GCC} -o $@ $@.o snotif.o ${CFLAGS} -lncursesw

install: snotifc snotifd
	install snotifd ${DEST}/usr/local/bin/snotifd
	install snotifc ${DEST}/usr/local/bin/snotifc

clean:
	rm -f *.o
	rm -f snotifc
	rm -f snotifd

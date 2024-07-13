GCC := gcc
CFLAGS_DBUS := $(shell pkg-config --cflags --libs dbus-1)
CFLAGS = -g -Wall -O ${CFLAGS_DBUS}

DEST =

.PHONY: install clean

all: snotifd snotifc
o_files := $(patsubst %.c,%.o,$(wildcard *.c))

%: %.c
	${GCC} $< -o $@ -c ${CFLAGS}

snotifd: ${o_files}
	${GCC} -o $@ $@.o snotif.o ${CFLAGS}

snotifc: ${o_files}
	${GCC} -o $@ $@.o snotif.o ${CFLAGS} -lncursesw

install: snotifc snotifd
	install snotifd ${HOME}/bin/snotifd
	install snotifc ${HOME}/bin/snotifc
	mkdir -p ${XDG_CONFIG_HOME}/systemd/user/
	install snotifd.service ${XDG_CONFIG_HOME}/systemd/user/
	systemctl --user daemon-reload

clean:
	rm -f *.o
	rm -f snotifc
	rm -f snotifd

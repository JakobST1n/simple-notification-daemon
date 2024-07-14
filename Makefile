GCC := gcc
CFLAGS_DBUS := $(shell pkg-config --cflags --libs dbus-1)
CFLAGS = -Wall -O ${CFLAGS_DBUS}

DEST =

.PHONY: install clean

all: snotifd snotifc README
o_files := $(patsubst %.c,%.o,$(wildcard *.c))

%: %.c
	${GCC} $< -o $@ -c ${CFLAGS}

snotifd: ${o_files}
	${GCC} -o $@ $@.o snotif.o ${CFLAGS}
	strip snotifd

snotifc: ${o_files}
	${GCC} -o $@ $@.o snotif.o ${CFLAGS} -lncursesw
	strip snotifc

README: snotifd.1 snotifc.1
	groff -Tutf8 -mandoc snotifd.1 | sed -e 's/\x1b\[[0-9;]*m//g' > $@
	groff -Tutf8 -mandoc snotifc.1 | sed -e 's/\x1b\[[0-9;]*m//g' >> $@

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
	rm -f README

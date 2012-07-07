MAKE=make

.PHONY: all clean test

all: sockets irc core
test: irc_test

src/libsockets.a: | sockets
src/libirc.a: | irc

# Subdirectories
sockets:
	$(MAKE) -C src/sockets

irc: src/libsockets.a
	$(MAKE) -C src/irc

core: src/libirc.a
	$(MAKE) -C src/core

# Cleaning
clean:
	$(MAKE) -C src/sockets clean
	$(MAKE) -C src/irc clean
	$(MAKE) -C src/core clean

# Tests
.PHONY: irc_test

irc_test: src/libirc.a
	$(MAKE) -C src/irc test

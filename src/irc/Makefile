CXX=g++ -g
RM=del /F
AR=ar rcs
INCLUDES=
CPPFLAGS=$(INCLUDES) -Wall -W -Wall -Wextra -I"." -I".."

.PHONY: all test clean

all: ../libirc.a

test: runtests.exe
	runtests.exe

# Build the static library
../libirc.a: LineConnection.o IRCClient.o IRCCommand.o
	$(AR) ../libirc.a $^

# Compile a .cpp into a .o
%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@

# Clean up object files
clean:
	$(RM) *.o tests\*.o

# Test
runtests.exe: ../libsockets.a ../libirc.a \
        ../common/runtests.o \
        tests/test_LineConnection.o tests/test_IRCCommand.o
	$(CXX) $(CFLAGS) ../common/runtests.o tests/test_LineConnection.o tests/test_IRCCommand.o -o $@ -lcppunit -L.. -lirc -lsockets -lws2_32


LineConnection.o: LineConnection.cpp LineConnection.h ../sockets/Socket.h
IRCClient.o: IRCClient.cpp IRCClient.h ../sockets/Socket.h \
 ../common/ReferenceCounted.h LineConnection.h
IRCCommand.o: IRCCommand.cpp IRCCommand.h IRCClient.h ../sockets/Socket.h \
 ../common/ReferenceCounted.h LineConnection.h
test_LineConnection.o: tests/test_LineConnection.cpp LineConnection.h \
 ../sockets/Socket.h
test_IRCCommand.o: tests/test_IRCCommand.cpp IRCCommand.h IRCClient.h \
 ../sockets/Socket.h ../common/ReferenceCounted.h LineConnection.h

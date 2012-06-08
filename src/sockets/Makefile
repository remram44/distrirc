CPP=g++
#RM=rm -f
RM=del /F
AR=ar rcs
INCLUDES=
CPPFLAGS=$(INCLUDES) -g -Wall -O2

.PHONY: all clean

all: ../libsockets.a

# Builds the static library
../libsockets.a: Socket.o TCP.o SSLSocket.o
	$(AR) ../libsockets.a Socket.o TCP.o SSLSocket.o

# Compile a .cpp into a .o
%.o: %.cpp
	$(CPP) -c $(CPPFLAGS) $< -o $@

# Clean up object files
clean:
	$(RM) *.o

Socket.o: Socket.cpp Socket.h
TCP.o: TCP.cpp TCP.h Socket.h
SSLSocket.o: SSLSocket.cpp SSLSocket.h Socket.h TCP.h

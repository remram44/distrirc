CXX=g++ -g
RM=del /F
INCLUDES=
CPPFLAGS=$(INCLUDES) -Wall -W -Wall -Wextra -I"." -I".."

.PHONY: all clean

all: core.exe

# Link the executable
core.exe: core.o ../libirc.a ../libsockets.a
	$(CXX) $(CFLAGS) core.o -o $@ -L.. -lirc -lsockets -lws2_32

# Compile a .cpp into a .o
%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@

# Clean up object files
clean:
	$(RM) *.o

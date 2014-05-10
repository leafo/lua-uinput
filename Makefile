
.PHONY: local

uinput.so: uinput.c
	gcc -O2 -fPIC -I/usr/include/lua5.1 -c uinput.c -o uinput.o
	gcc -shared -o uinput.so -L/usr/lib uinput.o

local::
	luarocks make --local uinput-dev-1.rockspec


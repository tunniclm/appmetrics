#include makefile for Linux
include xi32.mk
LINK_OPT=-Wl,--no-as-needed -m31
CFLAGS=-O0 -g3 -Wall -c -fmessage-length=0 -fPIC -DLINUX -DIBMRAS_DEBUG_LOGGING -m31 -D_S390 -DREVERSED
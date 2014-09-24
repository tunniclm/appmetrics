#include makefile for Linux
include xi32.mk
CC=gcc
LINK=gcc
CFLAGS=-O0 -g3 -Wall -c -fmessage-length=0 -fPIC -m32 -DLINUX -D_PPC -DIBMRAS_DEBUG_LOGGING
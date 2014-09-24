include xa64.mk
CC=gcc
LINK=gcc
CFLAGS=-O0 -g3 -Wall -c -fmessage-length=0 -m64 -fPIC -DLINUX -DIBMRAS_DEBUG_LOGGING -D_PPC -D_64BIT
LFLAGS+=-m64
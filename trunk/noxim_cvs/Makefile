TARGET_ARCH = linux
CC     = g++
OPT    = -O2 # -O3
DEBUG  = -g
OTHER  = -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER)
# CFLAGS = $(DEBUG) $(OTHER)

MODULE = run
SRCS = TNoC.cpp TRouter.cpp TProcessingElement.cpp TBuffer.cpp TStats.cpp \
	TGlobalStats.cpp TGlobalRoutingTable.cpp TLocalRoutingTable.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)

include ./Makefile.defs

# stand-alone applications
apsra2noxim: apsra2noxim.o
	$(CC) $(CFLAGS) apsra2noxim.o -o apsra2noxim

apsra2noxim.o: apsra2noxim.cpp
	$(CC) $(CFLAGS) -c apsra2noxim.cpp -o apsra2noxim.o

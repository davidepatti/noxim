TARGET_ARCH = linux
CC     = g++
OPT    = -O2 # -O3
DEBUG  = -g
OTHER  = -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER)
#CFLAGS = $(DEBUG) $(OPT) $(OTHER)

MODULE = noxim
SRCS = TNoC.cpp TRouter.cpp TProcessingElement.cpp TBuffer.cpp TStats.cpp \
	TGlobalStats.cpp TGlobalRoutingTable.cpp TLocalRoutingTable.cpp \
	TGlobalTrafficTable.cpp TReservationTable.cpp TPower.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)

include ./Makefile.defs

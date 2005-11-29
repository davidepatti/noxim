TARGET_ARCH = linux
CC     = g++
OPT    = -O3
DEBUG  = -g
OTHER  = -Wall -Wno-deprecated
# CFLAGS = $(OPT) $(OTHER)
CFLAGS = $(DEBUG) $(OTHER)

MODULE = run
SRCS = TRouter.cpp TProcessingElement.cpp TBuffer.cpp TStats.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)

include ./Makefile.defs

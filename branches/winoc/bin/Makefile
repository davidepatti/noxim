SYSTEMC = systemc-2.3.1
TARGET_ARCH=macosx64
#TARGET_ARCH=macosx64

CC     = g++
OPT    = -O3
DEBUG  = -g
OTHER  = # -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER) # $(DEBUG)

INCDIR = -I. -I.. -I../src -I$(SYSTEMC)/include
LIBDIR = -L. -L.. -L../src -L$(SYSTEMC)/lib-$(TARGET_ARCH)

#EXTRA_LIBS = -lpthread
LIBS = -lsystemc -lm $(EXTRA_LIBS)

VPATH = ../src:.

MODULE = noxim

SRCS = NoximNoC.cpp NoximRouter.cpp NoximProcessingElement.cpp NoximBuffer.cpp \
	NoximStats.cpp NoximGlobalStats.cpp NoximGlobalRoutingTable.cpp \
	NoximLocalRoutingTable.cpp NoximGlobalTrafficTable.cpp NoximReservationTable.cpp \
	NoximPower.cpp NoximCmdLineParser.cpp NoximMain.cpp NoximHub.cpp

OBJS = $(SRCS:.cpp=.o)

EXE    = $(MODULE)

.SUFFIXES: .cpp .o 

$(EXE): $(OBJS) $(SYSTEMC)/lib-$(TARGET_ARCH)/libsystemc.a 
	$(CC) $(CFLAGS) $(INCDIR) $(LIBDIR) -o $@ $(OBJS) $(LIBS) 2>&1 | c++filt

.cpp.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $<

clean:
	rm -f $(OBJS) *~ $(EXE) core

depend: 
	makedepend $(SRCS) -Y -f Makefile.deps


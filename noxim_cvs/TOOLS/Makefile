CC     = g++
OPT    = -O2 # -O3
DEBUG  = -g
OTHER  = -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER)

apsra2noxim: apsra2noxim.o
	$(CC) $(CFLAGS) apsra2noxim.o -o apsra2noxim

apsra2noxim.o: TOOLS/apsra2noxim.cpp
	$(CC) $(CFLAGS) -c apsra2noxim.cpp -o apsra2noxim.o

explorer: explorer.o
	$(CC) $(CFLAGS) explorer.o -o explorer

explorer.o: explorer.cpp
	$(CC) $(CFLAGS) -c explorer.cpp -o explorer.o

CC     = g++
OPT    = -O2 # -O3
DEBUG  = -g
OTHER  = -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER)

apsra2noxim: apsra2noxim.o
	$(CC) $(CFLAGS) apsra2noxim.o -o apsra2noxim

apsra2noxim.o: TOOLS/apsra2noxim.cpp
	$(CC) $(CFLAGS) -c apsra2noxim.cpp -o apsra2noxim.o

spacefilegen: spacefilegen.o
	$(CC) $(CFLAGS) spacefilegen.o -o spacefilegen

spacefilegen.o: spacefilegen.cpp
	$(CC) $(CFLAGS) -c spacefilegen.cpp -o spacefilegen.o

noxim_explorer: noxim_explorer.o
	$(CC) $(CFLAGS) noxim_explorer.o -o noxim_explorer

noxim_explorer.o: noxim_explorer.cpp
	$(CC) $(CFLAGS) -c noxim_explorer.cpp -o noxim_explorer.o

clean:
	rm -f *.o spacefilegen apsra2noxim noxim_explorer

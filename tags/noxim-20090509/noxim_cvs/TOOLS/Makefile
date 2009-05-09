CC     = g++
OPT    = -O2 # -O3
DEBUG  = -g
# OTHER  = -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER)


all: apsra2noxim noxim_explorer mapping2cg

apsra2noxim: apsra2noxim.o
	$(CC) $(CFLAGS) apsra2noxim.o -o apsra2noxim

apsra2noxim.o: apsra2noxim.cpp
	$(CC) $(CFLAGS) -c apsra2noxim.cpp -o apsra2noxim.o

noxim_explorer: noxim_explorer.o
	$(CC) $(CFLAGS) noxim_explorer.o -o noxim_explorer

noxim_explorer.o: noxim_explorer.cpp
	$(CC) $(CFLAGS) -c noxim_explorer.cpp -o noxim_explorer.o

mapping2cg: mapping2cg.o
	$(CC) $(CFLAGS) mapping2cg.o -o mapping2cg

mapping2cg.o: mapping2cg.cpp
	$(CC) $(CFLAGS) -c mapping2cg.cpp -o mapping2cg.o

clean:
	rm -f *.o apsra2noxim noxim_explorer mapping2cg


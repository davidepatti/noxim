CC     = g++
OPT    = -std=c++11 -O2
DEBUG  = -g
#OTHER  = -Wall -Wno-deprecated
CFLAGS = $(OPT) $(OTHER)


all: apsra2noxim noxim_explorer mapping2cg hotspot_ttable distancebased_ttable ttable_distance_calculator ttable_from_hub

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

hotspot_ttable: hotspot_ttable.o
	$(CC) $(CFLAGS) hotspot_ttable.o -o hotspot_ttable

hotspot_ttable.o: hotspot_ttable.cpp
	$(CC) $(CFLAGS) -c hotspot_ttable.cpp -o hotspot_ttable.o

distancebased_ttable: distancebased_ttable.o
	$(CC) $(CFLAGS) distancebased_ttable.o -o distancebased_ttable

distancebased_ttable.o: distancebased_ttable.cpp
	$(CC) $(CFLAGS) -c distancebased_ttable.cpp -o distancebased_ttable.o

ttable_distance_calculator: ttable_distance_calculator.o
	$(CC) $(CFLAGS) ttable_distance_calculator.o -o ttable_distance_calculator

ttable_distance_calculator.o: ttable_distance_calculator.cpp
	$(CC) $(CFLAGS) -c ttable_distance_calculator.cpp -o ttable_distance_calculator.o

ttable_from_hub: ttable_from_hub.o
	$(CC) $(CFLAGS) ttable_from_hub.o -o ttable_from_hub

ttable_from_hub.o: ttable_from_hub.cpp
	$(CC) $(CFLAGS) -c ttable_from_hub.cpp -o ttable_from_hub.o


clean:
	rm -f *.o apsra2noxim noxim_explorer mapping2cg hotspot_ttable distancebased_ttable ttable_distance_calculator ttable_from_hub

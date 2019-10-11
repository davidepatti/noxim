To make it work on CLion with Linux
--------------------------------------------
Tested on Ubuntu, other distro should be similar.
Maybe also useful for creating a win version instructions (let me know I will add to the repository)
- Create a new project importing source files selecting the noxim directory
- Replace the CMakeLists.txt file in the project root directory with the one found in this directory
- Enter in the noxim/bin/libs/systemc-2.3.1 and recompile it typing the following:

 ./configure --enable-pthreads
make
make install


Of course, the binary of noxim will be in different directory:
noxim/cmake-build-debug

so will need to copy (or link) the power.yaml file from the same directory:

ln -s ../bin/power.yaml

Also, is suggested to create a link to the config_examples to it can be easily accessed:

ln -s ../config_examples


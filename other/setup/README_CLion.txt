To make it work on CLion with Linux
--------------------------------------------
- Create a new project importing source files 
- Replace the CMakeLists.txt file in the project root directory with the one found in this directory
- Enter in the noxim/bin/libs/systemc-2.3.1 and recompile it typing the following:

 ./configure --enable-pthreads
make
make install



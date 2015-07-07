#!/bin/sh
sudo apt-get update
sudo apt-get install build-essential linux-headers-$(uname -r) wget tar git libyaml-cpp-dev libboost-dev
cd
wget http://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.1.tgz
tar -xzf systemc-2.3.1.tgz
cd systemc-2.3.1
mkdir objdir
cd objdir
sudo mkdir /usr/local/systemc-2.3.1
../configure --prefix=/usr/local/systemc-2.3.1

make
sudo make install

echo `echo /usr/local/systemc-2.3.1/lib-*` > systemc.conf
sudo mv systemc.conf /etc/ld.so.conf.d/
sudo ldconfig

cd 

git clone https://github.com/davidepatti/noxim

cd noxim/bin

make

cp ../config_examples/config.yaml .

./noxim

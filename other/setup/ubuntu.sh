#!/bin/bash
sudo apt-get update
sudo apt-get -y install build-essential linux-headers-$(uname -r) wget tar 
sudo apt-get -y install git || \
    sudo apt-get -y install python-software-properties && \
    sudo add-apt-repository ppa:git-core/ppa && \
    sudo apt-get update && \
    sudo apt-get -y install git
sudo apt-get -y install libboost-dev
sudo apt-get -y install libyaml-cpp-dev || \
    sudo apt-get -y install cmake && \
    git clone https://github.com/jbeder/yaml-cpp && \
    cd yaml-cpp/ && \
    mkdir lib && \
    cd lib && \
    cmake .. && \
    make && \
    sudo make install

cd ../..

wget http://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.1.tgz
tar -xzf systemc-2.3.1.tgz
cd systemc-2.3.1
mkdir objdir
cd objdir
export CXX=g++
export CC=gcc
sudo mkdir /usr/local/systemc-2.3.1
../configure --prefix=/usr/local/systemc-2.3.1

make
sudo make install

echo `echo /usr/local/systemc-2.3.1/lib-*` > systemc.conf
sudo mv systemc.conf /etc/ld.so.conf.d/
sudo ldconfig

cd ../..

git clone https://github.com/davidepatti/noxim

cd noxim/bin

make

cp ../config_examples/config.yaml .

./noxim

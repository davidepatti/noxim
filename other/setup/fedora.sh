#!/bin/bash

set -e

export GIT_SSL_NO_VERIFY=1

export CXX=g++
export CC=gcc

sudo dnf -y install @development-tools
sudo dnf -y install gcc-c++ wget tar cmake git kernel-headers boost-devel

git clone https://github.com/davidepatti/noxim
cd noxim/bin
mkdir -p libs
cd libs

git clone https://github.com/jbeder/yaml-cpp
cd yaml-cpp/
git checkout -b r0.6.0 yaml-cpp-0.6.0
mkdir -p lib
cd lib
cmake ..
make
cd ../..

wget http://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.1.tgz
tar -xzf systemc-2.3.1.tgz
cd systemc-2.3.1
mkdir -p objdir
cd objdir
../configure
make
make install 
cd ..

echo `pwd`/lib-* > systemc.conf
sudo ln -sf `pwd`/systemc.conf /etc/ld.so.conf.d/noxim_systemc.conf
sudo ldconfig
cd ../..

make
./noxim -config ../config_examples/default_config.yaml

#!/bin/bash

set -e

export GIT_SSL_NO_VERIFY=1

sudo apt-get update
sudo apt-get -y install build-essential linux-headers-generic wget tar libboost-dev cmake

if [ $(dpkg -s git 2>/dev/null | grep "ok installed" | wc -l) -eq 0 ]
then 
    sudo apt-get -y install git || \
        sudo apt-get -y install python-software-properties && \
        sudo add-apt-repository ppa:git-core/ppa && \
        sudo apt-get update && \
        sudo apt-get -y install git
fi

git clone https://github.com/davidepatti/noxim
cd noxim/bin
mkdir -p libs
cd libs

git clone https://github.com/jbeder/yaml-cpp
cd yaml-cpp/
git checkout -b r0.5.3 release-0.5.3
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
export CXX=g++
export CC=gcc
../configure 
make
make install
cd ..
echo `pwd`/lib-* > systemc.conf
sudo ln -s `pwd`/systemc.conf /etc/ld.so.conf.d/noxim_systemc.conf
#sudo cat `pwd`/systemc.conf >> /etc/ld.so.conf.d/noxim_systemc.conf
sudo ldconfig
cd ../..

make

./noxim -config ../config_examples/default_config.yaml

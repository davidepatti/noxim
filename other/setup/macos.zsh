#!/bin/zsh

###############################################################################
#               :
#   File        :   macos.zsh
#               :
#   Author(s)   :   Tim Brewis (@t-bre, tab1g19@soton.ac.uk)
#               :   George Peppard (@inventor02, gjp1g21@soton.ac.uk)
#               :
#   Description :   macOS installer script for noxim
#               :
###############################################################################

set -e

# print colours
RED='\u001b[31m'
GREEN='\u001b[32m'
MAGENTA='\u001b[35m'
RESET='\u001b[0m'

# only actually try to run on macOS
if [[ $OSTYPE == 'darwin'* ]]; then

    echo -e $MAGENTA"\n'noxim' installation script for macOS"$RESET
    echo -e "noxim will be installed at `pwd`"
    echo -e "Begin? [y/n]"
    read -sk 1 RESPONSE

    if [[ ! $RESPONSE =~ ^[Yy]$ ]]; then
        echo "Cancelled."
        exit 0
    fi
    echo
 
    # make sure required commands exist
    REQUIRED_COMMANDS=(git cmake make curl tar gcc g++)  

    echo -e $MAGENTA"Checking for required executables:"$RESET

    for CMD in $REQUIRED_COMMANDS
    do
        if ! command -v $CMD > /dev/null; then
            echo -e $RED"Error: $CMD not found"$RESET
            exit 1
        else 
            echo -e " - $CMD"
        fi
    done
    echo

    # check if noxim already downloaded
    # -> try to update if it does
    # -> clone if not
    echo -e $MAGENTA"Downloading noxim..."$RESET
    if [ -d "./noxim" ]; then
        echo -e $GREEN"noxim repository already exists."$RESET
        echo "Pulling from origin..."
        cd noxim 
        git pull
        cd bin
    else 
        git clone https://github.com/davidepatti/noxim
        echo -e $GREEN"noxim downloaded."$RESET
        cd noxim/bin
    fi
    echo

    # libs directory
    mkdir -p libs
    cd libs

    # yaml download
    echo -e $MAGENTA"Downloading yaml-cpp..."$RESET
    if [ ! -d "./yaml-cpp" ]; then
        git clone https://github.com/jbeder/yaml-cpp
        echo -e $GREEN"yaml-cpp downloaded.\n"$RESET
    else 
        echo -e $GREEN"yaml-cpp already downloaded.\n"$RESET
    fi

    # yaml build
    echo -e $MAGENTA"Building yaml-cpp..."$RESET
    cd yaml-cpp/

    if [ "`git branch --list r0.6.0`" ]; then
        git checkout r0.6.0
    else 
        git checkout -b r0.6.0 yaml-cpp-0.6.0
    fi

    mkdir -p lib
    cd lib
    cmake ..
    cmake --build .
    cd ../..
    echo -e $GREEN"Installed yaml-cpp successfully\n"$RESET

    # systemc download
    echo -e $MAGENTA"Downloading SystemC..."$RESET
    SYSTEMC_VERSION=systemc-2.3.1
    SYSTEMC_VERSION_SUFFIX=a

    if [ ! -d "./$SYSTEMC_VERSION" ]; then

        # download archive
        if [ ! -f "./$SYSTEMC_VERSION$SYSTEMC_VERSION_SUFFIX.tar.gz" ]; then
            curl -O https://www.accellera.org/images/downloads/standards/systemc/$SYSTEMC_VERSION$SYSTEMC_VERSION_SUFFIX.tar.gz
        fi
        
        # extract archive
        tar -xf $SYSTEMC_VERSION$SYSTEMC_VERSION_SUFFIX.tar.gz $SYSTEMC_VERSION$SYSTEMC_VERSION_SUFFIX
        rm $SYSTEMC_VERSION$SYSTEMC_VERSION_SUFFIX.tar.gz

        # rename removing suffix for compatibility with makefile
        mv $SYSTEMC_VERSION$SYSTEMC_VERSION_SUFFIX $SYSTEMC_VERSION
        echo -e $GREEN"SystemC downloaded.\n"$RESET

    else 
        echo -e $GREEN"SystemC already downloaded.\n"$RESET
    fi

    cd $SYSTEMC_VERSION

    # pre-build hack for ARM Macs
    # see: https://forums.accellera.org/topic/6984-running-systemc-on-new-apple-m1-silicon/
    if [[ $(uname -m) == 'arm64' ]]; then
        if ! grep -q arm64 configure; then
            sed -i '' '4618i\
            arm)\
                TARGET_ARCH=\"macosarm\"\
                CPU_ARCH="arm64"\
                QT_ARCH="pthreads"\
                ;;\
            ' configure
        fi
    fi

    # systemc build
    echo -e $MAGENTA"Building SystemC..."$RESET
    mkdir -p objdir
    cd objdir 

    export CXX=g++
    export CC=gcc
    ../configure 
    make 
    make install 

    echo -e $GREEN"SystemC built successfully.\n"$RESET 
    cd ..

    # make noxim
    echo -e $MAGENTA"Building noxim..."$RESET
    cd ../..
    make
    echo -e $GREEN"Done.\n"$RESET

else
    echo -e $RED"Error: not macOS!"$RESET
    exit 1
fi
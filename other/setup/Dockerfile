FROM ubuntu:18.04
RUN apt-get update && apt-get install -y apt-utils && apt-get install -y \
    sudo \
    software-properties-common \
    wget
RUN cd /opt && \
    bash -c 'bash <(wget -qO- https://raw.githubusercontent.com/davidepatti/noxim/master/other/setup/ubuntu.sh)'

FROM ubuntu:23.04

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    gcc \
    cmake \
    gdb \
    wget \
    ca-certificates \
    git \
    doxygen graphviz

WORKDIR /tmp

RUN git clone https://github.com/google/googletest.git -b release-1.12.1 &&\
    cd googletest &&\
    mkdir build &&\
    cd build &&\
    cmake ..
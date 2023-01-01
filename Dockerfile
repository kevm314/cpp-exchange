FROM ubuntu:23.04

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    gcc \
    cmake \
    gdb \
    wget

FROM fedora:latest

RUN dnf -y update \
    && dnf -y install \
        cmake \
        gcc-c++ \
        gdb \
        git \
        make \
    && dnf clean all

COPY . /mechanism_configuration/

ENV CXX=g++
ENV CC=gcc

RUN mkdir /build \
      && cd /build \
      && cmake \
        -D CMAKE_BUILD_TYPE=debug \
        ../mechanism_configuration \
      && make install -j 8

WORKDIR /build
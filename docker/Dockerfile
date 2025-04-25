FROM fedora:latest

RUN dnf -y update \
    && dnf -y install \
        cmake \
        gcc-c++ \
        gdb \
        git \
        make \
        python3 \
        python3-devel \
        python3-pip \
        python3-setuptools \
    && dnf clean all

COPY . /mechanism_configuration/

ENV CXX=g++
ENV CC=gcc

RUN cd /mechanism_configuration \
    && python3 -m pip install --upgrade wheel setuptools \
    && pip3 install --verbose .[test]

RUN mkdir /build \
      && cd /build \
      && cmake \
        -D CMAKE_BUILD_TYPE=debug \
        ../mechanism_configuration \
      && make install -j 8

WORKDIR /build
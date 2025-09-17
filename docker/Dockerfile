FROM fedora:latest

RUN dnf -y update \
    && dnf -y install \
        cmake \
        gcc-c++ \
        gdb \
        git \
        make \
    && dnf clean all

    # Install lcov from source (clean upstream version)
# RUN git clone https://github.com/linux-test-project/lcov.git /tmp/lcov && \
#     cd /tmp/lcov && \
#     make install && \
#     rm -rf /tmp/lcov

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
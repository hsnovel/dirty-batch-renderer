#!/bin/bash

export CC=gcc

cmake -DGLFW_BUILD_DOCS=OFF -S . -B build

pushd build
make
popd

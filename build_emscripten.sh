#!/bin/sh

mkdir -p obj
make CC=emcc "$@"


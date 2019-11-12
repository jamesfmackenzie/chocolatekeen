#!/bin/sh

cp -r ../../data data
cp emscripten_custom_config/chocolate-keen.cfg data
mkdir -p obj

make CC=emcc "$@"

rm -rf data

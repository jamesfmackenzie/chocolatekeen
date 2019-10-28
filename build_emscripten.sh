#!/bin/sh

cp emscripten_custom_config/chocolate-keen.cfg data

mkdir -p obj
make CC=emcc "$@"

rm data/chocolate-keen.cfg


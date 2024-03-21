#!/bin/sh

cp -r ../../data/GAMEDATA GAMEDATA
mkdir -p obj

make V=1

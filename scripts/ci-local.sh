#!/bin/sh
set -eu

mkdir -p build/linux/obj
mkdir -p build/emscripten/obj

make -C tests run
make -C tests clean
make -C build/linux obj/ui_gui_mapper.o obj/ui_gui_menu_loop.o
make -C build/emscripten -n

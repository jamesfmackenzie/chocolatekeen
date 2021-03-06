#!/bin/sh

cp -r ../../data/GAMEDATA GAMEDATA
cp lib/x64/SDL2.dll .
mkdir -p obj

if [ -z "$SDLCONFIGBIN" ]; then
    export SDLCONFIGBIN=sdl2-config
fi

#NOTE: An earlier revision of this script was tested with an archive of
#SDL 1.2.15 ready for mingw32. It seemed to work since we didn't recompile
#the libraries. In this revision of the script SDL 2.0 is the default, though.
if [ "$1" = "" ]; then
	echo
	echo "Usage:"
	echo "./build_mingw-w64.sh /path/to/SDL-mingw <Args for \"make\">"
	echo
	echo "Example:"
	echo "./build_mingw-w64.sh /c/development/SDL2-devel-2.0.10-mingw/SDL2-2.0.10/x86_64-w64-mingw32/ -j4"
	echo
	echo "Note that to use, for instance, SDL 1.2, you want something like this:"
	echo "SDLCONFIGBIN=sdl-config ./build_mingw-w64.sh ~/dev/SDL-1.2.15 -j4"
	echo
else
	SDL_PATH=$1
	shift 1
	make BINPREFIX=x86_64-w64-mingw32- SDLCONFIG="$SDL_PATH/bin/$SDLCONFIGBIN --prefix=$SDL_PATH" PLATFORM=WINDOWS CXXFLAGS=-Iwindows/include "$@"
fi


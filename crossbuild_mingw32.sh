#!/bin/sh

if [ -z "$SDLCONFIGBIN" ]; then
    export SDLCONFIGBIN=sdl2-config
fi

#NOTE: The following has been tested with an archive of SDL 2.0.1 ready for
#mingw-w64. It seems to work since we don't recompile the libraries for now...
if [ "$1" = "" ]; then
	echo
	echo "Usage:"
	echo "./crossbuild_mingw32.sh /path/to/SDL-mingw <Args for \"make\">"
	echo
	echo "Example:"
	echo "./crossbuild_mingw32.sh ~/dev/SDL-2.0.1 -j4"
	echo
	echo "Note that to use, for instance, SDL 1.2, you want something like this:"
	echo "SDLCONFIGBIN=sdl-config ./crossbuild_mingw32.sh ~/dev/SDL-1.2.15 -j4"
	echo
else
	SDL_PATH=$1
	shift 1
	make BINPREFIX=i686-w64-mingw32- SDLCONFIG="$SDL_PATH/bin/$SDLCONFIGBIN --prefix=$SDL_PATH" PLATFORM=WINDOWS CXXFLAGS=-Iwindows/include "$@"
fi

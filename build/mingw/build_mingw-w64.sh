#!/bin/sh
set -eu

SDL_DLL_PATH=/mingw64/bin/SDL2.dll
SDL_CONFIG_BIN=${SDLCONFIGBIN:-sdl2-config}

if ! command -v "$SDL_CONFIG_BIN" >/dev/null 2>&1; then
    echo "Missing SDL config tool: $SDL_CONFIG_BIN"
    echo "Install the MSYS2 MinGW SDL2 package and ensure it is in PATH."
    exit 1
fi

if [ ! -f "$SDL_DLL_PATH" ]; then
    echo "Missing SDL2 runtime DLL: $SDL_DLL_PATH"
    echo "Install the MSYS2 MinGW SDL2 package before running this script."
    exit 1
fi

rm -rf GAMEDATA
cp -r ../../data/GAMEDATA GAMEDATA
cp "$SDL_DLL_PATH" .
mkdir -p obj

make BINPREFIX=x86_64-w64-mingw32- SDLCONFIG="$SDL_CONFIG_BIN" PLATFORM=WINDOWS CXXFLAGS=-Iwindows/include "$@"

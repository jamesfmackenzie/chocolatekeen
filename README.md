# chocolatekeen
Chocolate Keen is a reverse engineering of Commander Keen in C/C++ and SDL. The goal is to be super accurate versus the original

The original authors are NY00123, QuantumG and Lemm. For more info see https://pckf.com/viewtopic.php?f=4&t=2536

## Releases
Find binaries and game data in the Releases directory:

* Releases/x86 - for 32-bit Windows
* Releases/x64 - for 64-bit Windows

## How to Play
Download the Releases subfolder that corresponds to your platform. Run `chocolate-keen.exe`  

## Building
This is a Makefile project. To build on Windows you'll need something like <a href="https://www.msys2.org/" target="_blank">MSYS2</a> and a gcc toolchain. Find setup instructions <a href="https://www.math.ucla.edu/~wotaoyin/windows_coding.html" target="_blank">here</a>

1. Download and unzip SDL 2.0 development libraries: https://www.libsdl.org/download-2.0.php
2. Launch a UNIX shell
3. Run build script that corresponds to your platform:
* Windows x86:
  * `./crossbuild_mingw32.sh /path/to/SDL2`
  * e.g. `./crossbuild_mingw32.sh /d/Development/SDL2-2.0.9/i686-w64-mingw32/`
* Window x64:
  * `./crossbuild_mingw-w64.sh /path/to/SDL2`
  * e.g. `./crossbuild_mingw-w64.sh /d/Development/SDL2-2.0.9/x86_64-w64-mingw32/`

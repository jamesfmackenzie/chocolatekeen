# chocolatekeen
Chocolate Keen is a reverse engineering of Commander Keen in C/C++ and SDL. The goal is to be super accurate versus the original

The original authors are NY00123, QuantumG and Lemm. For more info see https://pckf.com/viewtopic.php?f=4&t=2536

## How to Play
* You can play directly in the browser! Click <a href="http://www.jamesfmackenzie.com/chocolatekeen">here</a>
* If you want to download and play offline, see the Releases section below  

### Controls
* Arrows - Move Keen around
* Ctrl - Jump
* Alt- Use Pogo/Stop Pogo (if you have one)
* Ctrl AND Alt - Fire raygun (if it has charges)

## Releases
Download the game package from the <a href="https://github.com/jamesfmackenzie/chocolatekeen/tree/master/Releases">Releases</a> directory:

* Releases/x86 - for 32-bit Windows. Download the zip, extract and run `chocolate-keen.exe`
* Releases/x64 - for 64-bit Windows. Download the zip, extract and run `chocolate-keen.exe`
* Releases/WebAssembly - WebAssembly version. Download the zip, extract and run `chocolate-keen.html`

## Building
You can clone or download the chocolatekeen repo and build it yourself

### Windows (x86/x64)
This is a Makefile project. To build for Windows you'll need something like <a href="https://www.msys2.org/" target="_blank">MSYS2</a> and a gcc toolchain. Find setup instructions <a href="https://www.math.ucla.edu/~wotaoyin/windows_coding.html" target="_blank">here</a>

1. Download and unzip SDL 2.0 development libraries: https://www.libsdl.org/download-2.0.php
2. Launch a UNIX shell
3. Run build script that corresponds to your platform:
* Windows x86:
  * `./crossbuild_mingw32.sh /path/to/SDL2`
  * e.g. `./crossbuild_mingw32.sh /d/Development/SDL2-2.0.9/i686-w64-mingw32/`
* Window x64:
  * `./crossbuild_mingw-w64.sh /path/to/SDL2`
  * e.g. `./crossbuild_mingw-w64.sh /d/Development/SDL2-2.0.9/x86_64-w64-mingw32/`

### WebAssembly
To build for WebAssembly, you'll need the <a href="https://github.com/emscripten-core/emsdk" target="_blank">Emscripten SDK</a>. Find setup instructions <a href="https://emscripten.org/docs/getting_started/downloads.html" target="_blank">here</a>

1. Launch a shell with emscripten build tools (emsdk) in the PATH
2. Run `make`

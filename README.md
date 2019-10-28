# Chocolate Keen
Chocolate Keen is a reverse engineering of Commander Keen in C and SDL. The goal is to be super accurate versus the original

## How to Play
* You can play directly in the browser! Click <a href="http://www.jamesfmackenzie.com/chocolatekeen">here</a>
* If you want to download and play offline, see the Releases section below  

### Controls
* Arrows - Move Keen around
* Ctrl - Jump
* Alt- Use Pogo/Stop Pogo (if you have one)
* Ctrl AND Alt - Fire raygun (if it has charges)

## Releases
Downloadable game packages available in <a href="https://github.com/jamesfmackenzie/chocolatekeen/tree/master/Releases">Releases</a>. All versions come packaged as zip files with game data. Just extract and run. On Linux, you'll need the <a href="https://wiki.libsdl.org/Installation#Linux.2FUnix" target="_blank">SDL 2.0 runtime</a> installed. Other versions come pre-packaged with SDL

* <a href="https://github.com/jamesfmackenzie/chocolatekeen/tree/master/Releases/Linux">Releases/Linux</a> - Linux version. Run `./chocolate-keen`
* <a href="https://github.com/jamesfmackenzie/chocolatekeen/tree/master/Releases/WebAssembly">Releases/WebAssembly</a> - WebAssembly version. Run `chocolate-keen.html`
* <a href="https://github.com/jamesfmackenzie/chocolatekeen/tree/master/Releases/Windows-x86">Releases/Windows-x86</a> - for 32-bit Windows. Run `chocolate-keen.exe`
* <a href="https://github.com/jamesfmackenzie/chocolatekeen/tree/master/Releases/Windows-x64">Releases/Windows-x64</a> - for 64-bit Windows. Run `chocolate-keen.exe`

### Episodes Two and Three
The downloadable releases above come packaged with Commander Keen "Invasion of the Vorticons" Episode One: Marooned on Mars. But the engine supports the entire Vorticons trilogy. If you have Episode Two or Three, drop the level/data files into the GAMEDATA folder as follows:

    .
    ├── chocolate-keen.exe
    └── GAMEDATA
        ├── KEEN1
        |   └── ... Episode One level data (pre-packaged)
        ├── KEEN2
        |   └── ... Episode Two level data
        └── KEEN3
            └── ... Episode Three level data
    
You can then start each episode via `chocolate-keen.exe -startkeen1`, `chocolate-keen.exe -startkeen2` and `chocolate-keen.exe -startkeen3` respectively

## Building
You can clone or download the chocolatekeen repo and build it yourself

### Linux
For Linux builds you'll need a C compiler (e.g. gcc) and toolchain. The setup varies between distros. For Ubuntu you can install the build-essential package via `sudo apt-get install build-essential`. Once you have that setup:

1. Install the SDL 2.0 development libraries. This varies between Linux distros. For Ubuntu it's `sudo apt-get install libsdl2-dev`. More info <a href="https://wiki.libsdl.org/Installation#Linux.2FUnix" target="_blank">here</a>
2. Launch a shell
3. Run `./build_linux.sh`

### WebAssembly
To build for WebAssembly, you'll need the <a href="https://github.com/emscripten-core/emsdk" target="_blank">Emscripten SDK</a>. Find setup instructions <a href="https://emscripten.org/docs/getting_started/downloads.html" target="_blank">here</a>

1. Launch a shell with emscripten build tools (emsdk) in the PATH
2. Run `build_emscripten.sh`

### Windows (x86/x64)
This is a Makefile project. To build for Windows you'll need something like <a href="https://www.msys2.org/" target="_blank">MSYS2</a> and a gcc toolchain. Find setup instructions <a href="https://www.math.ucla.edu/~wotaoyin/windows_coding.html" target="_blank">here</a>

1. Download and unzip SDL 2.0 development libraries: https://www.libsdl.org/download-2.0.php
2. Launch a UNIX shell
3. Run build script that corresponds to your platform:
* Windows x86:
  * `./build_mingw32.sh /path/to/SDL2`
  * e.g. `./build_mingw32.sh /d/Development/SDL2-2.0.9/i686-w64-mingw32/`
* Window x64:
  * `./build_mingw-w64.sh /path/to/SDL2`
  * e.g. `./build_mingw-w64.sh /d/Development/SDL2-2.0.9/x86_64-w64-mingw32/`

## Authors
The original authors of Chocolate Keen are NY00123, QuantumG and Lemm. The project started with a reverse engineering of the original Keen code, and the goal is complete authenticity to the original - bugs and all. For more info see https://pckf.com/viewtopic.php?f=4&t=2536

Since bringing this to GitHub, I've ported to WebAssembly and I intend to support other platforms too 

## Developer Notes

### WebAssembly Notes

#### Performance
The WebAssembly build of Chocolate Keen relies on <a href="https://github.com/emscripten-core/emscripten/wiki/Emterpreter" target="_blank">Emterpreter</a>. This hinders performance significantly vs using Emscripten loops. If you don't have a sufficiently fast machine you may notice audio stutters or frame skips

#### Graphics
Although the original game supports both EGA and VGA graphics modes, the WebAssembly build is hardcoded to VGA. The original game resolution is 320x200, which is internally scan-doubled to 320x400. The x-axis is then doubled also to preserve the aspect ratio, yielding 640x400. Once overscan borders are added, the internal game resolution jumps to 672x414. This is rendered into the HTML canvas to provide a pixel perfect image, and then aspect-ratio corrected to 4:3 via CSS.

Some parts of the game (initial load and exit screen) use VGA text mode. Internally this renders at 720x400 (or 736x414 with overscan) and is nearest-neighbour downsampled to the game resolution of 672x414. It looks ugly, but we have to live with this for now to preserve crisp game visuals

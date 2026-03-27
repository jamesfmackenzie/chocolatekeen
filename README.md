# Chocolate Keen
Chocolate Keen is a reverse engineering of Commander Keen in C and SDL. The goal is to be super accurate versus the original

## How to Play
* You can play directly in the browser! Click <a href="http://www.jamesfmackenzie.com/chocolatekeen">here</a>
* If you want to download and play offline, see the Releases section below  

### Controls
* Arrows - Move Keen around
* Ctrl - Jump
* Alt on Windows/Linux, or Option on Mac - Use Pogo/Stop Pogo (if you have one)
* Ctrl AND Alt on Windows/Linux, or Ctrl AND Option on Mac - Fire raygun (if it has charges)

If you’re having issues with the controls (e.g. on a Mac keyboard), you can change the key bindings with F3.

On PlayStation Vita, use D-Pad or left analog for movement/navigation, Cross for jump/confirm, Circle for pogo or back, Square to fire, Triangle for status, and Start for confirm. For the full context-sensitive Vita mapping table, see [PlayStation Vita Controls](docs/vita-controls.md).

## Releases
Downloadable game packages are published on <a href="https://github.com/jamesfmackenzie/chocolatekeen/releases">GitHub Releases</a>. All versions come packaged as zip files with game data. Just extract and run. On Linux, you'll need the <a href="https://wiki.libsdl.org/Installation#Linux.2FUnix" target="_blank">SDL 2.0 runtime</a> installed. Other versions come pre-packaged with SDL.

* <a href="https://github.com/jamesfmackenzie/chocolatekeen/releases/latest">Latest release</a> - includes Linux, WebAssembly, Windows x86, Windows x64, and PlayStation Vita packages

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

On PlayStation Vita, episode data is currently loaded from bundled `app0:/GAMEDATA/KEEN1`, `app0:/GAMEDATA/KEEN2`, and `app0:/GAMEDATA/KEEN3` paths inside the installed VPK. That means Episodes Two and Three need to be included during VPK packaging rather than copied onto the device after install. If more than one episode is bundled, the Vita build will show the launcher so you can choose between them at startup.

## Building
You can clone or download the chocolatekeen repo and build it yourself

## Documentation

- [Architecture Overview](docs/architecture.md)
- [Technical Debt Backlog](docs/tech-debt.md)
- [PlayStation Vita Controls](docs/vita-controls.md)
- [Docs Layout Note](docs/README.md)

### Linux
For Linux builds you'll need a C compiler (e.g. gcc) and toolchain. The setup varies between distros. For Ubuntu you can install the build-essential package via `sudo apt-get install build-essential`. Once you have that setup:

1. Install the SDL 2.0 development libraries. This varies between Linux distros. For Ubuntu it's `sudo apt-get install libsdl2-dev`. More info <a href="https://wiki.libsdl.org/Installation#Linux.2FUnix" target="_blank">here</a>
2. Launch a shell
3. Navigate to `/build/linux`
4. Run `./build_linux.sh`

### WebAssembly
To build for WebAssembly, you'll need the <a href="https://github.com/emscripten-core/emsdk" target="_blank">Emscripten SDK</a>. Find setup instructions <a href="https://emscripten.org/docs/getting_started/downloads.html" target="_blank">here</a>

1. Launch a shell with emscripten build tools (emsdk) in the PATH
2. Navigate to `/build/emscripten`
3. Run `./build_emscripten.sh`

### Windows (MinGW)
This is a Makefile project. To build for Windows you'll need something like <a href="https://www.msys2.org/" target="_blank">MSYS2</a> and a gcc toolchain. Find setup instructions <a href="https://www.math.ucla.edu/~wotaoyin/windows_coding.html" target="_blank">here</a>

1. Download and unzip SDL 2.0 development libraries: https://www.libsdl.org/download-2.0.php
2. Launch a UNIX shell
3. Navigate to `/build/mingw`
4. Run build script that corresponds to your platform:
* Windows x86:
  * `./build_mingw32.sh /path/to/SDL2`
  * e.g. `./build_mingw32.sh /d/Development/SDL2-2.0.9/i686-w64-mingw32/`
* Window x64:
  * `./build_mingw-w64.sh /path/to/SDL2`
  * e.g. `./build_mingw-w64.sh /d/Development/SDL2-2.0.9/x86_64-w64-mingw32/`

### Windows (Visual Studio)
Build and Debug with Microsoft Visual Studio / Visual C++

1. Navigate to `/build/visual_studio`
2. Open Solution file (`ChocolateKeen.sln`) in Visual Studio
3. Restore NuGet dependencies (Project -> Manage NuGet Dependencies > Restore)
4. Build (Build > Build Solution)
5. Run (Debug > Start Without Debugging) or Debug (Debug > Start Debugging)

Game data is automatically copied to the Target Directory as part of build - so everything should "just run"

### PlayStation Vita (VitaSDK)
The repository includes a VitaSDK target in `build/vita`. It builds a `.vpk` that packages the executable, default config, and bundled Episode One `GAMEDATA`.

Prerequisites:
1. Install VitaSDK and SDL2 for Vita.
2. Source the VitaSDK environment so tools like `arm-vita-eabi-gcc`, `vita-elf-create`, `vita-make-fself`, and `vita-mksfoex` are in `PATH`.
3. Make sure `zip` is available on the host machine that is building the VPK.

Build steps:
1. Launch a shell with VitaSDK environment loaded.
2. Navigate to `/build/vita`.
3. Run `./build_vita.sh -j4` to build and package `chocolate-keen-vita.vpk`.
4. Optional: Run `./build_vita.sh game -j4` if you only want `chocolate-keen.elf`.
5. Copy `build/vita/chocolate-keen-vita.vpk` to the Vita and install it with VitaShell or another VPK installer.

Install/runtime layout:
* Read-only bundled game data is loaded from `app0:/GAMEDATA/...`
* Config, save data, mapper data, and other writable files are created under `ux0:data/chocolatekeen/`
* The build copies `data/GAMEDATA/*` into the VPK, so Episode One is available immediately after install
* Episode data lookup currently targets bundled `app0:/GAMEDATA/KEEN1`, `app0:/GAMEDATA/KEEN2`, and `app0:/GAMEDATA/KEEN3`

Notes:
* The Vita build defaults to software rendering (`USE_OPENGL=0`).
* The Vita target uses a platform-specific input policy in [src/platform/input_vita.c](/Users/jamesmackenzie/Documents/Code/GitHub/chocolatekeen/src/platform/input_vita.c) and still has open follow-up work tracked in [docs/tech-debt.md](/Users/jamesmackenzie/Documents/Code/GitHub/chocolatekeen/docs/tech-debt.md).
* The build currently places a default `chocolate-keen.cfg` in the VPK, but the runtime config path on Vita is `ux0:data/chocolatekeen/chocolate-keen.cfg`.

## Authors
The original authors of Chocolate Keen are NY00123, QuantumG and Lemm. The project started with a reverse engineering of the original Keen code, and the goal is complete authenticity to the original - bugs and all. For more info see https://pckf.com/viewtopic.php?f=4&t=2536

Since bringing this to GitHub, I've ported to WebAssembly and I intend to support other platforms too 

## Developer Notes

### Local CI-Style Check

Run this from the repo root to execute the same quick checks used during refactors:

```bash
./scripts/ci-local.sh
```

### WebAssembly Notes

#### Performance
The WebAssembly build of Chocolate Keen relies on <a href="https://github.com/emscripten-core/emscripten/wiki/Emterpreter" target="_blank">Emterpreter</a>. This hinders performance significantly vs using Emscripten loops. If you don't have a sufficiently fast machine you may notice audio stutters or frame skips

#### Graphics
Although the original game supports both EGA and VGA graphics modes, the WebAssembly build is hardcoded to VGA. The original game resolution is 320x200, which is internally scan-doubled to 320x400. The x-axis is then doubled also to preserve the aspect ratio, yielding 640x400. Once overscan borders are added, the internal game resolution jumps to 672x414. This is rendered into the HTML canvas to provide a pixel perfect image, and then aspect-ratio corrected to 4:3 via CSS

Some parts of the game (initial load and exit screen) use VGA text mode. Internally this renders at 720x400 (or 736x414 with overscan) and is nearest-neighbour downsampled to the game resolution of 672x414. It looks ugly, but we have to live with this for now to preserve crisp game visuals

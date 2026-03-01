# Architecture Overview

This repository is organized by subsystem to keep platform, game logic, rendering, and third-party code separated.

## Source Layout

- `src/app/`
  - Entrypoint and process startup.
- `src/core/`
  - Shared core types, globals, constants, and central engine-facing declarations.
- `src/engine/`
  - Main engine runtime orchestration and high-level lifecycle.
- `src/game/`
  - Game logic and gameplay systems (world map, menus, physics, enemies, UI helpers, sprite-related data).
- `src/episodes/`
  - Episode-specific game code/data for episodes 1, 2, and 3.
- `src/render/`
  - Rendering and graphics pipeline, including OpenGL-related interfaces.
- `src/input/`
  - Input handling and input mapping.
- `src/platform/`
  - Platform-dependent shims (sleep/timing/input prep variants per target).
- `src/decompression/`
  - Project-owned decompression code (`imageRLE`).
- `src/third_party/cgenius/`
  - Imported third-party decompression components.

## Build Layout

- `build/linux/`
  - Linux Makefile and scripts.
- `build/mingw/`
  - MinGW Makefile and scripts.
- `build/emscripten/`
  - Emscripten Makefile and scripts.
- `build/visual_studio/`
  - Visual Studio solution/project files.

## Docs and Releases

- `docs/`
  - Project docs and web assets.
- `docs/legacy/readme/`
  - Historical notes/reference material moved from the old root `readme/` folder.
- `releases/`
  - Packaged release artifacts by target platform.

## CI

GitHub Actions workflow (`.github/workflows/ci.yml`):

- Linux build and unit tests.
- Emscripten smoke build.

## Local Validation

Typical local validation flow:

```bash
mkdir -p build/linux/obj
make -C build/linux -j4
make -C tests run
```

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

## Naming Conventions

- Files are named by subsystem responsibility and feature (for example: `src/render/gfx.c`, `src/episodes/episode1_engine.c`).
- New file names should avoid historical `cvorticons_` prefixes unless matching untouched upstream content.
- Core headers in `src/core/` use concise names (`core.h`, `types.h`, `globals.h`, `constants.h`).
- Build object targets in Makefiles should mirror subsystem-oriented names (for example: `game_gameplay.o`, `render_gfx.o`).
- Public cross-file APIs currently keep legacy `CVort_` names for compatibility; prefer neutral names for new internal `static` helpers.

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

Quick CI-style smoke check from repo root:

```bash
./scripts/ci-local.sh
```

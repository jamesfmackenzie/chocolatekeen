# Architecture Overview

Chocolate Keen is organized by subsystem so runtime orchestration, gameplay logic, rendering, UI, platform shims, and data loading stay separated.

## Subsystems (`src/`)

- `src/app/`
  - Program entrypoint.
  - Key file: `chocolate-keen.c` (hands off to engine startup API).
- `src/core/`
  - Cross-cutting definitions shared across subsystems.
  - Core types/constants/globals plus small utility headers (`types.h`, `constants.h`, `globals.h`, `io_helpers.h`, `path_helpers.h`).
- `src/engine/`
  - High-level runtime lifecycle and orchestration.
  - Handles startup/shutdown flow, timing, audio glue, config/argument processing, and engine-level I/O helpers.
- `src/game/`
  - Core game simulation logic shared across episodes.
  - Includes gameplay loop pieces such as world map flow, menus, physics, enemies, and in-game UI helpers.
- `src/episodes/`
  - Episode-specific behavior/data layers for episodes 1, 2, and 3.
  - Split into per-episode logic modules and memory/data helpers.
- `src/render/`
  - Graphics/rendering pipeline and host output integration.
  - Contains EGA/VGA-era rendering paths, display updates, and OpenGL-related interfaces.
- `src/input/`
  - Input mapping and runtime event handling.
  - Includes mapping metadata, defaults, key-name tables, and host-event translation.
- `src/ui/`
  - Launcher/settings/input-mapper UI subsystem.
  - Contains menu definitions, handlers, menu loop logic, mapper navigation, and shared UI runtime types.
- `src/platform/`
  - Platform-dependent shims and target-specific host bindings.
  - Includes native vs Emscripten sleep behavior, path policies, and target-specific input mapping/policy shims.
- `src/decompression/`
  - Project-owned decompression helpers (`imageRLE`).
- `src/third_party/cgenius/`
  - Imported third-party decompression components.

## Naming Conventions

- Files are named by subsystem responsibility and feature (for example: `src/render/gfx.c`, `src/episodes/episode1_engine.c`).
- New file names should avoid historical `cvorticons_` prefixes unless matching untouched upstream content.
- Core headers in `src/core/` use concise names (`types.h`, `globals.h`, `constants.h`, `io_helpers.h`, `path_helpers.h`).
- Build object targets in Makefiles should mirror subsystem-oriented names (for example: `game_gameplay.o`, `render_gfx.o`).
- Public cross-file APIs currently keep legacy `CVort_` names for compatibility; prefer neutral names for new internal `static` helpers.

## Build Layout

- `build/linux/`
  - Linux Makefile/scripts.
- `build/mingw/`
  - MinGW Makefile/scripts for Windows builds.
- `build/emscripten/`
  - Emscripten Makefile/scripts for WebAssembly builds.
- `build/visual_studio/`
  - Visual Studio solution/project files (Windows IDE workflow).

## Other Top-Level Areas

- `docs/`
  - Project and engineering documentation.
- `site/`
  - GitHub Pages web/demo site assets.
- `docs/legacy/readme/`
  - Historical notes/reference material moved from the old root `readme/` folder.
- `data/`
  - Packaged game data used for local builds and release packaging.
- `tests/`
  - Unit-style and targeted regression tests.

## CI

GitHub Actions workflows:

- `.github/workflows/ci.yml`
  - Linux build + tests.
  - Emscripten smoke/build validation.
- `.github/workflows/release.yml`
  - Multi-platform release packaging (Linux, WebAssembly, Windows x86/x64).
  - Publishes release zip artifacts to GitHub Releases.
- `.github/workflows/pages.yml`
  - Publishes the static web/demo site from `site/` to GitHub Pages.

## Platform Boundaries

- `src/game/*`, `src/input/*`, and `src/engine/*` should stay platform-neutral.
- Platform-specific behavior and policy should live under `src/platform/*` behind `platform.h` hooks.
- Direct platform branches (for example `__VITA__`) in core gameplay/input/engine code are considered exceptions and should be justified narrowly when unavoidable.

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

Vita-specific local validation (when VitaSDK is installed):

```bash
make -C build/vita -j4
make -C build/vita vpk -j4
```

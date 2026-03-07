# Build System Layout

This directory uses small shared makefile fragments to reduce duplication across:

- `build/linux/Makefile`
- `build/mingw/Makefile`
- `build/emscripten/Makefile`
- `build/vita/Makefile`

## Shared Fragments

- `common_base.mk`: Base defaults (`CC`, `OBJ`, `SRC`, `SINGLE_EPISODE`, `BUILD_PROFILE`, etc.).
- `common_objects.mk`: Shared object lists and episode-specific object expansion.
- `common_profiles.mk`: Profile presets for Emscripten flags.
- `common_flags.mk`: Compiler defines and output naming (`EXE_PATH`, `EXT`).
- `common_toolchain.mk`: `emcc` vs native SDL toolchain flags.
- `common_preload.mk`: Optional Emscripten `--preload-file` handling.
- `common_rules.mk`: Shared compile rules for source/object mappings.
- `common_clean.mk`: Shared `clean` target with `CLEAN_DIRS` / `CLEAN_FILES`.

## Typical Include Order

Platform makefiles should follow this order:

1. `common_base.mk`
2. (platform-specific object/source variables)
3. `common_objects.mk`
4. `common_flags.mk`
5. `common_profiles.mk`
6. `common_toolchain.mk`
7. `common_preload.mk`
8. `common_rules.mk`
9. `common_clean.mk`

## Build Profiles

- `BUILD_PROFILE=native` (default)
- `BUILD_PROFILE=emscripten`
- `BUILD_PROFILE=emscripten-legacy`
- `BUILD_PROFILE=vita`

`build/emscripten/Makefile` sets `BUILD_PROFILE=emscripten` and defaults `CC=emcc` unless `CC` is explicitly overridden by the caller.
`build/vita/Makefile` sets `BUILD_PROFILE=vita` and defaults `CC=arm-vita-eabi-gcc`.

## Customization Points

- Change platform object inputs in each platform makefile:
  - `PLATFORM_OBJECTS`
  - `PLATFORM_SLEEP_OBJECT` / `PLATFORM_SLEEP_SOURCE`
  - `PLATFORM_INPUT_OBJECT` / `PLATFORM_INPUT_SOURCE`
- Change clean behavior per target with:
  - `CLEAN_DIRS`
  - `CLEAN_FILES`
- Change preload behavior per target with:
  - `PRELOAD_DATA_DIR`
  - `PRELOAD_REQUIRE_EXISTING_DIR`

## Notes

- Keep profile/toolchain logic inside shared fragments when possible.
- Prefer adding new shared behavior in fragments rather than duplicating per-platform blocks.
- If you add a new platform makefile, reuse this fragment structure to avoid drift.
- Emscripten warning policy lives in `common_profiles.mk` (`EMCC_WARN_CFLAGS`):
  switch exhaustiveness noise is suppressed, while higher-signal warnings remain enabled.

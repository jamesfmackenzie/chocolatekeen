# Technical Debt Backlog

This file tracks prioritized, actionable follow-ups for known limitations currently documented in code comments.

## How to use this backlog

- Keep each item scoped to a small/medium patch series.
- Link commits/PRs to the item ID.
- When complete, mark `Status` as `Done` and keep a short resolution note.

## Open items

| ID | Priority | Area | File(s) | Problem statement | Proposed scope | Acceptance criteria | Status |
|---|---|---|---|---|---|---|---|
| TD-001 | High | Input mapping | `src/input/input.c` | Pause and Ctrl share a DOS scancode in legacy mapping paths, so "wait for key" flows can consume unintended release events. | Split Pause-vs-Ctrl handling in emulated input event routing; add explicit state handling for the affected key path. | Press/release behavior for Ctrl and Pause is distinguishable in mapping/update flows; add regression tests in `tests/test_input_mapping_nav.c` or a dedicated input test. | Open |
| TD-002 | High | Engine input device state | `src/engine/engine.c` | `CVort_engine_isJoystickInUse()` returns a legacy constant (`true`) and ignores runtime device selection/disable state. | Introduce a runtime flag/source of truth (arguments + active mappings/devices) and use it in `CVort_engine_isJoystickInUse()`. | Function reflects real runtime state; no regressions in keyboard-only play and no-joystick environments. | Open |
| TD-003 | High | EXE load failure path | `src/engine/engine_loader.c` | Startup failure path skips normal shutdown to avoid persistence side effects, but this can drop launcher setting changes unexpectedly. | Define explicit policy: what should be persisted on early failure; implement a narrow persistence path if needed. | Behavior is documented and deterministic; manual verification of launcher-changed settings on startup error path. | Open |
| TD-004 | Medium | Audio device negotiation | `src/engine/engine_loader.c`, `src/engine/engine_audio.c` | Audio setup assumes fixed format/chunk behavior rather than adapting to opened device spec. | Query/validate final audio spec; adapt chunk/sample handling or fail with clear log when unsupported. | Audio init logs actual opened format; playback remains correct when device differs from requested spec. | Open |
| TD-005 | Medium | Text-mode emulation fidelity | `src/engine/engine.c` | DOS text-mode edge behavior (wrapping/attributes/cursor movement) is only partially verified. | Audit key text operations (`puts`, cursor movement, scrolling) against target DOS behavior and document intentional deviations. | Known deviations are documented in code/docs; tested representative edge cases. | Open |
| TD-006 | Medium | Render data decompression flags | `src/render/gfx.c` | EGALATCH/EGASPRIT decompression flag interpretation may not be correct for all assets/mods. | Verify flag semantics against known data sets; adjust conditions and error handling where needed. | Decompression path works for baseline episodes and at least one representative mod/data variant. | Open |
| TD-007 | Medium | Surface output buffering | `src/render/gfx.c` | Surface output path defers secondary-surface strategy with placeholder behavior. | Decide and implement a concrete secondary-surface policy for `OUTPUTSYS_SURFACE` (allocate/use/remove path). | No placeholder note remains; resize/reset paths stay stable across SDL versions. | Open |
| TD-008 | Medium | Sprite conversion fidelity | `src/render/gfx.c` | Sprite conversion currently ignores some per-copy metadata assumptions (offset/copy details). | Validate conversion logic against source format metadata and update conversion if mismatches are confirmed. | Visual/regression check passes for affected sprites across episodes; assumptions documented. | Open |
| TD-009 | Low | OpenGL border clear path | `src/render/gfx.c` | Border clear/copy path uses viewport-sized chunk logic with known limitations for full-window coverage. | Refactor copy strategy to a single consistent full-target clear/copy approach. | No viewport-coverage caveat remains; border rendering stable in fullscreen/windowed modes. | Open |
| TD-010 | Low | Legacy WORKAROUND review | `src/game/menus.c`, `src/game/gameplay.c`, `src/game/worldmap.c`, `src/episodes/episode2.c` | Several behavior-preserving workaround comments remain without explicit closure criteria. | Triage each workaround: keep-with-rationale, replace with explicit logic, or delete if obsolete. | Each workaround has either a concrete rationale + guard condition or is removed. | Open |


# Graphics Modes and Timing

This note explains three related but different concepts in Chocolate Keen:

- engine video modes
- emulated display adapter behavior
- perceived gameplay pacing

## Engine Video Modes

The engine uses a small set of internal video mode identifiers declared in [src/core/constants.h](/Users/jamesmackenzie/Documents/Code/GitHub/chocolatekeen/src/core/constants.h):

- `CVORT_VIDEO_MODE_LAUNCHER`
  - Internal launcher UI mode.
  - This is not a DOS BIOS mode.
- `CVORT_VIDEO_MODE_TEXT`
  - Text mode.
  - Corresponds to BIOS video mode `3`.
- `CVORT_VIDEO_MODE_GRAPHICS`
  - Graphics mode used by gameplay rendering.
  - Corresponds to BIOS video mode `0x0D`.

These constants describe the current engine display mode only. They do not decide whether the game is emulating VGA or EGA behavior.

## Adapter Behavior

VGA vs EGA is controlled separately through the emulated graphics adapter setting:

- `emulatedgfx=vga`
- `emulatedgfx=ega`

That choice affects values calculated in [src/engine/engine.c](/Users/jamesmackenzie/Documents/Code/GitHub/chocolatekeen/src/engine/engine.c), including:

- refresh rate
- overscan dimensions
- text font and text dimensions
- vertical retrace timing
- VGA double-scanning behavior for the 200-line graphics mode

So the engine can be in `CVORT_VIDEO_MODE_GRAPHICS` while still using either VGA-like or EGA-like timing and presentation rules.

## Why `0x0D` Does Not Mean "VGA Mode"

Historically, BIOS mode `0x0D` is a 320x200 16-color graphics mode associated with EGA-era hardware.

In Chocolate Keen, the same mode identifier is used as the gameplay graphics-mode switch, while adapter-specific behavior is layered on top of it. That is why the neutral name `CVORT_VIDEO_MODE_GRAPHICS` is preferable to an adapter-specific constant name.

## Refresh Rates

The engine currently simulates these display refresh rates from [src/core/constants.h](/Users/jamesmackenzie/Documents/Code/GitHub/chocolatekeen/src/core/constants.h):

- VGA: about `70.086 Hz`
- EGA: about `59.920 Hz`

These values feed into timing setup in [src/engine/engine_loader.c](/Users/jamesmackenzie/Documents/Code/GitHub/chocolatekeen/src/engine/engine_loader.c) and delay logic in [src/engine/engine_timing.c](/Users/jamesmackenzie/Documents/Code/GitHub/chocolatekeen/src/engine/engine_timing.c).

This is meant to reflect DOS-era display timing, not a modern uncapped render loop.

## Why Gameplay May Feel Slower Than 70 FPS

Even when VGA timing is selected, gameplay does not necessarily present a brand-new simulation state on every emulated refresh.

Relevant points:

- display timing is modeled around the selected refresh rate
- gameplay and drawing still use older tick-based pacing
- the main draw/update path waits in larger gameplay-step increments, for example the `delayInGameTicks(..., 6)` path in [src/render/gfx.c](/Users/jamesmackenzie/Documents/Code/GitHub/chocolatekeen/src/render/gfx.c)
- many animations and game systems advance using tick-derived logic such as `getTicks() >> n`

So a build can be "running with VGA timing" and still feel much closer to a lower gameplay update rate than a modern true-70-fps game.

## EGA vs VGA Feel

Switching from VGA to EGA changes the emulated refresh model and some presentation details, so the game can feel a bit slower or less smooth in EGA mode.

However, the difference in feel may be modest because the dominant pacing is not just the raw refresh rate. Gameplay cadence, animation steps, scrolling, and legacy delay behavior still shape what the player actually perceives.

## Authenticity vs Modern Framerate

The current timing design is oriented toward authenticity:

- PIT-derived timing
- refresh-rate simulation
- vertical blank modeling
- DOS-style delay behavior

Because of that, moving the project to a "true 60 fps" model would be more than a simple renderer change. It would likely require either:

- keeping original simulation timing and adding smoother presentation/interpolation, or
- a broader refactor that decouples simulation timing from rendering

The first approach is much lower risk if smoother motion is ever pursued.

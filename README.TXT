==============================================
Chocolate Keen v0.9.4 Alpha (April 29th, 2014)
==============================================

Supported Commander Keen episodes: 1-3 (i.e. all of Invasion of the Vorticons).

Thanks for getting Chocolate Keen! It originally begun as an attempt at a
"port" of Commander Keen 1 to modern platforms, taking advantage of earlier
work done by QuantumG, later processed by Lemm. The last stage of the "porting"
was done mostly by NY00123, with the help of decompression routines found in
the Commander Genius project. Later, Lemm also added support for episode 2.
Episode 3 support was then added in a similar fashion.

Commander Genius (formerly named CloneKeen Plus) is an interpreter
for Commander Keen managed by Gerstrong, which started off CloneKeen
by Caitlin Shaw.

Chocolate Keen aims to be compatible with the original files
used by vanilla Keen 1-3, like SCORES.CK1 and CTLPANEL.CK1.

----------------------------------
What is the current status of this
----------------------------------

- Ignoring bugs and some potential inaccuracies, in the
current state Chocolate Keen should be a complete "port"
of unmodified vanilla Commander Keen 1-3, v1.31.
- About joystick support: It works the vanilla Keen 1-3 way, meaning it has
great limitations. This is done for the purpose of being (sort of) compatible
with the CTLPANEL.CK1 file format, and simply for acting like vanilla Keen.
One issue is that the joystick has to be "calibrated" before it can be properly
used. For another issue, the buttons and axes to use can't be chosen.
In practice, though, the input mapper can be used for resolving these.
- Furthermore, even after a calibration and some usage of an actual joystick,
once you quit the application and restart it, keyboard input is forced again,
so you basically need to re-calibrate the joystick. This is the behavior of
vanilla Keen, so it is replicated by Chocolate Keen. However, you can modify
this behavior from the Chocolate Keen launcher (compatibility options).
- As an alternative, again the mapper can be used, emulating keyboard actions
while a dedicated game controller is used in practice.
- In case the mapper is used, though, chances are that after replacing one
game controller with another one, you'll have to reconfigure the mapping.
- If you think that you may replicate some memory corruption artifacts like
"random" maps by using cheats... you should rather expect a crash. Sorry, but
modern multi-tasking environments don't forgive bad attempts to access memory!
- Even if no crash occurs, though, a different (non-vanilla) behavior is
expected for now.
- Note that the surrounding overscan border *always* appears. There are
actually some kinds of usages to it within vanilla Keen 1-3, which are also
implemented in this "port".
- It implies that a bit unusual screen resolutions may be used, like 336x214.
Fullscreen mode may NOT work as expected in that case. However, this is usually
not a problem, since the desktop resolution is used by default (with scaling),
and a custom resolution may be specified optionally.
- The overscan border dimensions may be inaccurately chosen, though...

---------------------------------------------------------------------
An idea, maybe for later. No promise that anything will ever be done.
---------------------------------------------------------------------

- For the emulation of the EGA and VGA behaviors, ensure that accurate values
are used for stuff (like overscan border dimensions).

-----------------------------------------------
How to toggle off/on fullscreen within the game
-----------------------------------------------

Hold Alt and press on Enter (assuming the default input mapper setup is used).
Note that it does *not* work within the launcher.

--------------------------------
How to (un)lock the mouse cursor
--------------------------------

Hold Ctrl and press on F10 (if not affected by the mapping). While the cursor
is unlocked, you can also click on the window for locking it.

------------------------------------------------------------------------------
How to force an immediate shutdown of Chocolate Keen (while in a game session)
------------------------------------------------------------------------------

Hold Ctrl and press on F9 (unless mapped differently).

--------------------------------------
Any explanation about the input mapper
--------------------------------------

See mapper_doc/how_to_use.txt for details about the mapper.

-------------------------------------------------------------------------
After updating Chocolate Keen, something's very wrong with input handling
-------------------------------------------------------------------------

Chances are you've upgraded to v0.9.3 or later. Beginning with version 0.9.3,
SDL 2.0 is used by default, rather than SDL 1.2. As a side effect, this means
that a new mapper file should be constructed, because SDL 2.0 scancodes are
used instead of SDL 1.2 keysyms for keyboard input mapping.

-------------------
How to run the game
-------------------

- On Windows you can extract the archive to some place and then
load chocolate-keen.exe, if you want to play Commander Keen 1 (v1.31).
For Keen 2/3 you need to have a copy of Commander Keen 2/3, version 1.31,
ready. Like Keen 1 which is found in a directory named KEEN1 within GAMEDATA,
a copy of Keen 2 should reside in KEEN2 (again to be in GAMEDATA), and
similarly a copy of Keen 3 should be in KEEN3.

- On GNU/Linux, the source code should be compiled.
For more details see "Building Chocolate Keen from sources".

-------------------------------------------------------------------------------
Where should copies of the original games be in order located to make this work
-------------------------------------------------------------------------------

As stated beforehand, in the GAMEDATA directory.

------------------------------------------
OK, but where are the saved games and more
------------------------------------------

These are located in the exact place from which Chocolate Keen is loaded.

-------------------------------------
Um, what is chocolate-keen-sdl1_2.exe
-------------------------------------

Beginning with version 0.8.2, it is possible to construct a Chocolate Keen
executable targeting the SDL 2.0 API, as an alternative to SDL 1.2.
Since version 0.9.3, SDL 2.0 is the default SDL branch in use. In case of
problems, though, you can still use SDL 1.2 with chocolate-keen-sdl1_2.exe.
Note that a different input mapping file is to be used in such a case, and
some other settings are different (currently just video-related).

----------------------------------
Experimental OpenGL ES 2.0 support
----------------------------------

Beginning with v0.9.1, using an executable targeting SDL 2.0,
you can optionally pick "ES 2.0" as the version of OpenGL to use,
in case OpenGL output is desired (not via the SDL renderer).

It should be noted this was never tested on a device not supporting desktop
variants of OpenGL (non-ES), so there may be some bugs.
Furthermore, on the desktop, the OpenGL ES 2.0 and OpenGL 2.0 codepaths are
almost identical (up to a single ES-specific addition to the fragment shader
and possibly different function pointers used for various OpenGL calls).

If a working GPU with recent drivers is in use, supporting the OpenGL extension
EXT_create_context_es2_profile (WGL/GLX), then ES 2.0 should be supported.

As a potentially unstable alternative, on the Windows platform, ANGLE can be
used. Basically, ANGLE is an implementation of OpenGL ES 2.0 on top of DirectX.
For more details you can check out: http://code.google.com/p/angleproject/

ANGLE support explains the presence of a few additional files in a new
subdirectory named "experimental":
libegl.dll, libglesv2.dll, d3dcompiler_43.dll, d3dcompiler_46.dll.

There are great chances that with the way the Chocolate Keen video-related
codebase is currently structured, it simply won't work as expected (and with
the OpenGL ES 2 implementation of the SDL renderer a crash is possible).
In case you want to experiment with OpenGL ES 2.0, you should make sure that
the files libegl.dll and libglesv2.dll are in the exact some location where
chocolate-keen.exe is found.
Furthermore, on Windows Vista and later, d3dcompiler_46.dll should also be
located alongside the exe. On Windows XP, you want d3dcompiler_43.dll instead.

----------------------
Command line arguments
----------------------

These can be used for overriding settings found in chocolate-keen.cfg, a
configuration file that should be generated after Chocolate Keen has properly
functioned. For instance, to override the fullscreen setting and disable that:

chocolate-keen -fullscreen=false

It is further possible to start a specific game episode, if available.
For instance, to load Keen 2:

chocolate-keen -startkeen2

Multiple commands line arguments can be mixed, like in this example:

chocolate-keen -startkeen1 -fullscreen=false -scalef=3

Finally, to show the launcher UI, add -showlauncher as an argument. For a
fullscreen launcher window add -showfulllauncher instead.
A few BAT files (bundled with the Windows build) can be used
as alternatives for showing the launcher, too.

----------------------------
How to record/playback demos
----------------------------

Beginning with version 0.8.6, a "Vorticons Keen demo mode" can be toggled on.
When it is done, some behaviors change (in comparison to vanilla Keen 1-3),
letting you record demos and playback existing ones. Originally, this feature
was implemented as a patch for vanilla Keen 1-3. As of July 29th, 2013,
the patch can be found on any of the following pages:

http://www.shikadi.net/keenwiki/Patch:Demo
http://www.keenmodding.org/viewtopic.php?t=1749

Furthermore, the second page (on keenmodding.org) also has a link to samples
of recorded demos.

The motivation behind this feature is compatibility testing. To be more
specific, one may now test if both vanilla Keen (with a few modifications)
and Chocolate Keen reproduce demo playbacks in the exact same way.

To record a demo, observe that the menu item "Continue Game" is replaced with
"Record A Demo" when this feature is in use. Furthermore, to play existing
demos hide the main menu by choosing "Restart Demo" and then waiting for a bit.

For Keen 1, valid demo filenames are DEMO0.CK1, DEMO1.CK1 and so on up to
DEMO9.CK1. Similar names are valid for Keen 2/3.

Note that while the chosen demo file format is simpler than the format
used by Keen 4-6, it it also less efficient in terms of storage.
In particular, don't expect to be able to record a 4 minutes long demo.
However, this format comes from code that was actually found in the
vanilla Keen 1-3 executables.

-----------
Known issue
-----------

- Troubles may arise if a single emulated event is mapped to two distinct
host events or more. This is the case, for instance, if physical key Left Ctrl
and a joystick button both trigger the emulation of Left Ctrl key press
in-game. Chances are the troubles are less significant if these are just
digital on/off switches which are in use (say non-analog keyboard keys and
joystick buttons), but once different host events are involved in some ways
(such as joystick axis motion or mouse motion) it may seem like certain buttons
or keys simply do no function at all, even if it's not the expected behavior.

---------
Changelog
---------

April 29th 2014 (v0.9.4):
- The launcher UI can be displayed in a fullscreen window again, by specifying
the command line argument -showfulllauncher (instead of -showlauncher).
Note that in the SDL 1.2 case it's just a small(er) sub-window in a fullscreen
window for now (assuming the fullscreen dimensions are greater than 640x480).
- One piece missing since v0.8.2 should finally be available:
A UI for input mapping. It is now possible to add/remove/edit mapper bindings
from the Chocolate Keen app itself (via the launcher). With that, there is no
need to fiddle with a file like chocolate-keen-mapper-sdl2_0.map manually.
- mapper_doc/how_to_use.txt has been updated to reflect this addition.
- As some words of a warning, this BREAKS compatibility with older mapper
files having too many bindings involving the exact same host side event.
Realistically, though, that hasn't really been usable from the very beginning
(think about a press on the "1" key leading to emulated presses on all keys
of a keyboard).
- Furthermore, there are very great chances that various mapper-related bugs
are hidden, here and there.
- In fact, a few such bugs dating back to v0.8.2 have been found and fixed.
For one, after writing the mapper state to a file, the file wasn't guaranteed
to be closed as expected. In addition, bindings for the mapper saving handler
(Ctrl+F8 by default) weren't getting properly loaded from the mapper file.
- As a consequence of the addition of the mapper UI, though,
this saving handler has been removed.
- Misc. (minor) UI adaptations and README file edits have been done.

April 4th 2014 (v0.9.3):
- SDL 2.0 is the default SDL branch in use now. In particular,
chocolate-keen.exe is renamed chocolate-keen-sdl1_2.exe,
while chocolate-keen-sdl2.exe is renamed chocolate-keen.exe.
- As a side effect, a new input mapper file should be generated and edited, in
case a migration from an old SDL 1.2 exe to a recent SDL 2.0 exe is desired.
- "Back to the roots": As of v0.9.3, the launcher UI is *not* displayed on
startup by default. -showlauncher should be added as a command line argument
in order to gain access to the launcher. Alternatively, you can check out
chocolate-keen-launcher.bat (or chocolate-keen-sdl1_2-launcher.bat).
- If no command line argument is added, Chocolate Keen looks for a supported
Keen episode in a location like GAMEDATA/KEEN1. A specific episode can be
requested by adding -startkeen1, -startkeen2 or -startkeen3 as an argument.
- Realizing the code handling the launcher UI had been made to do more than
necessary (roughly being a super-simplistic GUI toolkit), it has basically
been replaced with (mostly) different code, used for launcher menus exclusively
as of this release. It can still be improved, but for now we have that.
- User video settings don't apply to the Launcher UI itself anymore. The exact
settings used for the UI are chosen to be internal safe defaults. For now they
depend on the SDL branch (1.2 vs 2.0).
- While the window is still not resizable during gameplay, with SDL 2.0 the
launcher UI window should be resizable as of this release. The main reason
the window is not permanently resizable is a kind of a technical difficulty.
- The setting "Don't forced (emulated) keyboard" has been replaced with a more
general setting, "First (emu) input device", allowing one to force (emulated)
mouse or joystick input on startup as additional options.

March 28th 2014 (v0.9.2):
- GPU palette cycling has been added. Requires OpenGL 2.0 / ES 2.0 rendering
(not via an SDL renderer). It (very) roughly simulates the behaviors of EGA/VGA
adapters in mode 0Dh, where a palette of colors can be quickly modified with
no change to video memory data representing graphics.
This theoretically reduces (by a little margin) CPU usage, since the code
executed by the CPU doesn't need to handle the cycling and 8-bit -> RGBA
conversions on its own. Memory usage and bandwidth is also reduced (a bit),
since only 8-bit graphical data is stored and transferred to the graphics card
(or similar). A minor disadvantage is that the GLSL fragment shader program is
doing a bit more work.
Enabled by default (if GL 2.0 / ES 2.0 rendering is in use).
- A general restructuring of graphics handling, mainly related to window and
resource management (say creation/destruction and resizing), has been done.
This may result in unexpected bugs.
- As a consequence, with a bit of luck, it should be possible to modify more
settings from the launcher UI and apply the changes with no restart required.
- VSync may still be a problem in the OpenGL case, if SDL 1.2 is used.
- VSync for software rendering in SDL 1.2 has been added, but is totally
untested (and is, in fact, not available on Linux if an X11 server is used).
- ANGLE can be used, although a window recreation can still lead to a crash
(coming from a fullscreen toggle or getting out of the video settings submenu).
Hence, ANGLE is not in use by default.
However, the crash may occur on Wine only, and not on a real Windows setup, as
revealed by a simple (external) test case. Thanks to Lunick for checking this.

March 22nd 2014 (v0.9.1):
- A new OpenGL rendering path has been added (unrelated to the SDL renderer),
powered by Open GL 2.0 / ES 2.0 and shader-based. As a consequence, it is now
possible to pick one of a few supported OpenGL versions, depending on the
environment and the Chocolate Keen executable: ES 2.0 (SDL 2.0 only), 2.0, 1.1.
- It should be noted that the OpenGL ES 2.0 path has not been tested in an
environment lacking support for desktop variants of OpenGL (say a phone).
- Some general improvements in OpenGL handling have been applied, mainly in the
way supported features of a given context are detected.
- For the few who want to experiment with this (and expect a crash or other
malformed behaviors), SDL2.dll has been updated to v2.0.3, so there is the
benefit of (theoretical) support for OpenGL ES 2 via the ANGLE library.
- If OpenGL output is chosen, with OpenGL ES 2.0 as the version, then
offscreen rendering is forced. Reason is that otherwise, a hackish alternative
involving glCopyTexSubImage2D would be in use, only it appears to be buggy on
one setup at the least (if supported at all).
- While a non-fullscreen window is not resizable, some improvements have been
done to take care of the case a window is resized anyway (possibly fullscreen).
In particular, less work should be done when a resize notification arrives.
- Fixed an infinite loop coming from a failure to get a desired fullscreen
resolution.
- Another such loop fixed, for the case a framebuffer configuration fails.
- The option "Force cut fullscreen" has been removed from the launcher UI,
although it can manually be toggled by editing the cfg file. In general, there
are *very* great chances it'll make things malfunction (if supported at all).
- Updated logging function (e.g., arguments should be printed as expected now).
- Fixed a few compilations warnings related to fprintf format string arguments.

January 14th, 2014 (v0.9.0):
- A new launcher GUI has been added. It can assist while there is no access to
a full/laptop-sized keyboard. It can further assist in other ways, of course.
- Chocolate Keen specific settings are saved to chocolate-keen.cfg and are
remembered between launches.
- The usage of the command line arguments has changed, mostly mimicking the
cfg file format in a way, but also letting one skip the launcher.
- Chocolate Keen now looks for a Keen episode in a location like GAMEDATA/KEEN1
(GAMEDATA\KEEN1 on Windows).
- On the other hand, rewritable files from vanilla Keen, like SCORES.CK1, are
written to the *working directory* of the Chocolate Keen process (as before).
- A bug with text mode cursor blinking recently introduced is now fixed.
- Internal CGA/EGA/VGA font storage is modified, using 1 byte per pixel
instead of 1 bit.

January 5th, 2014 (v0.8.10):
- Chocolate Keen should now be ready to work on environments with desktop color
depths differing from 32-bit; Or at least, the situation is improved with
regard to that. A few specific tests show code changes are doing their job,
although this version has not exactly been tested with truly varying desktop
color depths. Note that a different color depth may be used internally.
- The command line argument -nofbo is replaced with -nooffscreen, which is
applicable with -texture if SDL 2.0 is used, in addition to -opengl.
- With -texture in use, off screen rendering is not a must if bilinear
filtering is desired. (The latter was, in fact, the only way in v0.8.6.)
- Some more optimizations for text mode emulation are now applied.
- Sorry, but the text mode contents appearing on loading are back, although
they shouldn't be visible for long.
- A few more technical changes, here and there. Again this includes the README.

December 14th, 2013 (v0.8.9): A small update, a 23-years anniversary release.
- Proper handling of the emulated VGA graphical 320x200 mode, which is
double-scanned to 320x400, is now done. In practice, this has an effect on
the dimensions of the overscan border. Note that this may result in some bugs.
- Some overscan border dimensions have, at least hopefully, been corrected.
Although this is a small update, the change should be noticeable visually.
- The aspect ratio of 4:3 is applied on the visual contents *without* the
overscan border now, which differs from the situation in the previous versions.
Again, this may result in bugs.
- In addition to the -egacga argument, used for emulating the behaviors of the
EGA with a CGA monitor, a new -ega argument has been added. It serves the
purposes of -egacga, just with an EGA monitor.
- SDL2.dll has been updated again (to v2.0.1). This may be the last time such
an update is ever mentioned in the changelog.
- A few more fixes have been applied to the codebase. Furthermore, A few minor
fixes and other modifications have been applied to the README file.

August 18th, 2013 (v0.8.8):
- While it probably won't matter for many users of Chocolate Keen, the
licensing terms have been updated, so the whole application is (re)licensed
under the GNU GPL v2 and any later version now. Previously, only code sections
taken off the Commander Genius project had been marked as such.
- Now that SDL 2.0.0 is released, the bundled file SDL2.dll has been updated
yet again.
- This release has several changes regarding graphics handling. In fact, some
of these are not far from a rewrite. While various drawing functions have not
really changed, the situation is different when it comes to (emulated) video
mode changes, fullscreen toggling and more. These changes include restructuring
of code and some optimizations. A few specific changes are to be listed here.
- If OpenGL output and bilinear interpolation are both requested, a framebuffer
object (or FBO) is used for doing a first-step zoom (optionally based on an
input to the -scalef argument) by a GPU, if supported. The older behavior of
software zooming is applied if this approach is unsupported or the -nofbo
argument is specified. Basically, the FBO lets one use GPU resources as an
alternative to system/CPU resources, especially with a large scaling factor
(say 3 or more).
- Similarly, if an SDL 2.0 build is run and the SDL 2.0 renderer is requested,
along with bilinear filtering, then a secondary texture is used as a render
target for hardware zoom.
- When text mode is in use (e.g. when quitting via an in-game dialog),
a copy of the textual contents stored in (emulated) EGA memory is cached.
This is done so actual graphical updates are done only when ones are required.
It results in a significant decrease in CPU usage.
- When SDL 2.0 is used and a software surface is requested (using -surface),
less system memory should be consumed, at least on Linux with X11. While it may
be possible to take advantage of hardware acceleration in some way even with
the -surface argument being specified (with the cost of more memory required),
it seems like this is far from useful. -opengl or -texture should be the way
to go for hardware acceleration anyway (with SDL 2.0).
- Although not exactly tested, Chocolate Keen tries to revert to a software
surface in a few cases of problems. There is probably more to do, though.
- A dedicated function used for printing a warning or error to console has
been added, used as an alternative to direct calls to printf and fprintf.
For now this is not more useful than before.
- A long standing sound bug has finally been fixed. It is the case that the bug
has not always been reproduced, but whenever it has, it would be noticeable
during the whole Chocolate Keen session, at least with specific sound effects.
It is basically the result of a few chunks of memory, used for storing sound
effects, not getting initialized.
- Another minor sound bug has been fixed, based on the difference between
a signed mathematical operation (multiplication by -1) and a seemingly
similar unsigned operation (with a clearly different meaning).

July 29th, 2013 (v0.8.6):
- The bundled SDL2.dll file has been updated, solving a minor sound bug
in case the SDL 2.0 path is chosen on the Windows platform.
- A new "Vorticons Keen demo mode" has been implemented. Basically, it lets
you record and playback demos, as do vanilla Keen 4-6 (although Keen 4-6
can't just playback new demos without importing these in some way). It has
also been implemented as a patch for vanilla Keen 1-3 a bit earlier, so these 
can be used for finding NON-vanilla bugs in Chocolate Keen. It is actually
the case that the vanilla Keen executables (without patching) contain
some unused code handling demos. The patch takes advantage of that.
- As a consequence of this, a fix to a function used for setting "game ticks"
(a kind of a moment measured in internal time units) has been applied.
- Furthermore, a typo in a function responsible for calculating jump heights
(and more) has been fixed, again as a result of demo playback tests.
- On an unrelated note, a minor fix to the main menu code has been applied.
- A minor error in the vorticon walk function has also been fixed
(before implementing demo playback). It's again all about == vs = in C...
- Furthermore, yet another instance of invalid memory access and a possible
memory corruption in some drawing function has been fixed, reproduced during
the Keen 3 finale when an image loaded from a file to a chunk of memory
generally used for map data is to be displayed. To anybody wondering about
the usage of map data storage for an image, that IS vanilla behavior.

July 7th, 2013 (v0.8.4):
- Fixed a clear memory leak that may repeat while OpenGL is in use.

July 5th, 2013 (v0.8.3):
- The codebase has been migrated to C (to be more specific C99). This may be
the cause of instabilities, so you have been warned. Note that it is still
possible to build an executable using a C++ compiler (as C++ code).
Furthermore, as an interesting consequence this migration has seemed to reduce
the executable file size. For instance, mingw-w64 has constructed an executable
of size 200704 bytes for v0.8.2, compared to 174592 bytes for v0.8.3.
Furthermore, the executable constructed by mingw-w64 as C++ code requires a few
additional dependencies as shared libraries (libgcc_s and libstdc++). In fact,
for that exact reason the original MinGW has been used so far. If you check the
executable as found in the v0.8.2 ZIP archive file, its size is even larger
(283648 bytes). That may be the case due to some additional code required for
C++ support as an alternative to functionalities loaded from such libraries.
- A minor fix (if not more) regarding the input mapper has been integrated.
- Some fixes regarding SDL 2.0 fullscreen handling have similarly been applied.
Furthermore, in case SDL 2.0 is used, Chocolate Keen should be ready for
the possibility of fullscreen window resizes (tested with the Unity shell).
- So far, the game's visual contents have filled the screen with aspect
correction by default, unless "-surface" has been specified. This has changed
and software scaling is used for that now (again if "-surface" is specified).
- More generally, some changes have been applied regarding the command line
arguments and scaling. To summarize, one can say that "-scale" has been renamed
to "-scalef", "-noaspect" has been replaced with "-scalet" and a new optional
command line argument identified by "-forcecutfullscreen" has been added. For
more details, as usual one can type something like this: "chocolate-keen -?".

June 22nd, 2013 (v0.8.2):
- The Chocolate Keen executable name (with no extension or episode number) has
been renamed chocolate-keen, i.e. a minus sign is used as a separator rather
than an underscore. This seems to be more commonly used in one environment and
also corresponds with the executable name of the Chocolate Doom source port.
- Currently experimental feature: Chocolate Keen can target SDL 2.0 now, in
addition to SDL 1.2 as before. Note that while SDL 2.0 is almost ready for an
official release, it is still in the stage of a "Release Candidate" as of now
and some problems are expected.
- While -overlay is *not* available when targeting SDL 2.0, a new -texture
argument has been added. Two more SDL 2.0 related arguments added are -renderer
(to be used with -texture) and -displaynum (currently untested).
- In the SDL 2.0 case, -texture is the default. Otherwise it is -opengl, unless
GL support is disabled in the executable, where -overlay is the default.
- DOSBox-style input mapping has been implemented, with current values saved
using Ctrl+F8. For the ones who want to take advantage of a game controller,
either out of preference or simply since there's no nearby keyboard, this
can be used to map controller buttons to emulated (102-key QWERTY layout)
keyboard keys.
- Note that while the mapper file format may be similar to the one used by the
DOSBox emulator, it is NOT identical. (!)
- Some code handling (emulated) pause key in a special manner has been removed
for now, as it often, well, hasn't done its job.
- Windowed and fullscreen resolutions are now separated: When -surface is *not*
used, these can be specified with -windowres and -fullres, respectively.
- As a consequence, these black bars known for aspect correction are no longer
shown while displaying a non-full window. Basically, when specific window
dimensions are chosen with -windowres, one can think of such imaginary bars
getting removed from the window.
- As a new default, the window resolution is (in each dimension) a specific
ratio of the desktop resolution now (imaginary bars included). Guess which...
- On a relevant note, there are chances that -surface, combined with fullscreen
and SDL 2.0, is going to fail, due to a failure to set a specific screen
resolution. In that case, "-texture -renderer software" can be used for
software rendering with nearest neighbor interpolation for scaling.
- Windows OS specific: US keyboard layout is now forced for the Chocolate Keen
application. Among other things AltGr should behave like Right Alt, rather than
Ctrl+LAlt. Thanks to Tesseract for reporting, along with helpful assistance!

May 12th, 2013 (v0.8.0):
In a similar fashion to what has recently been done for v0.7.0, support for
Commander Keen 3 v1.31 is now built-in! As a result, Chocolate Keen has full
support for the Invasion of the Vorticons trilogy.
- Even before the addition of Keen 3 support, some more code restructuring has
been added in regard to multiple episode support (and possibly more).
- An example for an experimental decision out of this restructuring:
For now a single executable is used, supporting all episodes. It is still
possible to compile one per episode. Maybe it will be the default setup again
later, similarly to the situation with the source codes for Wolfenstein 3D and
various source ports of it. For now, though, it is not the case.
- An icon is now set for the Chocolate Keen applications. Thanks to Lemm for
suggesting the chocolate bar!
- A few changes have been applied in regard to the timing yet again. As a side
note, it worths to mention that (also applying to earlier releases),
if Chocolate Keen is being run for sufficiently long (let's hope it implies,
say, 12 hours at least), then there IS the possible risk of a timing overflow
glitch occurring at some point. Not sure there's an easy way to take care of
that. Maybe such a way will be found at some point, though.
- A new command line argument has been added: "-uselastinputdevice". On vanilla
Keen 1-3, if you setup a joystick within the game (including calibration), use
it for a bit, quit the application and then restart it, keyboard input will be
forced. Since Chocolate Keen aims to act like vanilla Keen, by default it does
the same thing. With the above argument it will rather use the last selected
input device stored in the CTLPANEL file, if it is found.
- In what may seem to be a controversial decision, the (host) mouse cursor is
now LOCKED in the application's window by default, when fullscreen is toggled
on. To release, use the key combination Ctrl+F10 (also the default in DOSBox
for the same functionality). This works for a non-fullscreen window as well.
You may further (re)lock the cursor by clicking on the window or using the
key combination Ctrl+F10 (again as usually done in DOSBox).
There are a few reasons for doing this. A later point may describe one of them.
- A new "-nocursorlock" argument has been added. When passed to Chocolate Keen,
the mouse cursor will NOT be locked on startup, nor when toggling fullscreen
on/off. As usual it can be locked using the Ctrl+F10 key combo or by clicking
on the window.
- Another related argument, "-alwayshidecursor", has further been added. If one
wants the mouse cursor to not appear in the application's window even while it
is unlocked, this can be used.
- For convenience and/or a case of emergency, the keyboard combination
Ctrl+F9 (again familiar for DOSBox users) can be used for shutting down
Chocolate Keen now.
- A bit of an obscure fact: The vanilla Keen 1-3 executables do contain some
code for (possibly not-comfortable-to-use) mouse input. It might be a variation
of the code from the PC version of Dangerous Dave. While it is possible to
manually edit the CTLPANEL file to use a mouse as the input device,
keyboard input is always forced on startup as mentioned above. Now, however,
Chocolate Keen has the -uselastinputdevice argument. So, similar mouse
support has been implemented. Chances are you wouldn't like to use it every
day, though, so no special command line argument has been added for enabling
the mouse. If you want it, open the CTLPANEL file with a hex editor and set
bytes 4-5 (counting from 0) to the values of 01 00 (i.e. the Little Endian
16-bit value of 1). Afterwards, load Chocolate Keen with -uselastinputdevice.
- It should be noted that, while not commonly used by a single individual,
a secondary game controller can also be used now, again with the help
of -uselastinputdevice. A simple change of the controller type (to 3)
in the CTLPANEL file is possibly not sufficient, though, as a few
calibration-related values should be set. Furthermore, the secondary
controller's buttons can NOT be used to halt the discussion during the
encounter with the Grand Intellect in Keen 3 (which IS vanilla behavior).
- Chocolate Keen can now be compiled in a different "mode" for debugging.
Basically, using the current Makefile, when running an executable built with
something like "make DEBUG=1", the mouse cursor is NEVER locked. Furthermore,
in such a case the cursor is ALWAYS displayed, unless -alwayshidecursor is
added as an argument. The usual cursor lock toggle still works as a toggle for
using the mouse in-game at all, though.
- Fixed a bug or two in regard to joystick support.
- More bug fixes, here and there.

April 11th, 2013 (v0.7.0):
Thanks to Lemm, Commander Keen 2 v1.31 is now supported!
- As a consequence, the project has been renamed "Chocolate Keen" (as a
substitute for the name of... "Chocolate Keen1").
- Technically, the code has been restructured so Keen 2 support can be added.
More restructuring may be done later.
- Some more changes have been applied, mainly to the code layout.
- A few timing related modifications have been applied, done for a hopefully
better accuracy among more platforms. There is still more to improve, though.
For instance, sometime after activating a Tantalus ray it may be impossible to
toggle on/off fullscreen mode for a little while.
- A buffer overflow has been fixed, where the high scores table is displayed.
Actually, it MAY still be produced - if there is a score of 1,000,000,000 or
more... however, it is apparently a problem even on vanilla Keen.
- Fixed drawing of masked tiles. (These weren't really revealed on Keen 1,
while they are much clearer on Keen 2.)
- Fixed a bug in the bridge code (again mostly noticeable on Keen 2).
- An alternative keyboard key chosen for the pogo is actually being set now.
- A few more fixes, here and there.
- There are also (very) minor edits to the changelog within the README file.

November 23rd, 2012 (v0.6.0):
In terms of simulating Commander Keen 1 v1.31, this should be a
feature-complete release (ignoring a few potential issues).
- It is now considered a "Beta" release (rather than "Alpha"), since it has
probably been relatively stable for a while.
- Basic 80-column text mode emulation is implemented! This is used for a little
while when Keen 1 is loaded, as well as for the textual contents seen when
quitting vanilla Keen 1.
- The command line arguments can now be shown by Chocolate Keen1 itself
on ALL platforms, taking advantage of the text mode emulation.
Hence, at least for now these have been removed from the README file,
since any update to the arguments may easily be missed anyway...
- The command line argument -ega has been renamed to -egacga and it now does
more things than changing the emulated video card's refresh rate. Among these,
a larger overscan area is used on the 200-line graphics mode (as observed with
an actual EGA). The text mode has also got a 200-line height in pixels
(which is compatible with CGA monitors). Somehow, the fact that the observed
behavior does not correspond to the EGA when configured to work with an EGA
monitor has been spotted just a little before releasing this version...
At least, though, things like the overscan dimensions should be similar.
By default, plain VGA behavior is emulated (combined with a VGA monitor).
- A dummy /K (or /k) argument has been added. On vanilla Keen 1 it is used to
solve keyboard-related issues on some BIOS variants, while on Chocolate Keen 1
it simply adds a message on the very first few milliseconds of loading.
- Workaround for OpenGL glitches has been removed, since it can't be used now
as-is with the changes between text and graphics modes. Well, almost.
It is still in use when just toggling fullscreen mode, although it may not help
as much as it has before.
- Vanilla-style joystick support is implemented. Note that there are VERY great
chances that you wouldn't like the way it works, since it currently determines
which pair of game controller buttons will be used, as well as a pair of axes.
Oh, and the joystick needs to be calibrated, just like on vanilla Keen 1. The
game-port way of game controllers was very problematic anyway...
Ah, one more thing:
- OK, a minor bug dating back to the beginning of the game "porting" process
has been fixed. What it implies now is that the controller type to use is reset
to the keyboard whenever Chocolate Keen1 is loaded. This is also the behavior
of vanilla Keen1. Maybe it was done on purpose due to the great issues with
joysticks back then...
- Another such bug: Now after the F3 key is pressed to reconfigure the keys,
the keyboard is what is used for input (if joystick has just been used).
Note that this is the vanilla behavior, and the bug was that the joystick would
still be in use afterwards. Truly, it hasn't had a great impact beforehand...
- The sound sample rate used internally can now be set from the command line,
using the -srate argument. The current default is now 48000Hz, while before the
release of the current version it has been 44100Hz.
An example for a command:
chocolate_keen1 -srate 44100
- With a few exceptions, stdint.h is used now for numeric types of specific
sizes (such as uint16_t), rather than types defined by SDL (like Uint16). While
there should really be no reason to abandon SDL, it is still better to avoid
the usage of some stuff not directly related to SDL, if it can be done.
- Oops... the exe name is "chocolate_keen1" now, not "chocolate-keen1"!
Corrected a few references mentioning the latter one.
- There are some more changes on the technical side.

November 14th, 2012 (v0.4.12):
- Dependency on SDL_mixer has been removed, and SDL is used for audio directly
in this version.
- More shortcut keys are implemented! These consist of F1 for the help text,
F2 for sound toggling and F3 for reconfiguring keyboard assignments.
It is a good time to note that the -nosound argument still works, although it
makes a few things shorter since they usually wait for a sound playback to
complete. The level completion sequence is an example.
- The "clear keys" functionality hasn't done a vital part of its job, hence
toggling off the scorebox with the "Space" key hasn't worked as expected
(basically displaying another one immediately). That is fixed now.
- Another keyboard related change:
The keyboard's "repeat rate" and "repeat delay" are now emulated, with specific
values chosen for these. Hold a key for about half a second, and then multiple
presses of the same key are going to be simulated (but not releases of it).
- Settings like keyboard assignments and sound toggling are now actually saved
on quit! Truly, the bug was irrelevant before since none of these could be
changed up to the current revision...
- Alright, Looks like the time measurement is more accurate now. Or at least,
when displaying two instances of the finale sequence side by side: One by
Chocolate Keen1 and the other from a captured DOSBox video, they actually tend
to stay in sync way more than beforehand!
The fix? Well, it has already occurred before with the general game timer, and
now again for the simulation of VGA vertical retrace: Force the usage of 64-bit
values for calculations due to (relatively) large numbers involved...
A bit more info: The simulation needs to know the refresh rate the game should
"think" it has. Online specs tell that, at least on a VGA card, 70.086Hz is
the refresh rate for the 200-line EGA graphics mode used by Keen 1, while VLC
reports a frame rate of 70.086304fps for the captured video from DOSBox.
For now the rate of 70.086Hz is simulated.
- On a relevant note, while it is easy to think the graphics mode on EGA and
VGA cards looks identical on both, there are in fact a few differences. One of
them is the refresh rate. It can now be selected using the -ega argument,
picking a rate of 59.92Hz (rather than the default of 70.086Hz).
- There are a few additional minor fixes again.

November 11th, 2012 (v0.4.7):
- Scrolling text functionality is now fully implemented.
The help text is still not there, though.
- The ability to show an image from a file is also implemented!
This includes the Keen 2 and 3 previews within Keen 1.
- It has happened after all... The Keen 1 finale can now be watched!
- As a consequence, a minor bug in the image loading routine has been spotted
while originally implemented. Upon fixing it, though, it has resulted in a
crash... Why? Well, on vanilla Keen the image is temporarily loaded to the
map data (!!), but the fix wants to update the screen. This includes an update
to the on-screen map tiles. So, as a workaround/fix, no bogus tile update
is done at all now.
- Switches for lights, bridges and tantalus rays are now implemented. While out
of all absolutely unmodified Keens these are found on Keen 2 only, relevant
code is also found on Keen 1. The light switches can easily be tested by
modifying a level. For the two other switches, tile types need to be adjusted.
Note that while the Tantalus Ray switch appears to show the sequence with the
screen shaking as expected, it does not result in a "game over" like on Keen 2.
Rather, it returns to the world map with a black screen, being the result of
a "fade out" without a following "fade in". It is the case in the original
DOS version of Keen 1 with minor modifications to the tile properties, though.
- An ice cube shot from a cannon is now split into four smaller cubettes
when it hits something (i.e. the original behavior), instead of two.
- An unpacked KEEN1.EXE file can actually be used now.
- A workaround for OpenGL border glitches which may appear on window/fullscreen
mode switching within X11 is now in effect. At least, it works locally on
GNU/Linux with X11. On Windows it may not occur since SDL resets the OpenGL
context upon video mode switching, while it is not done on GNU/Linux with X11
(apparently).
- A few more minor fixes/changes.

November 3rd, 2012 (v0.4.1):
- Fixed a couple of bugs in regard to the garg, mainly with the jumps.
- The "chocolate_keen1_full_overlay" scripts are actually removed from the
source tarball now.

October 31th, 2012 (v0.4.0): A secondary release.
- Three numbers are used now for the version numbering;
Although, I can't think of that many (theoretical) future changes right now...
- OpenGL support has been implemented. It can be disabled at compile time.
- Hopefully fixed a serious potential cause of several memory corruptions, even
though these do not always occur, or they occur on very specific setups.
Basically, the allocation of a quite LARGE object on the STACK, in contrary to
(not-too-many) smaller ones, is a pretty BAD idea!!! Of course, it may be
possible to increase the stack size, but it seems more portable to avoid this.
- A good reminder has come that, by default, SDL redirects text output to
stdout.txt and stderr.txt on Windows. Most importantly, this means that the
list of command line arguments is not seen if the game is loaded from a command
prompt. So, now the arguments are simply given in the README.TXT file.
- Changes have been done to the command line arguments and how they're used. A
few arguments are replacements of others, while the rest are actual additions.
- Fullscreen is now the default, along with a scale (zoom) level of 2.
- OpenGL is used as a default. If Chocolate Keen1 is compiled without OpenGL
support, then Overlay is the default.
- The desktop resolution is chosen for the display dimensions by default.
That role does not apply if a software surface is used for output, though.
- As a result of changes regarding the video mode (like fullscreen by default),
the chocolate_keen1_full_overlay.bat and chocolate_keen1_full_overlay.sh
scripts are not bundled anymore.
- Fixed the Keen death sequence in the case Keen falls to a pit
(below the level's bottom).
- The correct overscan border color should now be used during gameplay.
At least, it is probably the case with the vanilla unpatched Keen behaviors.
- When a raygun is collected, 5 additional charges are added to the current
arsenal, rather than replacing it.
- Fixed a few more cases of potential memory corruptions. While on vanilla Keen
such corruptions are found, we simply cannot accept it here. It has seemed to
resulted in a few "mysterious" crashes with OpenGL and video mode resets
(fullscreen versus windowed).
- A few more minor technical changes, here and there...

October 27th, 2012 (v0.3): Initial release. Most things should work, although a
few of them (like the end-of-game sequence and some shortcut keys) are missing.

-------
License
-------

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

------------------------------------
Building Chocolate Keen from sources
------------------------------------

Development files for SDL 1.2 or 2.0 are required, as well as GNU make and
the GNU Compiler Collection's C compiler (i.e. gcc). In practice, at the moment
only "make" and "gcc" are probably required. If nothing goes wrong, a
single "make" command should build the executable. "make clean" removes any
generated object or executable file, assuming the *exact* same arguments have
been passed to "make" as before, with the exception of "clean".
To build without OpenGL support, add USE_OPENGL=0 as an argument to "make".
You may also force usage of g++ by adding BUILDASCPP=1 as an argument.

To target SDL 1.2 (instead of SDL 2.0, the default since v0.9.3),
add SDLCONFIG=sdl-config (or similar, whatever is relevant) as an argument.

Note that a single chocolate-keen executable supporting all episodes is
built by default. You may also build a stripped executable that supports
just one episode. For instance, for building chocolate-keen1 with exclusive
support for Keen 1 pass SINGLE_EPISODE=1 as an argument to "make".

Note that "make veryclean" may work better in case of troubles related to
single episode support (e.g. if you suddenly want to use SINGLE_EPISODE).

crossbuild_mingw32.sh and crossbuild_mingw-w64.sh have been used to
cross-build 32-bit Windows executables from GNU/Linux, using MinGW and
MinGW-w64 respectively. Note that as of version 0.8.8, the additional
header file windows/include/GL/glext.h is used for FBO support.

# ETe

[![build](https://github.com/etfdevs/ETe/actions/workflows/ci.yml/badge.svg)](https://github.com/etfdevs/ETe/actions/workflows/ci.yml) * <a href="https://discord.com/channels/253600486219972608/401475882897899523"><img src="https://img.shields.io/discord/253600486219972608?color=7289da&logo=discord&logoColor=white" alt="Discord server" /></a>

This is a modern Wolfenstein: Enemy Territory engine aimed to be fast, secure and compatible with many existing ET mods and etmain.
It is based on the [Quake3e](https://github.com/ec-/Quake3e) engine which itself is based on the last non-SDL source dump of [ioquake3](https://github.com/ioquake/ioq3) with latest upstream fixes applied. Several common fixes from upstream [ET:Legacy](https://github.com/etlegacy/etlegacy) also are applied as needed for security.

*This repository does not contain any game content, so in order to play you must copy the resulting binaries into your existing Wolfenstein: Enemy Territory installation*

**Key features**:

* optimized OpenGL renderer
* ~~optimized Vulkan renderer~~ **NOT YET**
* raw mouse input support, enabled automatically instead of DirectInput(**\in_mouse 1**) if available
* unlagged mouse events processing, can be reverted by setting **\in_lagged 1**
* **\in_minimize** - hotkey for minimize/restore main window (win32-only, direct replacement for Q3Minimizer)
* **\video-pipe** - to use external ffmpeg binary as an encoder for better quality and smaller output files
* improved server-side DoS protection, much reduced memory usage
* raised filesystem limits (up to 20,000 maps can be handled in a single directory)
* reworked Zone memory allocator, no more out-of-memory errors
* non-intrusive support for SDL2 backend (video,audio,input), selectable at compile time for *nix via CMake
* tons of bug fixes and other improvements
* basic Discord Rich Presence support (win32-only currently, linux support to follow when 64-bit support is ready)

## Vulkan renderer

***NOT YET AVAILABLE***

## OpenGL renderer

Based on classic OpenGL renderers from [idq3](https://github.com/id-Software/Quake-III-Arena)/[sdet](https://github.com/id-Software/Enemy-Territory)/[ioquake3](https://github.com/ioquake/ioq3)/[cnq3](https://bitbucket.org/CPMADevs/cnq3)/[openarena](https://github.com/OpenArena/engine), features:

* OpenGL 1.1 compatible, uses features from newer versions whenever available
* high-quality per-pixel dynamic lighting, can be triggered by **\r_dlightMode** cvar
* ~~merged lightmaps (atlases)~~
* static world surfaces cached in VBO (**\r_vbo 1**)
* offscreen rendering, enabled with **\r_fbo 1**, all following requires it enabled:
* `screenMap` texture rendering - to create realistic environment reflections
* multi-sample anti-aliasing (**\r_ext_multisample**)
* supersample anti-aliasing (**\r_ext_supersample**)
* per-window gamma-correction which is important for screen-capture tools like OBS
* you can minimize game window any time during **\video**|**\video-pipe** recording
* high dynamic range render targets (**\r_hdr 1**) to avoid color banding
* bloom post-processing effect
* arbitrary resolution rendering
* greyscale mode

Performance is usually greater or equal to other opengl1 renderers

## OpenGL2 renderer

Original ioquake3 renderer, not available and unlikely to be ported

## Missing ioquake3 features

* OpenAL(-soft) support
* VoIP support (probably never going to add since Discord etc)
* Extra sound codecs like ogg and opus

## Build Instructions

### windows/msvc

Install Visual Studio Community Edition 2017 or later and compile all projects from solution

`src/wolf.sln`

Default settings are using Visual Studio 2017 with XP compatible toolset

Only 32-bit binaries compatible until 64-bit mods become available and a 64-bit etmain is available

Copy resulting exe from `src/[Debug|Release]` directory and `src/ded/[Debug|Release]` directory

~~To compile with Vulkan backend - clean solution, right click on `wolf` project, find `Project Dependencies` and select `renderervk` instead of `renderer`~~

---

### ~~windows/mingw~~

TODO

~~All build dependencies (libraries, headers) are bundled-in~~

~~Build with either `make ARCH=x86` or `make ARCH=x86_64` commands depending from your target system, then copy resulting binaries from created `build` directory or use command:~~

~~`make install DESTDIR=<path_to_game_files>`~~

---

### linux/bsd

[CMake](https://cmake.org/download/) is required to generate make files for this project under *nix

Debian or Arch based distros:
```
cd src
./linux_build.sh
```

Build artifacts ete.x86 and ete-ded.x86 are then found under the build directory.

Only 32-bit binaries compatible until 64-bit mods become available and a 64-bit etmain is available

The provided 32-bit cross compile toolchain is provided on this repository.

Several options available for linux builds with CMake:

`BUILD_CLIENT=ON` - build unified client/server executable, enabled by default

`BUILD_DEDSERVER=ON` - build dedicated server executable, enabled by default

`USE_SDL2=ON` - use SDL2 backend for video, audio, input subsystems

`USE_SYSTEM_JPEG=ON` - use current system JPEG-turbo library, enabled by default

---

### raspbery pi os

TODO

---

### macOS

* install the official SDL2 framework to /Library/Frameworks
* build should work similarly to linux/bsd section
* aarch64 compilation not tested at all due to lack of device or Github Action support
* Need testers and someone to help finish supporting the ET mac filesystem differences with regards to packaging and handling of mod binaries
* Probably will skip PPC support but 32/64/aarch64 separate and universal would be ideal.

## Contacts

Discord channel: https://discordapp.com/invite/hsDQVby

## Links

* https://github.com/ec-/Quake3e
* https://github.com/etlegacy/etlegacy
* https://bitbucket.org/CPMADevs/cnq3
* https://github.com/ioquake/ioq3
* https://github.com/kennyalive/Quake-III-Arena-Kenny-Edition
* https://github.com/OpenArena/engine

# Building the Vita port

The repository includes the actual modified C++ core as ordinary files under src/Cpp, not an unresolved submodule. It also includes prepared launcher/LiveArea assets and source snapshots of vitaGL, vitaShaRK and SceShaccCgExt. Exact revisions: docs/UPSTREAM_REVISIONS.json.

## Environment

Use Linux or WSL Ubuntu, with VitaSDK installed at /opt/vitasdk and CMake, Ninja, Make and a C/C++ host compiler. Required VitaSDK libraries: SDL2, vitaGL, vitashark, SceShaccCgExt, taihen and mathneon, plus the SDK platform stubs. The working development environment used SDL2 2.32.8. Follow the VitaSDK project installation instructions; this repository does not bootstrap an entire SDK automatically.

To build the included graphics dependency sources after installing SDK package dependencies:

```sh
bash tools/build_graphics_dependencies.sh
```

vitaGL uses HAVE_CUSTOM_HEAP=1 HAVE_SHARK_LOG=1 NO_SPLASHSCREEN=1. A clean vitaGL build is necessary when changing these options; the provided script uses a dedicated vendored copy. No proprietary shader compiler module is included.

## Compile

From the repository root:

```sh
bash vita/build.sh
```

Output: build/artifacts/release-c1/nfs2se-vita-v1.vpk. The script compiles and packages; it does not install to a console. Change public metadata in vita/release.env. Keep data paths in vita/platform.c, vita/launcher.c, src/Kernel32.c and src/Wrapper.c coordinated if changing the data directory; changing TITLE_ID alone does not require moving data.

The source archive matches the development build's source files. The public layout uses its own build/vita directory. The v1 binary was compiled using the established SDK environment; a clean-machine dependency bootstrap has not been validated.

## Artwork

Prepared files are already included. With Python 3 and Pillow, run these only when updating source artwork:

```sh
python3 vita/prepare_assets.py
python3 vita/prepare_livearea.py
```

These read launcherdes/MENU.PNG and IMAGES/release-c1 respectively. Original PNGs are preserved; LiveArea copies are reduced to indexed 256-color PNGs.

## Host tests

```sh
bash tools/test.sh
```

The tests cover config persistence/bounds, menu/race/result control transitions and launcher navigation. They do not replace hardware testing. Legacy translated-core and thread-affinity compiler warnings are known; they are not proof of a runtime failure.

Package metadata must use `APP_VERSION="01.00"` for public version 1.00. VitaSDK requires exactly two digits, a dot and two digits. The build script rejects invalid versions and application IDs before compiling.

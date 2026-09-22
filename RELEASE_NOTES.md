# NFS II SE Recompiled v1.00 — PS Vita

The first public release of **NFS II SE Recompiled** brings Need for Speed II: Special Edition to PS Vita as part of **Classic NFS Saga for PS Vita**.

Built on **zaps166's NFSIISE wrapper and C++ game translation**, this adaptation uses **vitaGL** for GPU rendering through the game's Glide interface. Gameplay, controls, installation and the custom LiveArea presentation have been tested on real hardware.

## Included in v1.00

- Single-player racing, menus, sound and video playback.
- A custom launcher with game-language selection, display settings and configurable controls.
- Classic and modern control presets, with separate menu and race mappings.
- Internal resolutions of 320x240, 480x360 and 640x480.
- Original 4:3 or stretched full-screen presentation.
- Optional FPS counter, VSync and CPU/GPU performance profiles.
- Custom bubble, LiveArea and startup artwork.

## Download and install

**Players need both files:**

1. `nfs2se-vita-v1.vpk` — install with VitaShell.
2. `nfs2se-vita-runtime-v1.zip` — extract and copy its `ux0/data/nfs2-recomp` folder into `ux0:data/` on the Vita.

Then copy your own PC **Need for Speed II: Special Edition** `fedata` and `gamedata` folders into:

```text
ux0:data/nfs2-recomp/game/
```

All game-data files and directories must have lowercase names. The standard edition is not supported. The port also requires `ur0:data/libshacccg.suprx`. Original game data and the shader compiler module are not included.

See the repository README for the complete installation guide and data preparation helper. Launch **NFS II SE Recompiled**, choose your settings and press Start or select **Guardar y jugar**. Hold L when opening the app to show the launcher again if hidden. Launcher labels are currently in Spanish; game language is configurable.

## Performance and limitations

Reported performance is approximately **15–50 FPS**, depending on the scene and settings. This release does not guarantee stable 40 or 60 FPS. Improving the slowest scenes is the next NFS II milestone.

Multiplayer is disabled. Full-screen mode stretches the original image; true widescreen, HD texture packs and proportional analog steering are not implemented.

## Source code and contributions

The repository contains editable source files and build instructions so developers can fork, inspect and contribute. `nfs2se-vita-source-v1.zip` provides a versioned source snapshot; players do not need it to install the game. `SHA256SUMS.txt` contains checksums for all three release downloads.

Please preserve upstream credits and applicable licenses. For bug reports, include the track, car, display settings, control profile, reproduction steps and `boot.log` from `ux0:data/nfs2-recomp/`.

## Classic NFS Saga for PS Vita

- **NFS II SE:** first playable release; continued performance and usability work.
- **NFS III: Hot Pursuit:** separate port research and graphics adaptation in progress.
- **NFS IV: High Stakes:** a future investigation after NFS II and III.
- **Porsche Unleashed / Porsche 2000:** a longer-term feasibility goal.

These are development goals, not release-date promises. This package runs NFS II SE only.

## Thanks

Thanks to **zaps166 / Błażej Szczygieł**, **Rinnegatamante and the vitaGL contributors**, **vitaShaRK**, **SceShaccCgExt**, **VitaSDK**, **SDL2**, and everyone whose work makes this port possible. Full upstream links, revisions and license notices are in the repository.

Need for Speed and its original game content belong to their respective owners. This is an unofficial community project, unaffiliated with Electronic Arts or Sony.

# Attribution and license scope

The root LICENSE is the original MIT wrapper license from zaps166/NFSIISE and retains its copyright notice. As its upstream README states, that license covers the wrapper only. It does not relicense the translated original game code, commercial game assets, branding, artwork or independently licensed dependencies. This repository makes no claim that the entire game is MIT-licensed.

The modified NFSIISE-CPP translation is included with its upstream notices and recorded revision. The user must supply original PC Special Edition FEDATA/GAMEDATA. Those directories, the PC executable, saves and the proprietary libshacccg.suprx module are not distributed in this release.

Dependency source and original license texts:

- third_party/vitaGL: COPYING and COPYING.LESSER.
- third_party/vitaShaRK: LICENSE and retained source notices.
- third_party/SceShaccCgExt: LICENSE and retained source notices.

SDL2, mathneon, taihen and VitaSDK libraries are provided by the build environment under their respective upstream terms; source projects are https://github.com/vitasdk/SDL, https://github.com/Rinnegatamante/math-neon, https://github.com/henkaku/taiHEN and https://github.com/vitasdk. The linked-library versions depend on that environment. Refer to BUILDING.md and the included source revision record when rebuilding.

Original trademarks and game content belong to their respective owners. Project artwork is supplied for this port and is not covered automatically by the wrapper's MIT license. Preserve upstream attribution when redistributing source or binaries.

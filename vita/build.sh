#!/bin/bash
set -euo pipefail
export VITASDK=/opt/vitasdk
export PATH=/opt/vitasdk/bin:/usr/bin:/bin
root="$(cd "$(dirname "$0")/.." && pwd)"
cd "$root"
vita=vita
source "$vita/release.env"
# Match the validation used by VitaSDK's vita_create_vpk helper.
if [[ ! "$APP_VERSION" =~ ^[0-9][0-9]\.[0-9][0-9]$ ]]; then
    echo "Invalid APP_VERSION: use two digits on each side, e.g. 01.00" >&2
    exit 1
fi
if [[ ! "$TITLE_ID" =~ ^[A-Z0-9]{9}$ ]]; then
    echo "Invalid TITLE_ID: expected nine uppercase letters/digits" >&2
    exit 1
fi
# Validate required packaged assets before spending time compiling.
for asset in assets/menu.rgba sce_sys/icon0.png sce_sys/pic0.png sce_sys/livearea/contents/bg0.png sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/template.xml; do
    if [[ ! -s "$vita/$asset" ]]; then
        echo "Missing asset: $vita/$asset" >&2
        exit 1
    fi
done
cmake -S "$vita" -B build/vita -G Ninja -DCMAKE_TOOLCHAIN_FILE=/opt/vitasdk/share/vita.toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build/vita -j2
out="build/artifacts/$RELEASE_NAME"
mkdir -p "$out"
vita-elf-create build/vita/nfs_zaps "$out/nfs_zaps.velf"
vita-make-fself -c -s "$out/nfs_zaps.velf" "$out/eboot.bin"
vita-mksfoex -s APP_VER="$APP_VERSION" -s TITLE_ID="$TITLE_ID" "$APP_NAME" "$out/param.sfo"
vita-pack-vpk -s "$out/param.sfo" -b "$out/eboot.bin" \
 -a "$vita/assets/menu.rgba=menu.rgba" \
 -a "$vita/sce_sys/icon0.png=sce_sys/icon0.png" \
 -a "$vita/sce_sys/pic0.png=sce_sys/pic0.png" \
 -a "$vita/sce_sys/livearea/contents/bg0.png=sce_sys/livearea/contents/bg0.png" \
 -a "$vita/sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png" \
 -a "$vita/sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml" \
 "$out/$VPK_NAME"
printf 'VPK: %s\n' "$out/$VPK_NAME"

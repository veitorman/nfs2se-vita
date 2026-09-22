#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/.."
export VITASDK=/opt/vitasdk
export PATH="$VITASDK/bin:$PATH"
cmake -S third_party/SceShaccCgExt -B build/shacc-ext -G Ninja -DCMAKE_TOOLCHAIN_FILE="$VITASDK/share/vita.toolchain.cmake" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_C_FLAGS=-std=gnu11
cmake --build build/shacc-ext
cmake --install build/shacc-ext --prefix "$VITASDK/arm-vita-eabi"
make -C third_party/vitaShaRK -j2 install
make -C third_party/vitaGL clean
make -C third_party/vitaGL -j2 HAVE_CUSTOM_HEAP=1 HAVE_SHARK_LOG=1 NO_SPLASHSCREEN=1 install

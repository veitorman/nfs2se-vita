#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")/.."
work="$(mktemp -d)"
trap 'rm -f "$work/input" "$work/config" "$work/launcher" "$work/settings.cfg" "$work/settings.cfg.bak" "$work/settings.cfg.tmp"; rmdir "$work"' EXIT
for name in input config launcher_ui; do
 gcc -g -fsanitize=address,undefined -Wall -Wextra vita/config.c "vita/${name}_test.c" -o "$work/${name/launcher_ui/launcher}"
 "$work/${name/launcher_ui/launcher}" "$work/settings.cfg"
done

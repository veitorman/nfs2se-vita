"""Prepare indexed LiveArea PNGs. Requires Python 3 and Pillow; does not compile."""
from pathlib import Path
import hashlib
import json
from PIL import Image

here = Path(__file__).resolve().parent
source = here.parent / "IMAGES" / "release-c1"
output = here / "sce_sys"
specs = {
    "icon0.png": ((128, 128), output / "icon0.png"),
    "pic0.png": ((960, 544), output / "pic0.png"),
    "bg0.png": ((840, 500), output / "livearea/contents/bg0.png"),
    "startup.png": ((280, 158), output / "livearea/contents/startup.png"),
}
prepared = []
# Validate all source images before changing packaged assets.
for name, (size, target) in specs.items():
    original = source / name
    with Image.open(original) as image:
        if image.size != size:
            raise ValueError(f"{name}: expected {size}, got {image.size}")
        if image.convert("RGBA").getextrema()[3] != (255, 255):
            raise ValueError(f"{name}: this release expects opaque artwork")
        rgb = image.convert("RGB")
        palette = rgb.quantize(colors=256, method=Image.Quantize.MEDIANCUT)
        indexed = rgb.quantize(palette=palette, dither=Image.Dither.FLOYDSTEINBERG)
    prepared.append((original, target, indexed))
manifest = {}
for original, target, indexed in prepared:
    target.parent.mkdir(parents=True, exist_ok=True)
    indexed.save(target, format="PNG", bits=8, optimize=True)
    manifest[original.name] = {
        "source": f"IMAGES/release-c1/{original.name}",
        "source_sha256": hashlib.sha256(original.read_bytes()).hexdigest(),
        "packaged_sha256": hashlib.sha256(target.read_bytes()).hexdigest(),
        "size": list(indexed.size),
        "encoding": "8-bit indexed PNG, 256-color palette, opaque, not interlaced",
    }
(output / "artwork.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
print("Prepared four LiveArea assets; source images unchanged.")

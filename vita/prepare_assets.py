"""Run with Python + Pillow after changing launcherdes/MENU.PNG."""
from pathlib import Path
import hashlib,json
from PIL import Image
here=Path(__file__).resolve().parent
source=here.parent/"launcherdes"/"MENU.PNG"
with Image.open(source) as im:
    if im.size!=(960,544):raise ValueError("MENU.PNG must be 960x544")
    pixels=im.convert("RGBA").tobytes()
assets=here/"assets"
assets.mkdir(exist_ok=True)
(assets/"menu.rgba").write_bytes(pixels)
(assets/"source.json").write_text(json.dumps({"source":str(source),"sha256":hashlib.sha256(source.read_bytes()).hexdigest(),"width":960,"height":544,"encoding":"RGBA8 top row first","guide":"launcherdes/safeareas.png"},indent=2))

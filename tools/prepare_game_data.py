"""Copy owned Special Edition FEDATA/GAMEDATA to a NEW lowercase destination.
Usage: python prepare_game_data.py /path/to/cd /path/to/new-game-folder
Then merge the result into ux0:data/nfs2-recomp/game. Originals are read-only.
"""
from pathlib import Path
import sys, shutil
if len(sys.argv)!=3: raise SystemExit(__doc__)
source=Path(sys.argv[1]).resolve(); dest=Path(sys.argv[2]).resolve()
if dest.exists(): raise SystemExit("Destination must not exist; choose a new folder.")
if source==dest or source in dest.parents: raise SystemExit("Use a destination outside the original backup.")
folders={p.name.lower():p for p in source.iterdir() if p.is_dir()}
plan=[];seen=set()
for name in ("fedata","gamedata"):
 if name not in folders: raise SystemExit("Missing "+name+" in the source folder.")
 for p in folders[name].rglob("*"):
  if p.is_symlink(): raise SystemExit("Symbolic links are not supported: "+str(p))
  if p.is_file():
   rel=Path(name)/str(p.relative_to(folders[name])).lower()
   if str(rel) in seen: raise SystemExit("Lowercase name collision: "+str(rel))
   seen.add(str(rel));plan.append((p,dest/rel))
for src,target in plan:
 target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src,target)
print("Copied",len(plan),"files to",dest,"; original backup unchanged.")

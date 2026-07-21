from pcgw.config import MANIFEST_PATH
from pcgw.storage import save_json


def export_manifest(cache):
    manifest = []
    for title, entry in cache.items():
        if entry.get("state") != "ok":
            continue
        if not entry.get("saves"):
            continue
        manifest.append({"page": title, "appid": entry["appid"], "saves": entry["saves"]})
    save_json(MANIFEST_PATH, manifest)
    return len(manifest)

import time

from pcgw.api import now_iso
from pcgw.config import CACHE_PATH, MANIFEST_PATH, META_PATH, REQUEST_DELAY_SECONDS, SAVE_EVERY
from pcgw.discovery import fetch_all_steam_linked_pages, fetch_recent_changes
from pcgw.manifest import export_manifest
from pcgw.storage import load_json, save_json
from pcgw.wikitext import extract_save_locations, fetch_wikitext


def _pages_to_fetch(only_appids, cache, meta, run_started):
    if only_appids:
        wanted = set(only_appids.split(","))
        to_fetch = [title for title, entry in cache.items()
                    if entry.get("appid") in wanted and entry.get("state") != "ok"]
        print(f"Targeted run: {len(wanted)} appids requested, {len(to_fetch)} have an un-fetched page in the cache.\n")
        return to_fetch

    if "last_checked_recent_changes" in meta:
        print(f"Checking for changes since {meta['last_checked_recent_changes']}...")
        changed_titles = fetch_recent_changes(meta["last_checked_recent_changes"])
        for title in changed_titles:
            if title in cache:
                cache[title]["state"] = "outdated"
        print(f"{len(changed_titles)} recent edits, "
              f"{sum(1 for t in changed_titles if t in cache)} affect cached pages.")

    print("Enumerating Steam-linked pages (cheap, but there are a lot of them)...")
    all_pages = fetch_all_steam_linked_pages()
    print(f"{len(all_pages)} Steam-linked pages known to the wiki.")

    new_count = 0
    for title, info in all_pages.items():
        if title not in cache:
            cache[title] = {"appid": info["appid"], "page_id": info["page_id"], "saves": [], "state": "outdated"}
            new_count += 1
        else:
            cache[title]["appid"] = info["appid"]  # keep appid current in case it changed
    print(f"{new_count} brand new pages queued for fetching.")

    meta["last_checked_recent_changes"] = run_started
    return [title for title, entry in cache.items() if entry.get("state") != "ok"]


def run(only_appids=None):
    cache = load_json(CACHE_PATH, {})
    meta = load_json(META_PATH, {})

    run_started = now_iso()
    to_fetch = _pages_to_fetch(only_appids, cache, meta, run_started)

    print(f"{len(to_fetch)} pages need fetching this run.\n")

    fetched = 0
    for title in to_fetch:
        print(f"[{title}] fetching wikitext...")
        try:
            wikitext = fetch_wikitext(title)
        except Exception as ex:
            print(f"  ! failed: {ex}")
            continue

        cache[title]["saves"] = extract_save_locations(wikitext)
        cache[title]["state"] = "ok"
        fetched += 1

        if not cache[title]["saves"]:
            print("  (no usable save location entries)")
        for s in cache[title]["saves"]:
            flag = "" if s["clean"] else "  <-- needs manual review"
            print(f"  {s['os']}: {s['path']}{flag}")

        if fetched % SAVE_EVERY == 0:
            save_json(CACHE_PATH, cache)
            count = export_manifest(cache)
            print(f"  [checkpoint: {fetched}/{len(to_fetch)} fetched this run, {count} games in manifest]")

        time.sleep(REQUEST_DELAY_SECONDS)

    if not only_appids:
        save_json(META_PATH, meta)
    save_json(CACHE_PATH, cache)
    count = export_manifest(cache)

    print(f"\nDone. Fetched {fetched} pages this run. {count} games with usable save data in {MANIFEST_PATH}.")

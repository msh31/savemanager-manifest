import time

from pcgw.api import api_get, now_iso


def fetch_all_steam_linked_pages():
    """Every Infobox_game row with a Steam AppID set. Cheap (2 fields), but there are ~48k of them."""
    pages = {}
    offset = 0
    while True:
        data = api_get({
            "action": "cargoquery",
            "tables": "Infobox_game",
            "fields": "Infobox_game._pageName=Page,Infobox_game.Steam_AppID=AppID,Infobox_game._pageID=PageID",
            "limit": "500",
            "offset": str(offset),
            "format": "json",
        })
        rows = data.get("cargoquery", [])
        if not rows:
            break
        for row in rows:
            title = row["title"]
            appid = (title.get("AppID") or "").strip()
            if not appid:
                continue
            pages[title["Page"]] = {
                "appid": appid.split(",")[0],
                "page_id": title.get("PageID"),
            }
        offset += 500
        time.sleep(1)
    return pages


def fetch_recent_changes(since):
    """Page titles edited since `since` (iso8601), newest edits first collapsed into a set."""
    changed = set()
    data = api_get({
        "action": "query",
        "list": "recentchanges",
        "rcprop": "title",
        "rcstart": now_iso(),
        "rcend": since,
        "rcdir": "older",
        "rclimit": "500",
        "rcnamespace": "0",
        "rctype": "edit|new",
        "format": "json",
    })
    for change in data.get("query", {}).get("recentchanges", []):
        changed.add(change["title"])
    return changed

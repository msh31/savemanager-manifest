import json
import time
import urllib.error
import urllib.parse
import urllib.request
from datetime import datetime, timezone

from pcgw.config import API, USER_AGENT


def now_iso():
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def api_get(params):
    url = API + "?" + urllib.parse.urlencode(params)
    req = urllib.request.Request(url, headers={"User-Agent": USER_AGENT})
    while True:
        try:
            with urllib.request.urlopen(req) as resp:
                return json.load(resp)
        except urllib.error.HTTPError as ex:
            if ex.code != 429:
                raise
            wait = int(ex.headers.get("Retry-After", "30"))
            print(f"! rate limited, waiting {wait}s...")
            time.sleep(wait)

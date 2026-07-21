import re

from pcgw.api import api_get
from pcgw.config import PLACEHOLDER_MAP, REGISTRY_ROOTS


def fetch_wikitext(page_title):
    data = api_get({
        "action": "parse",
        "page": page_title,
        "prop": "wikitext",
        "format": "json",
        "formatversion": "2",
    })
    return data.get("parse", {}).get("wikitext", "")


def find_balanced_templates(text, name):
    needle = "{{" + name.lower()
    lower = text.lower()
    results = []
    i = 0
    while True:
        start = lower.find(needle, i)
        if start == -1:
            break
        depth = 1
        j = start + 2
        while depth > 0 and j < len(text):
            if text[j:j + 2] == "{{":
                depth += 1
                j += 2
            elif text[j:j + 2] == "}}":
                depth -= 1
                j += 2
            else:
                j += 1
        inner = text[start + 2 + len(name):j - 2]
        if inner.startswith("|"):
            inner = inner[1:]
        results.append(inner)
        i = j
    return results


def split_top_level(s, sep="|"):
    parts = []
    depth = 0
    current = ""
    i = 0
    while i < len(s):
        if s[i:i + 2] == "{{":
            depth += 1
            current += "{{"
            i += 2
        elif s[i:i + 2] == "}}":
            depth -= 1
            current += "}}"
            i += 2
        elif s[i] == sep and depth == 0:
            parts.append(current)
            current = ""
            i += 1
        else:
            current += s[i]
            i += 1
    parts.append(current)
    return parts


def normalize_path(raw_path):
    clean = True

    def replace(match):
        nonlocal clean
        token = match.group(1).strip().lower()
        mapped = PLACEHOLDER_MAP.get(token)
        if mapped is None:
            clean = False
            return f"<UNKNOWN:{token}>"
        return f"<{mapped}>"

    normalized = re.sub(r"\{\{[pP]\|([^{}]+)\}\}", replace, raw_path)
    normalized = re.sub(r"<ref[^>]*>.*?</ref>", "", normalized, flags=re.DOTALL)
    normalized = re.sub(r"<!--.*?-->", "", normalized, flags=re.DOTALL)
    normalized = normalized.strip()

    if "{{" in normalized or "}}" in normalized:
        clean = False

    return normalized, clean


def extract_save_locations(wikitext):
    entries = []
    for call in find_balanced_templates(wikitext, "Game data/saves"):
        parts = split_top_level(call)
        if len(parts) < 2:
            continue
        os_name = parts[0].strip()
        raw_path = parts[1].strip()
        if not raw_path:
            continue  # exists but was never filled in
        path, clean = normalize_path(raw_path)
        if path.startswith(REGISTRY_ROOTS):
            continue  # not a path, fuck that shit
        entries.append({"os": os_name, "path": path, "clean": clean})
    return entries

# savemanager-manifest
This is a crawler for [PCGamingWiki](https://www.pcgamingwiki.com/) for "Save game data location" data
and produces the `data/pcgw_manifest.json` file that contains a lookup table of Steam AppID → save file
locations, keyed by OS, with paths normalized to a small placeholder vocabulary
(`<USER_PROFILE>`, `<APPDATA>`, `<STEAM_DIR>`, `<GAME_INSTALL_DIR>`, `<USER_ID>`, etc.)

The manifest is consumed by [SaveManager](https://github.com/msh31/SaveManager)'s `CPCGamingWikiDetector`, which resolves the placeholders for a user's actual machine
(including via Wine/Proton prefixes on Linux) and checks which save paths exist for
their installed Steam library.

## Data license
The save-location data itself originates from PCGamingWiki and is licensed under [CC BY-NC-SA 3.0](https://creativecommons.org/licenses/by-nc-sa/3.0/).

- `data/pcgw_manifest.json` and `data/wiki_cache.json` are derived from PCGamingWiki
  content and are distributed under those same **CC BY-NC-SA 3.0** terms, not the
  MIT license.

- Attribution: save location data courtesy of [PCGamingWiki](https://www.pcgamingwiki.com/), contributed by its editors.

- SaveManager itself is free and non-commercial, so this is compatible; anything
  built on this data commercially would need separate permission from PCGamingWiki's
  contributors/license.

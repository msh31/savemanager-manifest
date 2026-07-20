API = "https://www.pcgamingwiki.com/w/api.php"
USER_AGENT = "savemanager-manifest/0.1 (https://github.com/msh31/savemanager-manifest)"

CACHE_PATH = "data/wiki_cache.json"
META_PATH = "data/wiki_meta.json"
MANIFEST_PATH = "data/pcgw_manifest.json"

REQUEST_DELAY_SECONDS = 3
SAVE_EVERY = 25

# See https://www.pcgamingwiki.com/wiki/Template:Path for the source list.
PLACEHOLDER_MAP = {
    "userprofile": "USER_PROFILE",
    "userprofile\\documents": "USER_PROFILE_DOCUMENTS",
    "username": "USERNAME",
    "appdata": "APPDATA",
    "userprofile\\appdata\\locallow": "APPDATA_LOCALLOW",
    "localappdata": "LOCAL_APPDATA",
    "temp": "TEMP",
    "public": "PUBLIC",
    "allusersprofile": "PROGRAM_DATA",
    "programdata": "PROGRAM_DATA",
    "programfiles": "PROGRAM_FILES",
    "windir": "WINDIR",
    "syswow64": "SYSWOW64",
    "wow64": "WOW64",
    "hkey_current_user": "HKEY_CURRENT_USER",
    "hkcu": "HKEY_CURRENT_USER",
    "hkey_local_machine": "HKEY_LOCAL_MACHINE",
    "hklm": "HKEY_LOCAL_MACHINE",
    "machome": "OSX_HOME",
    "osxhome": "OSX_HOME",
    "linuxhome": "LINUX_HOME",
    "xdgdatahome": "XDG_DATA_HOME",
    "xdgconfighome": "XDG_CONFIG_HOME",
    "game": "GAME_INSTALL_DIR",
    "uid": "USER_ID",
    "steam": "STEAM_DIR",
    "steamlibrary": "STEAM_LIBRARY_DIR",
}

REGISTRY_ROOTS = ("<HKEY_CURRENT_USER>", "<HKEY_LOCAL_MACHINE>")

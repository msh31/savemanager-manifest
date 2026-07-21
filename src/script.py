#!/usr/bin/env python3
"""
Crawls PCGamingWiki for Steam-linked games' "Save game data location" data
and produces data/pcgw_manifest.json for SaveManager's CPCGamingWikiDetector.
"""
import argparse

from pcgw import crawler


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--only-appids",
        help="comma-separated Steam appids - only fetch pages already known for these, skip the full crawl/enumeration")
    args = parser.parse_args()

    crawler.run(only_appids=args.only_appids)


if __name__ == "__main__":
    main()

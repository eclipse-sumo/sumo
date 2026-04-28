#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2008-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    checkClangWarnings.py
# @author  Michael Behrisch
# @date    2026-04-28

"""
Fetches the daily clang build log from sumo.dlr.de, parses compiler
warnings and writes a Markdown report.  Intended to be run by the
daily GitHub Actions workflow "clang-warning-check".
"""

from __future__ import print_function

import argparse
import re
import sys
import urllib.request
from collections import defaultdict

LOG_URL = "https://sumo.dlr.de/daily/clangDmake.log"

# Matches lines like:
#   /abs/path/to/file.cpp:123:45: warning: some message [-Wsome-flag]
# The warning flag part is optional (some clang warnings omit it).
_WARNING_RE = re.compile(
    r'^(.+?):(\d+):(\d+):\s+warning:\s+(.+?)(?:\s+\[(-W[^\]]+)\])?\s*$'
)

# Heuristic: strip everything up to and including "/sumo/" to obtain a
# repository-relative path.  Falls back to the raw absolute path when the
# prefix cannot be found.
_SUMO_ROOT_MARKER = "/sumo/"


def _repo_relative(path):
    idx = path.find(_SUMO_ROOT_MARKER)
    if idx != -1:
        return path[idx + len(_SUMO_ROOT_MARKER):]
    return path


def fetch_log(url):
    req = urllib.request.Request(url, headers={"User-Agent": "SUMO-CI/1.0"})
    with urllib.request.urlopen(req, timeout=60) as response:
        return response.read().decode("utf-8", errors="replace")


def parse_warnings(log_content):
    warnings = []
    for line in log_content.splitlines():
        m = _WARNING_RE.match(line)
        if m:
            filepath, lineno, col, message, flag = m.groups()
            warnings.append({
                "file": filepath,
                "rel_file": _repo_relative(filepath),
                "line": int(lineno),
                "col": int(col),
                "message": message.strip(),
                "flag": flag or "",
            })
    return warnings


def generate_report(warnings, log_url):
    lines = [
        "# Clang Build Warnings",
        "",
        f"Source: [{log_url}]({log_url})",
        "",
    ]

    if not warnings:
        lines += ["No warnings found. :white_check_mark:", ""]
        return "\n".join(lines)

    by_file = defaultdict(list)
    by_flag = defaultdict(int)
    for w in warnings:
        by_file[w["rel_file"]].append(w)
        if w["flag"]:
            by_flag[w["flag"]] += 1

    lines += [
        f"Total warnings: **{len(warnings)}** across **{len(by_file)}** file(s)",
        "",
        "## Summary by warning type",
        "",
        "| Warning flag | Count |",
        "| --- | ---: |",
    ]
    for flag, count in sorted(by_flag.items(), key=lambda x: -x[1]):
        lines.append(f"| `{flag}` | {count} |")

    lines += ["", "## Warnings by file", ""]
    for rel_path in sorted(by_file.keys()):
        file_warnings = sorted(by_file[rel_path], key=lambda x: x["line"])
        lines.append(f"### `{rel_path}`")
        lines.append("")
        for w in file_warnings:
            flag_str = f" `{w['flag']}`" if w["flag"] else ""
            lines.append(f"- **Line {w['line']}**: {w['message']}{flag_str}")
        lines.append("")

    return "\n".join(lines)


def main():
    parser = argparse.ArgumentParser(
        description="Fetch the daily clang build log and report warnings."
    )
    parser.add_argument(
        "--url", default=LOG_URL, help="URL of the clang build log"
    )
    parser.add_argument(
        "--output",
        default=None,
        help="Write Markdown report to this file (default: stdout)",
    )
    parser.add_argument(
        "--fail-on-warnings",
        action="store_true",
        help="Exit with code 1 when warnings are found",
    )
    args = parser.parse_args()

    try:
        log_content = fetch_log(args.url)
    except Exception as exc:
        print(f"ERROR: could not fetch {args.url}: {exc}", file=sys.stderr)
        sys.exit(2)

    warnings = parse_warnings(log_content)
    report = generate_report(warnings, args.url)

    if args.output:
        with open(args.output, "w", encoding="utf-8") as fh:
            fh.write(report)
    else:
        print(report)

    if args.fail_on_warnings and warnings:
        sys.exit(1)


if __name__ == "__main__":
    main()

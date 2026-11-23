#!/usr/bin/env python
# -*- coding: utf-8 -*-
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

# @file    updateReleaseInfo.py
# @author  Michael Behrisch
# @date    2022-07-11

from __future__ import absolute_import
from __future__ import print_function
import argparse
import datetime
import fileinput
import os
import re
from pathlib import Path

import version

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.dirname(os.path.dirname(os.path.dirname(__file__))))

parser = argparse.ArgumentParser(description="Update the release metadata.")
parser.add_argument("version", help="release version, e.g. 1.28.0")
parser.add_argument("date", nargs="?", help="release date in YYYY-MM-DD format (default: tomorrow)")
parser.add_argument("--release-readme", type=Path, help="write a release README to this path")
parser.add_argument("--summary", help="short release description for the README")
args = parser.parse_args()
if args.release_readme is not None:
    if not args.summary or not args.summary.strip():
        parser.error("--release-readme requires a non-empty --summary")
    if not re.fullmatch(r"[0-9]+(?:\.[0-9]+)+", args.version):
        parser.error("--release-readme requires a dotted numeric version")
    if not args.release_readme.parent.is_dir() or args.release_readme.is_dir():
        parser.error("--release-readme must name a file in an existing directory")
elif args.summary is not None:
    parser.error("--summary requires --release-readme")
try:
    date = (datetime.datetime.strptime(args.date, "%Y-%m-%d") if args.date else
            datetime.datetime.now() + datetime.timedelta(days=1))
except ValueError as error:
    parser.error(str(error))

last_release = version.get_version()
if "+" in last_release:
    last_release = last_release[:last_release.index("+")]
dot_release = last_release.replace("_", ".")[1:]

version = args.version
next_release = version.replace(".", "_")
print("updating", dot_release, "to", version, "release date", date)
author_names = []
with open(os.path.join(SUMO_HOME, "AUTHORS")) as authors:
    header_seen = False
    for line in authors:
        line = line.strip()
        if line == "" and not header_seen:
            header_seen = True
        if not header_seen or " " not in line or line[0] == "@":
            continue
        if "<" in line:
            name = line[:line.index("<") - 1]
        elif "*" in line:
            name = line[:line.index("*") - 1]
        else:
            name = line
        if " " in name:
            author_names.append('  - name: "%s"\n' % name)

with fileinput.FileInput(os.path.join(SUMO_HOME, "CITATION.cff"), inplace=True) as cff:
    have_authors = False
    for line in cff:
        if line[:14] == "date-released:":
            line = date.strftime('date-released: "%Y-%m-%d"\n')
        if line[:8] == "version:":
            line = 'version: %s\n' % version
        if 'sumo/releases/tag/v' in line:
            line = line[:line.find('sumo/releases/tag/v') + 19] + next_release + '"\n'
        if 'sumo.dlr.de/releases/' in line:
            line = line[:line.find('sumo.dlr.de/releases/') + 21] + version + '/"\n'
        if line[:8] == "message:":
            last = None
            for a in sorted(author_names):
                if a != last:
                    print(a, end='')
                    last = a
            have_authors = False
        if line == "authors:\n" and not have_authors:
            have_authors = True
            print(line, end='')
        elif have_authors:
            author_names.append(line)
        else:
            print(line, end='')

with fileinput.FileInput(os.path.join(SUMO_HOME, "src", "config.h.cmake"), inplace=True) as config:
    for line in config:
        if line == "#define HAVE_VERSION_H\n":
            print("//" + line, end='')
        else:
            print(line.replace(dot_release, version), end='')

with fileinput.FileInput(os.path.join(SUMO_HOME, "CMakeLists.txt"), inplace=True) as cmake:
    for line in cmake:
        if line == 'set(PACKAGE_VERSION "git")\n':
            print(line.replace("git", version), end='')
        else:
            print(line, end='')

with fileinput.FileInput(os.path.join(SUMO_HOME, "docs", "web", "mkdocs.yml"), inplace=True) as mkdocs:
    for line in mkdocs:
        if "ReleaseDate:" in line:
            print('    ReleaseDate:', date.strftime("%d.%m.%Y"))
        else:
            print(line.replace(dot_release, version), end='')

with fileinput.FileInput(os.path.join(SUMO_HOME, "docs", "web", "docs", "ChangeLog.md"), inplace=True) as mkdocs:
    for line in mkdocs:
        if "## Git Main" in line:
            print(line.replace("Git Main", date.strftime("Version " + version + " (%d.%m.%Y)")), end='')
        else:
            print(line, end='')

with fileinput.FileInput(os.path.join(SUMO_HOME, "build_config", "package", "org.eclipse.sumo.metainfo.xml"),
                         inplace=True) as metainfo:
    have_next = False
    for line in metainfo:
        if next_release in line:
            have_next = True
        if not have_next and last_release in line:
            print('        <release version="v%s" date="%s"/>' % (next_release, date.strftime("%Y-%m-%d")))
        print(line, end='')


if args.release_readme is not None:
    changelog = "https://sumo.dlr.de/docs/ChangeLog/%s.html" % version
    args.release_readme.write_text(
        "# SUMO %s\n\nReleased on %s.\n\n%s\n\n[Changelog for SUMO %s](%s)\n"
        % (version, date.strftime("%Y-%m-%d"), args.summary.strip(), version, changelog),
        encoding="utf-8",
    )

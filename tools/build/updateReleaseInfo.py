#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
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
import os
import sys
import fileinput
import datetime

import version

SUMO_HOME = os.environ.get("SUMO_HOME", os.path.dirname(os.path.dirname(os.path.dirname(__file__))))

last_release = version.get_version()
if "+" in last_release:
    last_release = last_release[:last_release.index("+")]
dot_release = last_release.replace("_", ".")[1:]

version = sys.argv[1]
next_release = version.replace(".", "_")
if len(sys.argv) > 2:
    date = datetime.datetime.strptime(sys.argv[2], "%Y-%m-%d")
else:
    date = datetime.datetime.now() + datetime.timedelta(days=1)
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

with fileinput.FileInput(os.path.join(SUMO_HOME, "build", "package", "sumo.metainfo.xml"), inplace=True) as metainfo:
    have_next = False
    for line in metainfo:
        if next_release in line:
            have_next = True
        if not have_next and last_release in line:
            print('        <release version="v%s" date="%s"/>' % (next_release, date.strftime("%Y-%m-%d")))
        print(line, end='')

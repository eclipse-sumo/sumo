#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2025 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    history.py
# @author  Michael Behrisch
# @date    2014-06-21

"""
This script builds all sumo versions in a certain revision range
and tries to eliminate duplicates afterwards.
"""
from __future__ import absolute_import

import subprocess
import shutil
import os
import sys
import traceback

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import sumolib  # noqa


arg_parser = sumolib.options.ArgumentParser()
arg_parser.add_argument("-b", "--begin", default="v1_3_0", help="first revision to build")
arg_parser.add_argument("-e", "--end", default="HEAD", help="last revision to build")
arg_parser.add_argument("-d", "--destination", default="..", help="where to put build results")
arg_parser.add_argument("-t", "--tags-only", action="store_true", default=False,
                        help="only build tagged revisions")
options = arg_parser.parse_args()

LOCK = "../history.lock"
if os.path.exists(LOCK):
    sys.exit("History building is still locked!")
open(LOCK, 'w').close()
try:
    subprocess.call(["git", "checkout", "-q", "main"])
    subprocess.call(["git", "pull"])
    commits = {}
    if options.tags_only:
        active = False
        for tag in subprocess.check_output(["git", "tag", "--sort=taggerdate"], universal_newlines=True).splitlines():
            if tag == options.begin:
                active = True
            if active:
                commits[tag] = tag
            if tag == options.end:
                active = False
    else:
        for line in subprocess.check_output(["git", "log", "%s..%s" % (options.begin, options.end)]).splitlines():
            if line.startswith("commit "):
                h = line.split()[1]
                commits[h] = sumolib.version.gitDescribe(h)
    haveBuild = False
    for h, desc in sorted(commits.items(), key=lambda x: x[1]):
        dest = os.path.join(options.destination, 'bin%s' % desc)
        if not os.path.exists(dest):
            ret = subprocess.call(["git", "checkout", "-q", h])
            if ret != 0:
                continue
            os.chdir("build/cmake-build")
            subprocess.call('make clean; make -j32', shell=True)
            os.chdir("../..")
            haveBuild = True
            shutil.copytree('bin', dest, ignore=shutil.ignore_patterns('*.fmu', '*.bat', '*.jar'))
            subprocess.call('strip -R .note.gnu.build-id %s/*' % dest, shell=True)
            subprocess.call("sed -i 's/%s/%s/' %s" % (desc, len(desc) * "0", dest), shell=True)
    if haveBuild:
        for line in subprocess.check_output('fdupes -1 -q %s/binv*' % options.destination, shell=True).splitlines():
            dups = line.split()
            for d in dups[1:]:
                subprocess.call('ln -sf %s %s' % (dups[0], d), shell=True)
    subprocess.call(["git", "checkout", "-q", "main"])
except Exception:
    traceback.print_exc()
os.remove(LOCK)

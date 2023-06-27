#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    obsoleteTranslations.py
# @author  Mirko Barthauer
# @date    2023-07-04

"""
Collect information about lost translations due to changes in the original gettext msgid.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import io
import polib
import subprocess
import i18n
from glob import glob
from argparse import ArgumentParser

SUMO_HOME = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SUMO_LIBRARIES = os.environ.get("SUMO_LIBRARIES", os.path.join(os.path.dirname(SUMO_HOME), "SUMOLibraries"))


def getOptions(args=None):
    ap = ArgumentParser()
    ap.add_argument("-l", "--lang", nargs='*', help="languages to process (using the gettext short codes like en, fr, de)")
    ap.add_argument("--sumo-home", default=SUMO_HOME, help="SUMO root directory to use")
    ap.add_argument("-o", "--output", type=str, help="path to output file (protocol of obsolete translations)")
    ap.add_argument("--clear", default=False, action="store_true", help="remove obsolete entries from po translation files")
    return ap.parse_args(args)


def main(args=None):
    path = ""
    if os.name == "nt":
        paths = glob(os.path.join(SUMO_LIBRARIES, "gettext-*", "tools", "gettext", "bin"))
        if paths:
            path = paths[0] + os.path.sep
    options = getOptions(args)
    if options.lang is None:
        options.lang = [os.path.basename(p)[:-8] for p in glob(options.sumo_home + "/data/po/*_sumo.po")]
    # run i18n.py to update the translation files
    args = ['--sumo-home', options.sumo_home, '--lang']
    args.extend(options.lang)
    print("Run i18n.py ...")
    i18n.main(args=args)
    pot_file = options.sumo_home + "/data/po/sumo.pot"
    gui_pot_file = options.sumo_home + "/data/po/gui.pot"
    py_pot_file = options.sumo_home + "/data/po/py.pot"
    potFiles = [pot_file, gui_pot_file, py_pot_file]
    if options.output is not None:
        if os.path.exists(options.output):
            os.remove(options.output) # clear output file
    for potFile in potFiles:
        print("Check pot file '%s'..." % potFile)
        checkPotFile(potFile, options)


def checkPotFile(potFile, options):
    # loop through translated po files and collect the obsolete translations
    result = {}
    for langCode in options.lang:
        poFilePath = os.path.join(os.path.dirname(potFile), "%s_" % langCode + os.path.basename(potFile)[:-1])
        if not os.path.exists(poFilePath):
            continue
        po = polib.pofile(poFilePath)
        obsoleteEntries = po.obsolete_entries()
        for entry in obsoleteEntries:
            if entry.msgid not in result:
                result[entry.msgid] = []
            result[entry.msgid].append(langCode)
        # optionally overwrite obsolete entries completely
        if options.clear:
            print("Removing obsolete entries from %s..." % poFilePath)
            for entry in obsoleteEntries:
                po.remove(entry)
            po.save(poFilePath)
    
    if options.output is not None: # write protocol
        with io.open(options.output, "a", encoding="utf-8") as f:
            for msgid, langCodes in result.items():
                f.write("msgid \"%s\" has obsolete translations: %s\n" % (msgid, ', '.join(langCodes)))


if __name__ == "__main__":
    main()
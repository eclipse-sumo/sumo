#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
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

# @file    i18n.py
# @author  Michael Behrisch
# @date    2022-10-08

"""
Prepare gettext pot and po files for all languages.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import subprocess
import difflib
from glob import glob
from argparse import ArgumentParser


SUMO_HOME = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SUMO_LIBRARIES = os.environ.get("SUMO_LIBRARIES", os.path.join(os.path.dirname(SUMO_HOME), "SUMOLibraries"))


def get_args(args=None):
    arg_parser = ArgumentParser()
    arg_parser.add_argument("-l", "--lang", nargs='*', help="languages to process")
    arg_parser.add_argument("-m", "--mo-only", action="store_true", default=False,
                            help="only generate mo files, do not update po and pot")
    arg_parser.add_argument("-f", "--fuzzy", action="store_true", default=False,
                            help="use fuzzy matching to prefill new message ids")
    arg_parser.add_argument("--sumo-home", default=SUMO_HOME, help="SUMO root directory to use")
    return arg_parser.parse_args(args)


def generate_po(sumo_home, path, languages, pot_file, gui_pot_file, fuzzy):
    pots = {pot_file: open(pot_file + ".txt", "w"), gui_pot_file: open(gui_pot_file + ".txt", "w")}
    for f in sorted(glob(sumo_home + "/src/*.cpp") +
                    glob(sumo_home + "/src/*/*.cpp") +
                    glob(sumo_home + "/src/*/*/*.cpp") +
                    glob(sumo_home + "/src/*.h") +
                    glob(sumo_home + "/src/*/*.h") +
                    glob(sumo_home + "/src/*/*/*.h")):
        if "gui" in f[len(sumo_home):] or "netedit" in f[len(sumo_home):]:
            print(f, file=pots[gui_pot_file])
        else:
            print(f, file=pots[pot_file])
    for pot, sources in pots.items():
        sources.close()
        subprocess.check_call([path + "xgettext", "--files-from=" + sources.name, "--from-code=UTF-8",
                              "--keyword=TL", "--keyword=TLF", "--output=" + pot + ".new", "--package-name=sumo",
                               "--msgid-bugs-address=sumo-dev@eclipse.org"])
        os.remove(sources.name)
        has_diff = True
        if os.path.exists(pot):
            with open(pot) as old, open(pot + ".new") as new:
                a = [s for s in old.readlines() if not s.startswith(("#", '"POT-Creation-Date:'))]
                b = [s for s in new.readlines() if not s.startswith(("#", '"POT-Creation-Date:'))]
                has_diff = list(difflib.unified_diff(a, b))
            if has_diff:
                os.remove(pot)
        if has_diff:
            os.rename(pot + ".new", pot)
        else:
            os.remove(pot + ".new")
        for lang in languages:
            po_file = "%s/data/po/%s_%s" % (sumo_home, lang, os.path.basename(pot)[:-1])
            if os.path.exists(po_file):
                subprocess.check_call([path + "msgmerge", po_file, pot,
                                       "--output-file=" + po_file] + ([] if fuzzy else ["--no-fuzzy-matching"]))
            else:
                subprocess.check_call([path + "msginit", "--input=" + pot, "--output=" + po_file,
                                       "--no-translator", "--locale=" + lang])


def main(args=None):
    path = ""
    if os.name == "nt":
        paths = glob(os.path.join(SUMO_LIBRARIES, "gettext-*", "tools", "gettext", "bin"))
        if paths:
            path = paths[0] + os.path.sep
    options = get_args(args)
    if options.lang is None:
        options.lang = [os.path.basename(p)[:-8] for p in glob(options.sumo_home + "/data/po/*_sumo.po")]
    pot_file = options.sumo_home + "/data/po/sumo.pot"
    gui_pot_file = options.sumo_home + "/data/po/gui.pot"
    if not options.mo_only:
        generate_po(options.sumo_home, path, options.lang, pot_file, gui_pot_file, options.fuzzy)
    for lang in options.lang:
        po_files = ["%s/data/po/%s_%s" % (options.sumo_home, lang, os.path.basename(pot)[:-1])
                    for pot in (pot_file, gui_pot_file)]
        merged_po_file = "%s/data/po/%s.po" % (options.sumo_home, lang)
        subprocess.check_call([path + "msgcat"] + po_files + ["--output-file=" + merged_po_file])
        d = "%s/data/locale/%s/LC_MESSAGES" % (options.sumo_home, lang)
        try:
            os.makedirs(d)
        except OSError:
            pass
        subprocess.check_call([path + "msgfmt", merged_po_file, "--output-file=%s/sumo.mo" % d])
        os.remove(merged_po_file)


if __name__ == "__main__":
    main()

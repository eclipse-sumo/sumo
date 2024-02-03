#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2024 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    updateMessageIDs.py
# @author  Mirko Barthauer
# @date    2023-06-26

"""
Change gettext message IDs and propagate the changes to the source code and translated messages.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import io
import re
import polib
import subprocess
import i18n
from glob import glob
from argparse import ArgumentParser

SUMO_HOME = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SUMO_LIBRARIES = os.environ.get("SUMO_LIBRARIES", os.path.join(os.path.dirname(SUMO_HOME), "SUMOLibraries"))


def getOptions(args=None):
    ap = ArgumentParser()
    ap.add_argument("-l", "--lang", nargs='*',
                    help="languages to process (using the gettext short codes like en, fr, de)")
    ap.add_argument("--replace", nargs='*',
                    help="search/replace commands to apply automatically, giving the search string on the uneven \
                    position and the replace string on the even position; cannot be used together with \
                    --numbered-placeholders")
    ap.add_argument("--numbered-placeholders", dest="numberedPlaceholders", default=False, action="store_true",
                    help="add a running number to every placeholder in a msgid, starting with 0; cannot be \
                    used together with --replace")
    ap.add_argument("--placeholder", default='%', help="string used for unnumbered placeholders")
    ap.add_argument("--search-prefix", dest="searchPrefix", default=' ', type=str,
                    help="characters which can precede the actual search string in --replace (ignored with --strict)")
    ap.add_argument("--search-suffix", dest="searchSuffix", default=' .?!', type=str,
                    help="characters which can succeed the actual search string in --replace (ignored with --strict)")
    ap.add_argument("--strict", default=False, action="store_true",
                    help="compare the entire msgid to the search string")
    ap.add_argument("--start", default=False, action="store_true",
                    help="create the English to English po file to edit manually")
    ap.add_argument("--apply", default=False, action="store_true",
                    help="apply changes to source files and to msgid values in po files of languages to process")
    ap.add_argument("--remove-obsolete", dest="removeObsolete", default=False, action="store_true",
                    help="remove obsolete msgid entries which have been superseded by replace strings")
    ap.add_argument("--mark-fuzzy", dest="markFuzzy", default=False, action="store_true",
                    help="mark kept translations of changed msgid as fuzzy")
    ap.add_argument("--process-languages", dest="processLanguages", default=False, action="store_true",
                    help="process msgstr values of language po files with the same rules as the original message")
    ap.add_argument("--sumo-home", default=SUMO_HOME, help="SUMO root directory to use")
    return ap.parse_args(args)


def main(args=None):
    path = ""
    if os.name == "nt":
        paths = glob(os.path.join(SUMO_LIBRARIES, "gettext-*", "tools", "gettext", "bin"))
        if paths:
            path = paths[0] + os.path.sep
    options = getOptions(args)
    if not options.start and not options.apply:
        sys.exit("At least one of the --start or --apply options has to be given to do something.")
    if options.numberedPlaceholders and options.replace is not None:
        print("Cannot apply both placeholder numbers and replace commands together. \
            Replace commands will be neglected.")
        options.replace.clear()
    replaceRules = []
    if options.replace is not None:
        if len(options.replace) % 2 != 0:
            print("The replace string for the search string '%s' is missing. The named search string will be neglected."
                  % options.replace[-1])
            options.replace = options.replace[:-1]
        # update newline chars in input
        options.replace = [replaceInput.replace("\\n", "\n") for replaceInput in options.replace]
        for i in range(0, len(options.replace), 2):
            if options.strict:
                replaceRules.append((options.replace[i], options.replace[i+1],
                                     re.compile(re.escape(options.replace[i])), True, True))
            else:
                prefixes = list(options.searchPrefix)
                if len(prefixes) == 0:
                    prefixes.append('')
                suffixes = list(options.searchSuffix)
                if len(suffixes) == 0:
                    suffixes.append('')
                for prefix in prefixes:
                    for suffix in suffixes:
                        replaceRules.append((prefix + options.replace[i] + suffix,
                                             prefix + options.replace[i+1] + suffix, None, False, False))
                    replaceRules.append((options.replace[i] + suffix, options.replace[i+1] + suffix, None, True, False))
                    replaceRules.append((prefix + options.replace[i], prefix + options.replace[i+1], None, False, True))
    if options.lang is None:
        options.lang = [os.path.basename(p)[:-8] for p in glob(options.sumo_home + "/data/po/*_sumo.po")]
    if options.start:
        args = ['--sumo-home', options.sumo_home, '--lang']
        args.extend(options.lang)
        print("Run i18n.py...")
        i18n.main(args=args)
    pot_file = options.sumo_home + "/data/po/sumo.pot"
    gui_pot_file = options.sumo_home + "/data/po/gui.pot"
    py_pot_file = options.sumo_home + "/data/po/py.pot"
    potFiles = [pot_file, gui_pot_file, py_pot_file]
    for potFile in potFiles:
        print("Read pot file '%s'..." % potFile)
        updatePotFile(path, potFile, replaceRules, options)


def updatePotFile(gettextPath, potFile, replaceRules, options):
    uniLangPoFile = potFile[:-4] + "_en.po"
    if options.start or options.replace is not None:
        arguments = [gettextPath + "msgen", potFile, "-o", uniLangPoFile]
        subprocess.check_call(arguments)
    if options.replace or options.numberedPlaceholders:
        processRules(uniLangPoFile, replaceRules, options, markObsolete=True)
    if options.apply:
        fileReplaceCommands = {}
        # reload to discover which entries have changed and store for later application in source code
        replaceIDs = []
        po = polib.pofile(uniLangPoFile)
        for entry in po:
            if entry.msgid != entry.msgstr:  # changes in msgid
                for occurrence, lineNr in entry.occurrences:
                    if occurrence not in fileReplaceCommands:
                        fileReplaceCommands[occurrence] = []
                    # newline conversion between polib and source code
                    fileReplaceCommands[occurrence].append((entry.msgid.replace(
                        "\n", "\\n"), entry.msgstr.replace("\n", "\\n"), int(lineNr)))
                    replaceIDs.append(entry)

        # apply the changes to the source code
        for sourceFile, replaceCommands in fileReplaceCommands.items():
            lines = []
            with io.open(sourceFile, "r", encoding="utf-8") as f:
                lines.extend([line for line in f])
            lineCount = len(lines)
            updated = False
            for search, replace, lineNr in replaceCommands:
                if lineNr <= lineCount and replace not in lines[lineNr-1]:
                    lines[lineNr-1] = lines[lineNr-1].replace(search, replace)
                    updated = True
            if updated:
                print("\tUpdate %s" % sourceFile)
                with io.open(sourceFile, "w", encoding="utf-8", newline="\n") as f:
                    f.writelines(lines)

        # change the msgid in other language files accordingly
        for langCode in options.lang:
            translatedPoFile = os.path.join(os.path.dirname(potFile), "%s_" % langCode +
                                            os.path.basename(potFile)[:-4] + ".po")
            if not os.path.exists(translatedPoFile):
                print("Missing po translation file %s to update" % translatedPoFile)
                continue
            patchPoFile(translatedPoFile, replaceIDs, fuzzy=options.markFuzzy)

            # optionally process the translated messages as well
            if options.processLanguages:
                processRules(translatedPoFile, replaceRules, options, translated=True,
                             filterIDs=[entry.msgstr for entry in replaceIDs])
    if options.apply:
        os.remove(uniLangPoFile)


def transferOccurrences(fromEntry, toEntry):
    toEntry.occurrences.extend(fromEntry.occurrences)
    fromEntry.occurrences.clear()
    fromEntry.obsolete = True


def processRules(poFilePath, replaceRules, options, markObsolete=False, filterIDs=None):
    # change the msgstr values according to the rules given in the options
    poFile = polib.pofile(poFilePath)
    checkFilter = filterIDs is not None
    toRemove = []
    for entry in poFile:
        if checkFilter and entry.msgid not in filterIDs:  # only process the wanted entries
            continue
        replaced = None
        if options.numberedPlaceholders:
            i = 0
            placeholderIndex = entry.msgstr.find(options.placeholder)
            if placeholderIndex > -1:
                replaced = entry.msgstr
            while placeholderIndex > -1:
                replaced = replaced[:placeholderIndex + len(options.placeholder)] + str(i) +\
                    replaced[placeholderIndex + len(options.placeholder):]
                placeholderIndex = replaced.find(options.placeholder, placeholderIndex +
                                                 len(options.placeholder) + len(str(i)))
                i += 1
        for replaceRule in replaceRules:
            if options.strict and replaceRule[0] == entry.msgstr:
                replaced = replaceRule[1]
            elif replaceRule[3] and not replaceRule[4] and entry.msgstr.startswith(replaceRule[0]):  # starts with ...
                replaced = replaceRule[1] + entry.msgstr[len(replaceRule[0]):]
            elif not replaceRule[3] and replaceRule[4] and entry.msgstr.endswith(replaceRule[0]):  # ends with ...
                replaced = entry.msgstr[:-len(replaceRule[0])] + replaceRule[1]
            elif replaceRule[0] in entry.msgstr:
                replaced = entry.msgstr.replace(replaceRule[0], replaceRule[1])
        if replaced is not None:
            # check if it already exists >> then transfer occurrences and remember it
            print("Replace '%s' by '%s'" % (entry.msgstr, replaced))
            match = poFile.find(replaced, msgctxt=entry.msgctxt)
            if markObsolete and match is not None:
                print("Transfer duplicate entries for msgid '%s' (was '%s')." % (replaced, entry.msgstr))
                transferOccurrences(entry, match)
                if options.removeObsolete:
                    print("Remove obsolete entry '%s' completely." % entry.msgstr)
                    toRemove.append(entry)
            else:
                entry.msgstr = replaced
    for entry in toRemove:
        poFile.remove(entry)
    poFile.save(fpath=poFilePath)


def patchPoFile(filePath, replaceIDs, fuzzy=False):
    po = polib.pofile(filePath)
    for entry in replaceIDs:
        search = entry.msgid
        replace = entry.msgstr
        searchEntry = po.find(search)  # add context filter here
        replaceEntry = po.find(replace)
        if searchEntry is not None:
            if replaceEntry is not None:
                if len(replaceEntry.msgstr) == 0 and len(searchEntry.msgstr) > 0:
                    replaceEntry.msgstr = searchEntry.msgstr
                transferOccurrences(searchEntry, replaceEntry)
            else:
                searchEntry.msgid = replace
                if fuzzy:
                    searchEntry.fuzzy = True
    po.save(fpath=filePath)


if __name__ == "__main__":
    main()

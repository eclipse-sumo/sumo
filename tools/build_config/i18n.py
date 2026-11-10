#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2011-2026 German Aerospace Center (DLR) and others.
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
import io
import subprocess
import difflib
from glob import glob
from argparse import ArgumentParser
from html.parser import HTMLParser


SUMO_HOME = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
SUMO_LIBRARIES = os.environ.get("SUMO_LIBRARIES", os.path.join(os.path.dirname(SUMO_HOME), "SUMOLibraries"))
WEB_WIZARD_HTML = ["/tools/webWizard/index.html"]
WEB_WIZARD_JS = ["/tools/webWizard/script.js"]
WEB_WIZARD_PY = ["/tools/osmWebWizard.py"]


class HTMLStringExtractor(HTMLParser):
    """Collects the translatable strings of an html file.
    The direct text content of elements with the attribute "data-i18n" is translated
    as well as all attributes listed (space separated) in the attribute "data-i18n-attrs".
    """
    VOID_ELEMENTS = {"area", "base", "br", "col", "embed", "hr", "img", "input",
                     "link", "meta", "source", "track", "wbr"}

    def __init__(self):
        HTMLParser.__init__(self, convert_charrefs=True)
        self.stack = []
        self.messages = []

    def _add(self, text, line):
        msgid = " ".join(text.split())
        if msgid:
            self.messages.append((msgid, line))

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        for attr in (attrs.get("data-i18n-attrs") or "").split():
            if attrs.get(attr):
                self._add(attrs[attr], self.getpos()[0])
        if tag not in self.VOID_ELEMENTS:
            self.stack.append((tag, "data-i18n" in attrs))

    def handle_endtag(self, tag):
        if tag in self.VOID_ELEMENTS:
            return
        while self.stack and self.stack.pop()[0] != tag:
            pass

    def handle_data(self, data):
        if self.stack and self.stack[-1][1]:
            stripped = data.lstrip()
            self._add(data, self.getpos()[0] + data[:len(data) - len(stripped)].count("\n"))


def write_html_pot(html_files, pot):
    """Writes the translatable strings of the given html files as a pot file
    which can be used as input for xgettext --join-existing."""
    messages = {}
    for html_file in html_files:
        parser = HTMLStringExtractor()
        with io.open(html_file, encoding="utf-8-sig") as f:
            parser.feed(f.read())
        parser.close()
        for msgid, line in parser.messages:
            messages.setdefault(msgid, []).append("%s:%s" % (html_file, line))
    with io.open(pot, "w", encoding="utf-8") as out:
        out.write('msgid ""\nmsgstr ""\n"Content-Type: text/plain; charset=UTF-8\\n"\n')
        for msgid, refs in messages.items():
            out.write('\n#: %s\nmsgid "%s"\nmsgstr ""\n' % (" ".join(refs),
                                                            msgid.replace("\\", "\\\\").replace('"', '\\"')))


def get_args(args=None):
    arg_parser = ArgumentParser()
    arg_parser.add_argument("-l", "--lang", nargs='*', help="languages to process")
    arg_parser.add_argument("-m", "--mo-only", action="store_true", default=False,
                            help="only generate mo files, do not update po and pot")
    arg_parser.add_argument("-f", "--fuzzy", action="store_true", default=False,
                            help="use fuzzy matching to prefill new message ids")
    arg_parser.add_argument("--sumo-home", default=SUMO_HOME, help="SUMO root directory to use")
    arg_parser.add_argument("-o", "--out-dir", help="output directory to use")
    return arg_parser.parse_args(args)


def generate_po(sumo_home, path, languages, pot_file, gui_pot_file, py_pot_file, fuzzy, web_pot_file=None):
    pots = {pot_file: open(pot_file + ".txt", "w"),
            gui_pot_file: open(gui_pot_file + ".txt", "w"),
            py_pot_file: open(py_pot_file + ".txt", "w")}
    if web_pot_file is not None:
        pots[web_pot_file] = open(web_pot_file + ".txt", "w")
        for f in WEB_WIZARD_JS:
            print(sumo_home + f, file=pots[web_pot_file])
    for f in sorted(glob(sumo_home + "/src/*.cpp") +
                    glob(sumo_home + "/src/*/*.cpp") +
                    glob(sumo_home + "/src/*/*/*.cpp") +
                    glob(sumo_home + "/src/*/*/*/*.cpp") +
                    glob(sumo_home + "/src/*.h") +
                    glob(sumo_home + "/src/*/*.h") +
                    glob(sumo_home + "/src/*/*/*.h") +
                    glob(sumo_home + "/src/*/*/*/*.h") +
                    glob(sumo_home + "/tools/game/*.py")):
        if f[-3:] == ".py":
            print(f, file=pots[py_pot_file])
        elif "gui" in f[len(sumo_home):] or "netedit" in f[len(sumo_home):]:
            print(f, file=pots[gui_pot_file])
        else:
            print(f, file=pots[pot_file])
    for pot, sources in pots.items():
        sources.close()
        arguments = [path + "xgettext", "--files-from=" + sources.name, "--from-code=UTF-8",
                     "--keyword=TL", "--keyword=TLC:1c,2", "--keyword=TLF", "--output=" + pot + ".new",
                     "--package-name=sumo", "--msgid-bugs-address=sumo-dev@eclipse.org"]
        if pot == py_pot_file:
            arguments.append("--language=Python")
        if pot == web_pot_file:
            write_html_pot([sumo_home + f for f in WEB_WIZARD_HTML], pot + ".new")
            arguments += ["--language=JavaScript", "--join-existing", "--keyword=N_"]
        subprocess.check_call(arguments)
        if pot == web_pot_file:
            # the progress messages of the server are shown in the web interface as well
            subprocess.check_call([a for a in arguments if not a.startswith(("--files-from", "--language"))] +
                                  ["--language=Python"] + [sumo_home + f for f in WEB_WIZARD_PY])
        os.remove(sources.name)
        with io.open(pot + ".new", encoding="utf-8") as new, io.open(pot + ".fixed", "w", encoding="utf-8") as fixed:
            for s in new.readlines():
                if s.startswith("#") and sumo_home in s:
                    fixed.write(s.replace(sumo_home, "").replace("\\", "/").replace("#: /", "#: "))
                elif s.strip() == r'"Content-Type: text/plain; charset=CHARSET\n"':
                    fixed.write(s.replace("CHARSET", "UTF-8"))
                else:
                    fixed.write(s)
        os.remove(new.name)
        has_diff = True
        if os.path.exists(pot):
            with io.open(pot, encoding="utf-8") as old, io.open(pot + ".fixed", encoding="utf-8") as fixed:
                a = [s for s in old.readlines() if not s.startswith(("#", '"POT-Creation-Date:'))]
                b = [s for s in fixed.readlines() if not s.startswith(("#", '"POT-Creation-Date:'))]
                has_diff = list(difflib.unified_diff(a, b))
            if has_diff:
                os.remove(pot)
        if has_diff:
            os.rename(fixed.name, pot)
        else:
            os.remove(fixed.name)
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
    if options.out_dir is None:
        options.out_dir = options.sumo_home + "/data"
    pot_file = options.sumo_home + "/data/po/sumo.pot"
    gui_pot_file = options.sumo_home + "/data/po/gui.pot"
    py_pot_file = options.sumo_home + "/data/po/py.pot"
    web_pot_file = options.sumo_home + "/data/po/osmWebWizard.pot"
    if not options.mo_only:
        generate_po(options.sumo_home, path, options.lang, pot_file, gui_pot_file, py_pot_file, options.fuzzy,
                    web_pot_file)
    for lang in options.lang:
        po_files = ["%s/data/po/%s_%s" % (options.sumo_home, lang, os.path.basename(pot)[:-1])
                    for pot in (pot_file, gui_pot_file, py_pot_file)]
        if not os.path.exists("%s/po" % options.out_dir):
            os.makedirs("%s/po" % options.out_dir)
        merged_po_file = "%s/po/%s.po" % (options.out_dir, lang)
        subprocess.check_call([path + "msgcat"] + po_files + ["--output-file=" + merged_po_file])
        d = "%s/locale/%s/LC_MESSAGES" % (options.out_dir, lang)
        try:
            os.makedirs(d)
        except OSError:
            pass
        subprocess.check_call([path + "msgfmt", merged_po_file, "--output-file=%s/sumo.mo" % d])
        os.remove(merged_po_file)
        # the web wizard has its own domain because its strings are delivered to the browser separately
        web_po_file = "%s/data/po/%s_%s" % (options.sumo_home, lang, os.path.basename(web_pot_file)[:-1])
        if os.path.exists(web_po_file):
            subprocess.check_call([path + "msgfmt", web_po_file, "--output-file=%s/osmWebWizard.mo" % d])


if __name__ == "__main__":
    main()

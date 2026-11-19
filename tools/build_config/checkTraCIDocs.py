#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2020-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    checkTraCIDocs.py
# @author  Jakob Erdmann
# @date    2026-11-19

"""
Check Python traci domain methods against the Markdown documentation under docs/web/docs/TraCI.
"""
from __future__ import absolute_import
from __future__ import print_function

import glob
import inspect
import os
import re
import sys

SUMO_HOME = os.environ.get(
    "SUMO_HOME",
    os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))),
)
sys.path.append(os.path.join(SUMO_HOME, "tools"))
import sumolib  # noqa
import traci  # noqa
from traci.domain import Domain  # noqa


KNOWN_DEPRECATED = {
    ("simulation", "getBusStopIDList"),
    ("simulation", "getBusStopWaiting"),
    ("simulation", "getBusStopWaitingIDList"),
}


def get_options(args=None):
    ap = sumolib.options.ArgumentParser(description="Check for undocumented Python TraCI domain functions")
    ap.add_argument("-d", "--domains", default=None,
                    help="Comma-separated list of traci domain names to check (default: all domains)")
    ap.add_argument("--docs-dir", dest="docsDir",
                    default=os.path.join(SUMO_HOME, "docs", "web", "docs", "TraCI"),
                    help="Directory containing TraCI Markdown documentation")
    ap.add_argument("--exit-code", dest="exitCode", action="store_true", default=False,
                    help="Return non-zero exit status if undocumented functions are found")
    ap.add_argument("-v", "--verbose", action="store_true", default=False,
                    help="Print checked domain counts")
    return ap.parse_args(args=args)


def is_deprecated_callable(func):
    try:
        src = inspect.getsource(func)
        if "warnings.warn(" in src and "deprecated" in src.lower():
            return True
    except (OSError, TypeError):
        pass
    if getattr(func, "__closure__", None):
        for cell in func.__closure__:
            val = cell.cell_contents
            if isinstance(val, str) and "deprecated" in val.lower():
                return True
    doc = getattr(func, "__doc__", "") or ""
    return "deprecated" in doc.lower().splitlines()[0] if doc.strip() else False


def collect_doc_identifiers(docs_dir):
    words = set()
    for md_path in sorted(glob.glob(os.path.join(docs_dir, "*.md"))):
        with open(md_path, encoding="utf-8", errors="replace") as f:
            words.update(re.findall(r"\b[A-Za-z_][A-Za-z0-9_]*\b", f.read()))
    return words


def iter_domain_methods(selected_domains=None):
    base_methods = set(dir(Domain))
    for attr_name in sorted(dir(traci)):
        if attr_name.startswith("_"):
            continue
        dom = getattr(traci, attr_name)
        if not isinstance(dom, Domain):
            continue
        if getattr(dom, "_deprecatedFor", None):
            continue
        dom_name = dom._name
        if selected_domains is not None and dom_name not in selected_domains and attr_name not in selected_domains:
            continue
        cls = type(dom)
        methods = []
        for mname, mval in sorted(cls.__dict__.items()):
            if mname.startswith("_") or mname in base_methods:
                continue
            if inspect.isclass(mval) or not callable(mval):
                continue
            if (dom_name, mname) in KNOWN_DEPRECATED or is_deprecated_callable(mval):
                continue
            methods.append(mname)
        yield dom_name, methods


def main(args=None):
    options = get_options(args=args)
    selected = set(options.domains.split(",")) if options.domains else None
    doc_words = collect_doc_identifiers(options.docsDir)

    undocumented = []
    checked_domains = 0
    checked_methods = 0
    for dom_name, methods in iter_domain_methods(selected):
        checked_domains += 1
        for mname in methods:
            checked_methods += 1
            if mname not in doc_words:
                undocumented.append((dom_name, mname))

    for dom_name, mname in undocumented:
        print("Undocumented TraCI function: traci.%s.%s" % (dom_name, mname))

    if options.verbose or not undocumented:
        print("Checked %d methods across %d TraCI domains: %d undocumented." % (
            checked_methods, checked_domains, len(undocumented)))

    if undocumented and options.exitCode:
        sys.exit(1)


if __name__ == "__main__":
    main()

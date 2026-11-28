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

# @file    buildPyDoc.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2011-10-20

"""
Generates pydoc files for all python libraries.
"""
from __future__ import absolute_import
from __future__ import print_function
import os
import sys
import shutil
import pydoc
import types
import inspect
from html import escape
try:
    # this can be removed once https://github.com/python/cpython/issues/127276 has been resolved
    import importlib.resources
    css_data = importlib.resources.files('pydoc_data').joinpath('_pydoc.css').read_text()
except Exception:
    css_data = ""
try:
    import pathlib
    import pdoc
    HAVE_PDOC = True
except ImportError:
    HAVE_PDOC = False

TOOLS_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, TOOLS_DIR)
import traci  # noqa
import sumolib  # noqa
from sumolib.miscutils import working_dir  # noqa
if HAVE_PDOC:
    traci.__all__ += ["main"] + ["_" + d._name for d in traci.DOMAINS]


def pydoc_recursive(module, github_links):
    pydoc.writedoc(module)
    if css_data:
        with open(module.__name__ + ".html") as inp:
            html = inp.read()
        with open(module.__name__ + ".html", "w") as out:
            if github_links:
                html = html.replace("file:" + TOOLS_DIR, "https://github.com/eclipse-sumo/sumo/tree/main/tools")
                html = html.replace(TOOLS_DIR + "/", "")
            out.write(html.replace("</head>", "<style>%s</style></head>" % css_data))
    for submod in module.__dict__.values():
        if isinstance(submod, types.ModuleType) and submod.__name__.startswith(module.__name__):
            pydoc_recursive(submod, github_links)


def writeTraciFunctionIndex(output_dir):
    functions = [(name, function) for name, function in inspect.getmembers(traci.main, inspect.isfunction)
                 if not name.startswith("_") and function.__module__ == traci.main.__name__]
    sections = [("traci", functions)]
    for domain in sorted(traci.DOMAINS, key=lambda domain: domain._name):
        methods = [(name, method) for name, method in inspect.getmembers(domain, inspect.ismethod)
                   if not name.startswith("_")]
        sections.append(("traci." + domain._name, methods))

    path = os.path.join(output_dir, "traci-functions.html")
    with open(path, "w", encoding="utf-8") as page:
        page.write("<!DOCTYPE html>\n<html lang='en'>\n<head><meta charset='utf-8'>"
                   "<title>TraCI Python Functions</title></head>\n<body>\n<h1>TraCI Python Functions</h1>\n"
                   "<nav aria-label='Python modules and domains'><ul>\n")
        for section, _ in sections:
            page.write(f"<li><a href='#{section}'>{section}</a></li>\n")
        page.write("</ul></nav>\n<main>\n")
        for section, members in sections:
            page.write(f"<h2 id='{section}'>{section}</h2>\n<ul>\n")
            for name, function in members:
                module = function.__module__
                if HAVE_PDOC:
                    href = module.replace(".", "/") + ".html#" + function.__qualname__
                else:
                    owner = function.__qualname__.rpartition(".")[0]
                    href = f"{module}.html#{owner}-{name}"
                signature = escape(f"{section}.{name}{inspect.signature(function)}")
                page.write(f"<li><a href='{href}'><code>{signature}</code></a></li>\n")
            page.write("</ul>\n")
        page.write("</main>\n</body>\n</html>\n")


if __name__ == "__main__":
    optParser = sumolib.options.ArgumentParser()
    optParser.add_option("pydoc_output_dir", help="output folder for pydoc")
    optParser.add_option("--clean", action="store_true", default=False, help="remove output dirs")
    optParser.add_option("--local-links", action="store_true", default=False, help="keep file links")
    options = optParser.parse_args()

    if options.pydoc_output_dir:
        if options.clean:
            shutil.rmtree(options.pydoc_output_dir, ignore_errors=True)
        os.mkdir(options.pydoc_output_dir)
        if HAVE_PDOC:
            pdoc.pdoc(os.path.join(TOOLS_DIR, "sumolib"), os.path.join(TOOLS_DIR, "traci"),
                      output_directory=pathlib.Path(options.pydoc_output_dir))
        else:
            with working_dir(options.pydoc_output_dir):
                for module in (traci, sumolib):
                    pydoc_recursive(module, not options.local_links)
        writeTraciFunctionIndex(options.pydoc_output_dir)

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
sys.path.append(TOOLS_DIR)
import traci  # noqa
import sumolib  # noqa
from sumolib.miscutils import working_dir  # noqa
if HAVE_PDOC:
    traci.__all__ += ["_" + d._name for d in traci.DOMAINS]


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

#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

from contextlib import redirect_stderr
from io import StringIO
import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib.options import ArgumentParser  # noqa: E402


parser = ArgumentParser(prog="category-test")
parser.add_argument("source", help="input source", category="input")
parser.add_argument("--destination", help="output destination", category="output")
mode = parser.add_mutually_exclusive_group()
mode.add_argument("--fast", action="store_true", category="processing")
mode.add_argument("--safe", action="store_true", category="processing")

options = parser.parse_args(["source.xml", "--destination", "target.xml", "--fast"])
assert options.source == "source.xml"
assert options.destination == "target.xml"
assert options.fast and not options.safe
assert 'destination value="target.xml"' in options.config_as_string
try:
    with redirect_stderr(StringIO()):
        parser.parse_args(["source.xml", "--fast", "--safe"])
except SystemExit as error:
    assert error.code == 2
else:
    raise AssertionError("expected mutually exclusive options to be rejected")

parser.add_argument_group("custom", "Manual help group").add_argument("--manual")
help_text = parser.format_help()
sections = help_text.split("\n\n")
for title, arguments in [("input", ["source"]), ("output", ["--destination"]),
                         ("processing", ["--fast", "--safe"])]:
    matching = [section for section in sections if section.startswith(title + ":\n")]
    assert len(matching) == 1, title
    assert all(argument in matching[0] for argument in arguments), matching[0]
custom_help = help_text.partition("custom:\n")[2].partition("input:\n")[0]
assert "Manual help group" in custom_help and "--manual" in custom_help
assert "--fast | --safe" in parser.format_usage()
print("Help displays input, output, and processing options by category.")

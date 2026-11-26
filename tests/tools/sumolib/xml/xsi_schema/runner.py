#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2026-2026 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    runner.py
# @author  Josh
# @date    2026-11-26

from io import StringIO
import os
import sys
from xml.etree import ElementTree

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib.xml import create_document, parse  # noqa

namespace = "http://www.w3.org/2001/XMLSchema-instance"
xml = (
    '<additional xmlns:schema="%s" schema:noNamespaceSchemaLocation="additional_file.xsd?a=1&amp;b=2" '
    'schema:schemaLocation="urn:sample sample.xsd"><busStop id="stop" lane="e_0"/></additional>'
) % namespace
original = ElementTree.fromstring(xml)
parsed = next(parse(StringIO(xml), outputLevel=0))
assert parsed.busStop[0].id == "stop"
rendered = parsed.toXML()
round_trip = ElementTree.fromstring(rendered)
assert round_trip.attrib == original.attrib
assert round_trip[0].attrib == original[0].attrib
assert rendered.count("xmlns:xsi=") == 1
created = create_document("additional").toXML()
assert created.count("xmlns:xsi=") == 1
assert ElementTree.fromstring(created).attrib["{%s}noNamespaceSchemaLocation" % namespace].endswith(
    "/additional_file.xsd"
)
print("XSI schema locations survived the XML round trip")

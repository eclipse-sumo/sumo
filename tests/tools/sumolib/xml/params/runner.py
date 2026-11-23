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

from io import StringIO
import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib.xml import parse  # noqa

xml = """<routes>
    <vehicle id="with-params">
        <route edges="a b"><param key="nested" value="2"/></route>
        <param key="count" value="3"/>
        <param key="other" value="4"/>
        <param key="count" value="5"/>
    </vehicle>
    <vehicle id="without-params"/>
</routes>"""
with_params, without_params = parse(StringIO(xml), "vehicle", attr_conversions={"value": int})
assert with_params.getParams() == {"count": 5, "other": 4}
assert with_params.route[0].getParams() == {"nested": 2}
assert without_params.getParams() == {}
assert [param.value for param in with_params.param] == [3, 4, 5]

param_dict = with_params.getParams()
param_dict["count"] = 99
assert with_params.getParams()["count"] == 5
with_params.removeChild(with_params.param[-1])
added = with_params.addChild("param", {"key": "added", "value": 6})
assert with_params.getParams() == {"count": 3, "other": 4, "added": 6}
reparsed = next(parse(StringIO(with_params.toXML()), "vehicle", attr_conversions={"value": int}))
assert reparsed.getParams() == with_params.getParams()
assert reparsed.param[-1].value == added.value
print("XML parameter dictionary checks passed")

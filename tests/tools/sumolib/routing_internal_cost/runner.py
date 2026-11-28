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

import math
import os
import sys

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib.net import readNet  # noqa


def check_result(result, expected_path, expected_cost):
    path, cost = result
    assert path == expected_path, (path, expected_path)
    assert math.isclose(cost, expected_cost, abs_tol=1e-9), (cost, expected_cost)


filename = sys.argv[1]
net = readNet(filename, withInternal=True)
a, b, c = (net.getEdge(edge_id) for edge_id in ("a", "b", "c"))
first, intermediate, last = (net.getEdge(edge_id) for edge_id in (":n1_0", ":n2_0", ":n3_0"))
path = (first, b, intermediate, c, last)
middle = path[1:-1]
middle_length = sum(edge.getLength() for edge in middle)
middle_time = sum(edge.getLength() / edge.getSpeed() for edge in middle)
from_pos = first.getLength() / 2
to_pos = last.getLength() / 4
kwargs = dict(withInternal=True, fromPos=from_pos, toPos=to_pos)

check_result(net.getShortestPath(first, last, includeFromToCost=False, **kwargs), path, middle_length)
check_result(net.getFastestPath(first, last, includeFromToCost=False, **kwargs), path, middle_time)
check_result(net.getShortestPath(first, last, **kwargs), path, middle_length + from_pos + to_pos)
check_result(net.getFastestPath(first, last, **kwargs),
             path, middle_time + from_pos / first.getSpeed() + to_pos / last.getSpeed())
check_result(net.getShortestPath(first, last, includeFromToCost=False), (first, b, c, last), middle_length)
check_result(net.getShortestPath(c, last, fromPos=c.getLength() / 2, toPos=to_pos),
             (c, last), c.getLength() / 2 + to_pos)
check_result(net.getShortestPath(last, last, includeFromToCost=False), (last,), 0.)
assert net.getShortestPath(first, last, includeFromToCost=False, maxCost=middle_length - 0.5)[0] is None
check_result(net.getShortestPath(a, c, fromPos=10, toPos=15, ignoreDirection=True, withInternal=True),
             (a, first, b, intermediate, c),
             a.getLength() - 10 + first.getLength() + b.getLength() + intermediate.getLength() + 15)
check_result(net.getShortestPath(first, a, fromPos=from_pos, toPos=20, ignoreDirection=True, withInternal=True),
             (first, a), from_pos + a.getLength() - 20)

net.initRoutingCache()
for _ in range(2):
    check_result(net.getShortestPath(first, last, includeFromToCost=False, **kwargs), path, middle_length)
net.cacheRoute(first, c, False, None, middle_length, path[:-1])
check_result(net.getShortestPath(first, last, includeFromToCost=False, **kwargs), path, middle_length)

plain = readNet(filename, withInternal=False)
a, b, c, f = (plain.getEdge(edge_id) for edge_id in ("a", "b", "c", "f"))
check_result(plain.getShortestPath(a, c, includeFromToCost=False, fromPos=10, toPos=15),
             (a, b, c), b.getLength())
check_result(plain.getShortestPath(a, c, fromPos=10, toPos=15),
             (a, b, c), a.getLength() - 10 + b.getLength() + 15)
reverse = dict(ignoreDirection=True, fromPos=30, toPos=20)
reverse_length = 30 + a.getLength() - 20
check_result(plain.getShortestPath(b, a, **reverse), (b, a), reverse_length)
check_result(plain.getFastestPath(b, a, **reverse),
             (b, a), 30 / b.getSpeed() + (a.getLength() - 20) / a.getSpeed())
check_result(plain.getShortestPath(b, a, includeFromToCost=False, **reverse), (b, a), 0.)
check_result(plain.getShortestPath(b, f, **reverse), (b, f), b.getLength() - 30 + f.getLength() - 20)
check_result(plain.getShortestPath(b, b, **reverse), (b,), 10.)
check_result(plain.getShortestPath(b, b, fromPos=20, toPos=-10), (b,), b.getLength() - 30)
assert plain.getShortestPath(b, a, fromPos=30, toPos=20)[0] is None
assert plain.getShortestPath(b, b, fromPos=30, toPos=20)[0] is None

plain.initRoutingCache()
for _ in range(2):
    check_result(plain.getShortestPath(b, a, **reverse), (b, a), reverse_length)
plain.cacheRoute(b, a, False, None, a.getLength(), (b, a))
check_result(plain.getShortestPath(b, a, **reverse), (b, a), reverse_length)
print("Routing endpoint costs match the requested edges")

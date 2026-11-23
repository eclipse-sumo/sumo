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

import os
import sys
from xml.etree import ElementTree

import numpy as np

sys.path.append(os.path.join(os.environ["SUMO_HOME"], "tools"))
from sumolib.statistics import Statistics  # noqa

stats = Statistics("matrices", abs=True, printDev=True, printSum=True)
first = np.array([[5, 0], [-1, -2]])
second = np.array([[1, 8], [7, 2]])
stats.add(first, "first")
stats.add(second, "second")

np.testing.assert_array_equal(stats.avg(), [[3, 4], [3, 0]])
mean, deviation = stats.meanAndStdDev()
np.testing.assert_array_equal(mean, stats.avg())
np.testing.assert_array_equal(deviation, [[2, 4], [4, 2]])
np.testing.assert_array_equal(stats.median(), first)
np.testing.assert_array_equal(stats.median_abs(), np.abs(first))
np.testing.assert_array_equal(stats.quartiles()[0], second)
assert stats.min_label == "second"
assert stats.max_label == "first"
assert "sum [[6 8]" in stats.toString()
assert ElementTree.fromstring(stats.toXML()).get("minLabel") == "second"

with np.testing.assert_raises(ValueError):
    stats.add(np.array([5, 0]))
with np.testing.assert_raises(ValueError):
    stats.add(5)
assert stats.count() == 2

stats.clear()
stats.add(np.float64(4))
stats.add(np.array(6.0))
assert stats.avg() == 5
assert stats.meanAndStdDev() == (5, 1)
assert "min 4.00" in stats.toString()

vectors = Statistics("vectors")
for index in range(4):
    vectors.add(np.array([index, -index]))
np.testing.assert_array_equal(vectors.rank(0.25), [2, -2])
assert ElementTree.fromstring(vectors.toXML()).get("medianAbs") == "[2 2]"

histogram = Statistics(histogram=True)
with np.testing.assert_raises(ValueError):
    histogram.add(np.array([1, 2]))
assert histogram.count() == 0
histogram.add(np.int64(1))
assert histogram.histogram() == [(1, 1)]

print("NumPy statistics checks passed")

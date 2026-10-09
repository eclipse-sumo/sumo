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
import warnings

sys.path.insert(0, os.path.join(os.environ['SUMO_HOME'], 'tools'))
import sumolib.net  # noqa


net = sumolib.net.Net()
net.addNode('start', coord=(-10., 0., 0.))
net.addNode('end', coord=(110., 0., 0.))
edge = net.addEdge('road', 'start', 'end', 1, '', '')
pedestrian = net.addLane(edge, 1.4, 100., 3.2, allow='pedestrian')
pedestrian.setShape([(0., 0., 0.), (100., 0., 0.)])
passenger = net.addLane(edge, 13.89, 200., 3.2, allow='passenger')
passenger.setShape([(0., 4., 0.), (100., 4., 0.)])

with warnings.catch_warnings():
    warnings.filterwarnings('ignore', message="Module 'rtree' not available.*")
    lane, pos, dist = net.getNearestLane(30., 1., r=10.)
    assert lane is pedestrian and pos == 30. and dist == 1.
    print('nearest pedestrian: pos=30.0 dist=1.0')

    lane, pos, dist = net.getNearestLane(30., 1., r=10., vClass='passenger')
    assert lane is passenger and pos == 60. and dist == 3.
    print('nearest passenger: pos=60.0 dist=3.0')

    lane, pos, dist = net.getNearestLane(30., 3., r=10.)
    assert lane is passenger and pos == 60. and dist == 1.
    print('nearest without filtering: second lane')

    assert net.getNearestLane(30., 1., r=10., vClass='ignoring')[0] is pedestrian
    assert net.getNearestLane(30., 1., r=10., vClass='bicycle') is None
    assert net.getNearestLane(30., 1., r=2., vClass='passenger') is None
    assert net.getNearestLane(30., 1., r=0.5) is None
    assert net.getNearestLane(30., 1., r=1.) is None
    assert sumolib.net.Net().getNearestLane(30., 1.) is None
    print('no candidate: None')

    # A junction extension would make the passenger lane appear closer here.
    lane, pos, dist = net.getNearestLane(-5., 1., r=10.)
    assert lane is pedestrian and pos == 0.
    assert abs(dist - 26. ** 0.5) < 1e-10
    print('outside lane endpoints: clamped position')

    net.setLocation('0,0', '0,0,100,100', '0,0,100,100', '!')
    try:
        net.getNearestLane(30., 1., isGeo=True)
        raise AssertionError('missing geo-projection must raise an error')
    except RuntimeError:
        pass

    if sumolib.net.HAVE_PYPROJ:
        net.setLocation('-500000,0', '0,0,100,100', '9,0,9.01,0.01', '+proj=utm +zone=32 +datum=WGS84')
        lon, lat = net.convertXY2LonLat(30., 1.)
        lane, pos, dist = net.getNearestLane(lon, lat, r=10., vClass='passenger', isGeo=True)
        assert lane is passenger and abs(pos - 60.) < 1e-6 and abs(dist - 3.) < 1e-6

print('nearest lane checks passed')

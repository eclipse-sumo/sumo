# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    changeNet.py
# @author  Lena Kalleske
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2009-06-30
# @version $Id$

from __future__ import absolute_import

import xml.dom.minidom as dom


def changePhaseDurations(file, greenWE, yellowWE, greenNS, yellowNS):

    tree = dom.parse(file)

    net = tree.getElementsByTagName('net')[0]
    tlLogic = net.getElementsByTagName('tlLogic')[0]
    phases = tlLogic.getElementsByTagName('phase')

    phases[0].setAttribute('duration', str(greenNS))
    phases[1].setAttribute('duration', str(yellowNS))
    phases[2].setAttribute('duration', str(greenWE))
    phases[3].setAttribute('duration', str(yellowWE))

    writer = open(file, 'w')
    tree.writexml(writer)

# type = static, actuated


def setTLType(file, type):
    tree = dom.parse(file)

    net = tree.getElementsByTagName('net')[0]
    tlLogic = net.getElementsByTagName('tlLogic')[0]
    tlLogic.setAttribute('type', type)

    writer = open(file, 'w')
    tree.writexml(writer)


def setPhaseMinMax(file, minWE, maxWE, minNS, maxNS):
    tree = dom.parse(file)

    net = tree.getElementsByTagName('net')[0]
    tlLogic = net.getElementsByTagName('tlLogic')[0]
    phases = tlLogic.getElementsByTagName('phase')

    phases[0].setAttribute('duration', str(minNS))
    phases[0].setAttribute('minDur', str(minNS))
    phases[0].setAttribute('maxDur', str(maxNS))
    phases[2].setAttribute('duration', str(minWE))
    phases[2].setAttribute('minDur', str(minWE))
    phases[2].setAttribute('maxDur', str(maxWE))

    writer = open(file, 'w')
    tree.writexml(writer)


def setDetFreq(file, N):
    tree = dom.parse(file)

    dets = (tree.getElementsByTagName('additional')[0]).getElementsByTagName(
        'e1Detector')

    for det in dets:
        det.setAttribute('freq', str(N))

    writer = open(file, 'w')
    tree.writexml(writer)

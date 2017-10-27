#!/usr/bin/env python
# -*- coding: Latin-1 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    QuantityOverDay.py
# @author  Sascha Krieg
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2008-04-01
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

from pylab import *
import datetime
from matplotlib.dates import MinuteLocator, HourLocator, DateFormatter
import util.Path as path


# global vars
intervalDelta = datetime.timedelta(minutes=60)
intervalDate = datetime.datetime(2007, 7, 18, 0, 0)
format = "%Y-%m-%d %H:%M:%S"
barList = {}


def main():
    print("start program")

    countTaxis()

    # a figure (chart) where we add the bar's and change the axis properties
    fig = figure()
    ax = fig.add_subplot(111)

    # set the width of the bar to interval-size
    barWidth = date2num(intervalDate + intervalDelta) - date2num(intervalDate)
    # add a bar with specified values and width
    ax.bar(date2num(barList.keys()), barList.values(), width=barWidth)

    # set the x-Axis to show the hours
    ax.xaxis.set_major_locator(HourLocator())
    ax.xaxis.set_major_formatter(DateFormatter("%H:%M"))
    ax.xaxis.set_minor_locator(MinuteLocator())
    ax.grid(True)
    xlabel('Zeit (s)')
    ylabel('Quantit' + u'\u00E4' + 't')
    title('Menge der Taxis im VLS-Gebiet')
    ax.autoscale_view()

    # shows the text of the x-axis in a way that it looks nice
    fig.autofmt_xdate()

    # display the chart
    show()


def countTaxis():
    """Analyzes the FCD and generates a list which is used to draw the bar chart."""
    global barList
    global intervalDate
    taxis = set()

    # intervalDate+=intervalDelta
    inputFile = open(path.vls, 'r')
    for line in inputFile:
        words = line.split("\t")
        # if date >actual interval      (used intervalDate strptime function to
        # get String in a datetime-format)
        if intervalDate + intervalDelta > intervalDate.strptime(words[0], format):
            taxis.add(words[4])
            # print words
        else:
            barList[intervalDate] = len(taxis)
            intervalDate += intervalDelta
            taxis.clear()


# start the program
main()

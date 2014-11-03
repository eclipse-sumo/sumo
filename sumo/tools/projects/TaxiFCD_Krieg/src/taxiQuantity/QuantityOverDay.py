#!/usr/bin/env python
# -*- coding: Latin-1 -*-
"""
@file    QuantityOverDay.py
@author  Sascha Krieg
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2008-04-01
@version $Id$

Counts for an given interval all unique taxis in an FCD file and draws the result as a bar chart. 

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from pylab import * 
import datetime 
from matplotlib.dates import MinuteLocator, HourLocator, DateFormatter
import util.Path as path


#global vars
intervalDelta=datetime.timedelta(minutes=60)
intervalDate=datetime.datetime( 2007, 7, 18,0,0 )
format="%Y-%m-%d %H:%M:%S" 
barList={}


def main():     
    print "start program" 
    
    countTaxis()
    
    #a figure (chart) where we add the bar's and change the axis properties
    fig = figure()
    ax = fig.add_subplot(111)
    
    #set the width of the bar to interval-size
    barWidth=date2num(intervalDate+intervalDelta)-date2num(intervalDate)
    #add a bar with specified values and width
    ax.bar(date2num(barList.keys()),barList.values(),width=barWidth)
    
    #set the x-Axis to show the hours
    ax.xaxis.set_major_locator(HourLocator())
    ax.xaxis.set_major_formatter(DateFormatter("%H:%M"))
    ax.xaxis.set_minor_locator(MinuteLocator())
    ax.grid(True)
    xlabel('Zeit (s)')
    ylabel('Quantit'+u'\u00E4'+'t')
    title('Menge der Taxis im VLS-Gebiet')
    ax.autoscale_view()
    
    #shows the text of the x-axis in a way that it looks nice
    fig.autofmt_xdate()
    
    #display the chart
    show()


def countTaxis():
    """Analyzes the FCD and generates a list which is used to draw the bar chart."""    
    global barList
    global intervalDate    
    taxis=set()
    
    #intervalDate+=intervalDelta
    inputFile=open(path.vls,'r')
    for line in inputFile:
        words=line.split("\t")
        #if date >actual interval      (used intervalDate strptime function to get String in a datetime-format)
        if intervalDate+intervalDelta>intervalDate.strptime(words[0],format):
            taxis.add(words[4])
            #print words
        else:    
            barList[intervalDate]=len(taxis)
            intervalDate+=intervalDelta
            taxis.clear()
    
    
#start the program
main()

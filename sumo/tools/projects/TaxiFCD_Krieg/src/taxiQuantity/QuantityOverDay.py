# -*- coding: Latin-1 -*-
"""
@file    QuantityOverDay.py
@author  Sascha.Krieg@dlr.de
@date    2008-04-01

Counts for an given interval all unique taxis in an FCD file and draws the result as a bar chart. 

Copyright (C) 2008 DLR/FS, Germany
All rights reserved
"""

from pylab import * 
import datetime 
from matplotlib.dates import MinuteLocator, HourLocator, DateFormatter
import util.Path as path


#global vars
intervalDelta=datetime.timedelta(minutes=15)
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
    xlabel('time')
    ylabel('quantity')
    title('Taxi quantity of a day')
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
    inputFile=open(path.fcd,'r')
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
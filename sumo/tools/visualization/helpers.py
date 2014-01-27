#!/usr/bin/env python
"""
@file    helpers.py
@author  Daniel Krajzewicz
@date    2013-11-11
@version $Id$


Helper methods for plotting


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from matplotlib import rcParams
from pylab import *
from matplotlib.ticker import FuncFormatter as ff

# http://datadebrief.blogspot.de/2010/10/plotting-sunrise-sunset-times-in-python.html
def m2hm1(x, i):
    h = int(x/3600)
    m = int((x%3600)/60)
    return '%(h)02d:%(m)02d' % {'h':h,'m':m}
def m2hm2(x, i):
    h = int(x/3600)
    m = int((x%3600)/60)
    s = int(x%60)
    return '%(h)02d:%(m)02d:%(s)02d' % {'h':h,'m':m,'s':s}

def addPlotOptions(optParser):
  optParser.add_option("--colors", dest="colors", 
                         default=None, help="Defines the colors to use")
  optParser.add_option("--colormap", dest="colormap", 
                         default="spectral", help="Defines the colormap to use")
  optParser.add_option("-l", "--labels", dest="labels", 
                         default=None, help="Defines the labels to use")
  optParser.add_option("--xlim", dest="xlim", 
                         default=None, help="Defines x-limits of the figure <XMIN>,<XMAX>")
  optParser.add_option("--ylim", dest="ylim", 
                         default=None, help="Defines y-limits of the figure <YMIN>,<YMAX>")
  optParser.add_option("--xticks", dest="xticks", 
                         default=None, help="Set x-axis ticks <XMIN>,<XMAX>,<XSTEP>,<XSIZE> or <XSIZE>")
  optParser.add_option("--yticks", dest="yticks", 
                         default=None, help="Set y-axis ticks <YMIN>,<YMAX>,<YSTEP>,<YSIZE> or <YSIZE>")
  optParser.add_option("--xtime1", dest="xtime1", action="store_true",
                         default=False, help="Use a time formatter for x-ticks (hh:mm)")
  optParser.add_option("--ytime1", dest="ytime1", action="store_true",
                         default=False, help="Use a time formatter for y-ticks (hh:mm)")
  optParser.add_option("--xtime2", dest="xtime2", action="store_true",
                         default=False, help="Use a time formatter for x-ticks (hh:mm:ss)")
  optParser.add_option("--ytime2", dest="ytime2", action="store_true",
                         default=False, help="Use a time formatter for y-ticks (hh:mm:ss)")
  optParser.add_option("--xgrid", dest="xgrid", action="store_true",
                         default=False, help="Enable grid on x-axis")
  optParser.add_option("--ygrid", dest="ygrid", action="store_true",
                         default=False, help="Enable grid on y-axis")
  optParser.add_option("--xlabel", dest="xlabel", 
                         default=None, help="Set the x-axis label")
  optParser.add_option("--ylabel", dest="ylabel", 
                         default=None, help="Set the y-axis label")
  optParser.add_option("--xlabelsize", dest="xlabelsize", 
                         type="int", default=16, help="Set the size of the x-axis label")
  optParser.add_option("--ylabelsize", dest="ylabelsize", 
                         type="int", default=16, help="Set the size of the x-axis label")
  optParser.add_option("--title", dest="title", 
                         default=None, help="Set the title")
  optParser.add_option("--titlesize", dest="titlesize", 
                         type="int", default=16, help="Set the title size")
  optParser.add_option("--adjust", dest="adjust", 
                         default=None, help="Adjust the subplots <LEFT>,<BOTTOM> or <LEFT>,<BOTTOM>,<RIGHT>,<TOP>")
  optParser.add_option("-s", "--size", dest="size", 
                         default=False, help="Defines the figure size <X>,<Y>")
  optParser.add_option("--no-legend", dest="nolegend", action="store_true",
                         default=False, help="Disables the legend")

def addInteractionOptions(optParser):
  optParser.add_option("-o", "--output", dest="output", metavar="FILE",
                         default=None, help="Defines the filename the figure shall be written to")
  optParser.add_option("-b", "--blind", dest="blind", action="store_true",
                         default=False, help="If set, the figure will not be shown")



def applyPlotOptions(fig, ax, options):
  if options.xlim: xlim(float(options.xlim.split(",")[0]), float(options.xlim.split(",")[1]))
  if options.xticks:
    vals = options.xticks.split(",")
    if len(vals)==1: ax.tick_params(axis='x', which='major', labelsize=float(vals[0]))
    elif len(vals)==4: xticks(arange(float(vals[0]), float(vals[1]), float(vals[2])), size=float(vals[3]))
    else: 
      print "Error: ticks must be given as one float (<SIZE>) or four floats (<MIN>,<MAX>,<STEP>,<SIZE>)"
      sys.exit()
  if options.xtime1: ax.xaxis.set_major_formatter(ff(m2hm1))
  if options.xtime2: ax.xaxis.set_major_formatter(ff(m2hm2))
  if options.xgrid: ax.xaxis.grid(True)
  if options.xlabel: xlabel(options.xlabel, size=options.xlabelsize)

  if options.ylim: ylim(float(options.ylim.split(",")[0]), float(options.ylim.split(",")[1]))
  if options.yticks:
    vals = options.yticks.split(",")
    if len(vals)==1: ax.tick_params(axis='y', which='major', labelsize=float(vals[0]))
    elif len(vals)==4: yticks(arange(float(vals[0]), float(vals[1]), float(vals[2])), size=float(vals[3]))
    else: 
      print "Error: ticks must be given as one float (<SIZE>) or four floats (<MIN>,<MAX>,<STEP>,<SIZE>)"
      sys.exit()
  if options.ytime1: ax.yaxis.set_major_formatter(ff(m2hm1))
  if options.ytime2: ax.yaxis.set_major_formatter(ff(m2hm2))
  if options.ygrid: ax.yaxis.grid(True)
  if options.ylabel: ylabel(options.ylabel, size=options.ylabelsize)

  if options.title: title(options.title, size=options.titlesize)
  if options.adjust:
    vals = options.adjust.split(",")
    if len(vals)==2: fig.subplots_adjust(left=float(vals[0]), bottom=float(vals[1]))
    elif len(vals)==4: fig.subplots_adjust(left=float(vals[0]), bottom=float(vals[1]), right=float(vals[2]), top=float(vals[3]))
    else: 
      print "Error: adjust must be given as two floats (<LEFT>,<BOTTOM>) or four floats (<LEFT>,<BOTTOM>,<RIGHT>,<TOP>)"
      sys.exit()


def plotNet(net, colors, widths, options):
  for e in net._edges:
    gx = []
    gy = []
    for s in e._shape:
      gx.append(s[0])
      gy.append(s[1])
    if e._id in colors: c = colors[str(e._id)]
    else: c = options.defaultColor 
    if e._id in widths: w = widths[str(e._id)]
    else: w = options.defaultWidth 
    plot(gx, gy, color=c, linewidth=w)



def getColor(options, i, a):
  if options.colors:
    v = options.colors.split(",")
    if i>=len(v):
      print "Error: not enough colors given"
      sys.exit(1) 
    return v[i]
  cm = get_cmap(options.colormap) 
  cNorm  = matplotlib.colors.Normalize(vmin=0, vmax=a)
  scalarMap = matplotlib.cm.ScalarMappable(norm=cNorm, cmap=cm)
  return scalarMap.to_rgba(i)
          
def getLabel(f, i, options):
  label = f
  if options.labels:
    label = options.labels.split(",")[i]
  return label


def openFigure(options):
  if options.size:
    x = float(options.size.split(",")[0])
    y = float(options.size.split(",")[1]) 
    fig = figure(figsize=(x, y))
  else: fig = figure()
  ax = fig.add_subplot(111)
  return fig, ax


def closeFigure(fig, ax, options):
  if not options.nolegend: legend()
  applyPlotOptions(fig, ax, options)
  if options.output: savefig(options.output)
  if not options.blind: show()



def logNormalise(values, maxValue):
  if not maxValue:
    for e in values:
      if not maxValue or maxValue<values[e]:
        maxValue = values[e]
  emin = None
  emax = None
  for e in values:
    if values[e]!=0:
      values[e] = log(values[e]) / log(maxValue)
    if not emin or emin>values[e]: emin = edgeValuesC[e]
    if not emax or emax<values[e]: emax = edgeValuesC[e]
  for e in values:
    values[e] = (values[e]-emin) / (emax-emin)
  
def linNormalise(values, minColorValue, maxColorValue):
  for e in values:
    values[e] = (values[e]-minColorValue) / (maxColorValue-minColorValue)

            
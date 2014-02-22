#!/usr/bin/env python
"""
@file    plot_csv_bars.py
@author  Daniel Krajzewicz
@date    2014-01-27
@version $Id$


This script plots name / value pairs from a given .csv file (';'-separated).
The values are plotted as bars.
matplotlib (http://matplotlib.org/) has to be installed for this purpose


SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

import os, subprocess, sys, random, helpers
from matplotlib import rcParams
from pylab import *
from matplotlib.ticker import FuncFormatter as ff

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'tools'))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(__file__), '..', '..')), 'tools'))
import sumolib.output


  
def main(args=None):
  """The main function; parses options and plots"""
  ## ---------- build and read options ----------
  from optparse import OptionParser
  optParser = OptionParser()
  optParser.add_option("-i", "--input", dest="input", metavar="FILE",
                         help="Defines the csv file to use as input")
  optParser.add_option("-c", "--column", dest="column", 
                         type="int", default=1, help="Selects the column to read values from")
  optParser.add_option("-r", "--revert", dest="revert", action="store_true",
                         default=False, help="Reverts the order of read values")
  optParser.add_option("-w", "--width", dest="width", 
                         type="float", default=.8, help="Defines the width of the bars")
  optParser.add_option("--space", dest="space", 
                         type="float", default=.2, help="Defines the space between the bars")
  optParser.add_option("--norm", dest="norm", 
                         type="float", default=1., help="Divides the read numbers by this value before plotting them")
  optParser.add_option("--show-values", dest="showValues", action="store_true", 
                         default=False, help="Shows the values")
  optParser.add_option("--values-offset", dest="valuesOffset",  
                         type="float", default=1., help="Position offset for values")
  optParser.add_option("--vertical", dest="vertical", action="store_true", 
                         default=False, help="vertical bars are used")
  optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="If set, the script says what it's doing")
  # standard plot options
  helpers.addInteractionOptions(optParser)
  helpers.addPlotOptions(optParser)
  # parse
  options, remaining_args = optParser.parse_args(args=args)
  
  if options.input==None:
    print "Error: at least one csv file must be given"
    sys.exit(1)


  fd = open(options.input)
  labels = []
  vlabels = []
  vals = []
  total = 0
  xs = []
  ts = []
  s = options.width + options.space
  t = options.width / 2. + options.space / 2.
  x = options.space / 2.
  for line in fd:
    v = line.strip().split(";")
    if len(v)<2:
      continue
    labels.append(v[0].replace("\\n", "\n"))
    value = float(v[options.column]) / options.norm
    vals.append(value)
    vlabels.append(str(value) + "%")
    total += value
    xs.append(x)
    ts.append(t)
    x = x + s 
    t = t + s 

  if options.revert:
    labels.reverse()
    vals.reverse()
    vlabels.reverse()
  colors = []
  for i,e in enumerate(labels):
    colors.append(helpers.getColor(options, i, len(labels)))
  

  fig, ax = helpers.openFigure(options)
  if not options.vertical:
    rects = barh(xs, vals, height=options.width)
    for i,rect in enumerate(rects):
      if options.showValues:
        width = rect.get_width()
        ax.text(width+options.valuesOffset, rect.get_y()+rect.get_height()/2., vlabels[i], va='center', ha='left')
      rect.set_color(colors[i])
      rect.set_edgecolor('k')
    ylim(0, x)
    yticks(ts, labels)
  else:
    rects = bar(xs, vals, width=options.width)
    for i,rect in enumerate(rects):
      if options.showValues:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., height+options.valuesOffset, vlabels[i], ha='center', va='bottom')
      rect.set_color(colors[i])
      rect.set_edgecolor('k')
    xlim(0, x)
    xticks(ts, labels)
  helpers.closeFigure(fig, ax, options)

if __name__ == "__main__":
  sys.exit(main(sys.argv))
    
"""
@file    plot_net_selection.py
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2014-02-19
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2014 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
                    
import os, subprocess, sys, random, helpers
from xml.sax import make_parser
from xml.sax.handler import ContentHandler
from pylab import *
from matplotlib.ticker import FuncFormatter as ff
import matplotlib.pyplot as plt

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'tools'))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(__file__), '..', '..')), 'tools'))
import sumolib.output




def main(args=None):
  """The main function; parses options and plots"""
  ## ---------- build and read options ----------
  from optparse import OptionParser
  optParser = OptionParser()
  optParser.add_option("-n", "--net", dest="net", metavar="FILE",
                         help="Defines the network to read")
  optParser.add_option("-i", "--selection", dest="selection", metavar="FILE",
                         help="Defines the selection to read")
  optParser.add_option("--selected-width", dest="selectedWidth", 
                         type="float", default=1, help="Defines the width of selected edges")
  optParser.add_option("--color", "--selected-color", dest="selectedColor", 
                         default='r', help="Defines the color of selected edges")
  optParser.add_option("--edge-width", dest="defaultWidth", 
                         type="float", default=.2, help="Defines the width of not selected edges")
  optParser.add_option("--edge-color", dest="defaultColor", 
                         default='#606060', help="Defines the color of not selected edges")
  optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="If set, the script says what it's doing")
  # standard plot options
  helpers.addInteractionOptions(optParser)
  helpers.addPlotOptions(optParser)
  # parse
  options, remaining_args = optParser.parse_args(args=args)

  if options.net==None: 
    print "Error: a network to load must be given."
    return 1
  if options.selection==None: 
    print "Error: a selection to load must be given."
    return 1
  if options.verbose: print "Reading network from '%s'" % options.net
  net = sumolib.net.readNet(options.net)
  selection = sumolib.files.selection.read(options.selection)

  colors = {}
  widths = {}
  for e in selection["edge"]:
    colors[e] = options.selectedColor
    widths[e] = options.selectedWidth
  
  fig, ax = helpers.openFigure(options)
  ax.set_aspect("equal", None, 'C')
  helpers.plotNet(net, colors, widths, options)
  options.nolegend = True
  helpers.closeFigure(fig, ax, options)


if __name__ == "__main__":
  sys.exit(main(sys.argv))
    
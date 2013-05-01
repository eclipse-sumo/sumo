"""
@file    convert_fcd2phem.py
@author  Daniel Krajzewicz
@date    2013-01-15
@version $Id$

A script for converting SUMO's fcd-output into files readable by PHEM.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2013 DLR (http://www.dlr.de/) and contributors
All rights reserved
"""

import os, subprocess, sys, random
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'tools'))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(__file__), '..', '..')), 'tools'))

import sumolib.net
import sumolib.output.convert.phem as phem



def main(args=None):
  """The main function; parses options and converts..."""
  from optparse import OptionParser
  optParser = OptionParser()
  optParser.add_option("-i", "--fcd-input", dest="fcd", metavar="FILE",
                         help="Defines the FCD-output file to use as input")
  optParser.add_option("-n", "--net-input", dest="net", metavar="FILE",
                         help="Defines the network file to use as input")
  optParser.add_option("--dri-output", dest="dri", metavar="FILE",
                         help="Defines the name of the .dri-file to generate")
  optParser.add_option("--str-output", dest="str", metavar="FILE",
                         help="Defines the name of the .str-file to generate")
  optParser.add_option("--fzp-output", dest="fzp", metavar="FILE",
                         help="Defines the name of the .fzp-file to generate")
  optParser.add_option("--flt-output", dest="flt", metavar="FILE",
                         help="Defines the name of the .flt-file to generate")
  options, remaining_args = optParser.parse_args(args=args)
  
  # check needed values
  if options.dri or options.fzp or options.flt:
    if not options.fcd:
      print "A fcd-output must be given"
      return 1
  
  # .dri
  if options.dri:
    o = phem._getOutputStream(options.dri)
    phem.toDRI(options.fcd, o)
    phem._closeOutputStream(o)
  # .str
  if options.str or options.fzp or options.flt:
    if not options.net:
      print "A network must be given"
      return 1
    net = sumolib.net.readNet(options.net)
    o = phem._getOutputStream(options.str)
    sIDm = phem.toSTR(net, o)
    phem._closeOutputStream(o)
  # .fzp
  if options.flt or options.fzp:
    o = phem._getOutputStream(options.fzp)
    vIDm, vtIDm = phem.toFZP(options.fcd, o, sIDm)
    phem._closeOutputStream(o)
  # .flt    
  if options.flt:
    o = phem._getOutputStream(options.flt)
    phem.toFLT(None, o, vtIDm)
    phem._closeOutputStream(o)
  # exit    
  return 0


if __name__ == "__main__":
  main(sys.argv)

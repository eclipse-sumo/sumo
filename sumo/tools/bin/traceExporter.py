#!/usr/bin/env python
"""
A script for converting SUMO's fcd-output into files readable by PHEM and communication simulators.

@file    traceExporter.py
@author  Daniel Krajzewicz
@date    2013-01-15
@version $Id$

SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
Copyright (C) 2013 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""

from __future__ import print_function
import os, sys, random, datetime
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'tools'))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(__file__), '..', '..')), 'tools'))

import sumolib.net
import sumolib.output.convert.phem as phem
import sumolib.output.convert.omnet as omnet
import sumolib.output.convert.shawn as shawn
import sumolib.output.convert.ns2 as ns2
import sumolib.output.convert.gpsdat as gpsdat

class FCDVehicleEntry:
  def __init__(self, id, x, y, z, speed, typev, lane, slope):
    self.id = id
    self.x = x
    self.y = y
    self.z = z
    self.speed = speed
    self.type = typev
    self.lane = lane
    self.slope = slope


class FCDTimeEntry:
  def __init__(self, t):
    self.time = t
    self.vehicle = []
    
def disturb_gps(x, y, deviation):
    if deviation == 0:
        return x, y
    x += random.gauss(0, deviation)
    y += random.gauss(0, deviation)
    return x, y

def _getOutputStream(name):
  if not name:
    return None
  return open(name, "w")

def _closeOutputStream(strm):
  if strm: strm.close()
  

def procFCDStream(fcdstream, options):
  pt = -1 # "prior" time step
  lt = -1 # "last" time step
  ft = -1 # "first" time step
  lastExported = -1
  chosen = {}
  for i,q in enumerate(fcdstream):
    pt = lt
    lt = float(q.time.encode("latin1"))
    if ft<0:
      # this is the first step contained in the simulation
      ft = lt # save it for later purposes
    if options.begin and options.begin>lt:
      continue # do not export steps before a set begin
    if options.end and options.end<=lt:
      continue # do not export steps after a set end
    if lastExported>=0 and (options.delta and options.delta+lastExported>lt):
      continue # do not export between delta-t, if set
    lastExported = lt
    e = FCDTimeEntry(lt)
    if q.vehicle:
      for v in q.vehicle:
        if v.id not in chosen:
          chosen[v.id] = random.random() < options.penetration
        if chosen[v.id]:
          x, y = disturb_gps(float(v.x), float(v.y), options.blur)
          if v.z: z = v.z
          else: z = 0
          e.vehicle.append(FCDVehicleEntry(v.id, x, y, z, v.speed, v.type, v.lane, v.slope))
    yield e
  t = lt-pt+lt
  yield FCDTimeEntry(t)

def runMethod(inputFile, outputFile, writer, options, further={}):
    further["app"] = os.path.split(__file__)[1]
    if options.base >= 0:
        further["base-date"] = datetime.datetime.fromtimestamp(options.base)
    else:
        further["base-date"] = datetime.datetime.now()
    o = _getOutputStream(outputFile)
    fcdStream = sumolib.output.parse(inputFile, "timestep")
    ret = writer(procFCDStream(fcdStream, options), o, further)
    _closeOutputStream(o)
    return ret


def main(args=None):
  """The main function; parses options and converts..."""
  ## ---------- build and read options ----------
  from optparse import OptionParser
  optParser = OptionParser()
  optParser.add_option("-i", "--fcd-input", dest="fcd", metavar="FILE",
                         help="Defines the FCD-output file to use as input")
  optParser.add_option("-n", "--net-input", dest="net", metavar="FILE",
                         help="Defines the network file to use as input")
  optParser.add_option("-p", "--penetration", type="float", dest="penetration", 
                         default=1., help="Defines the percentage (0-1) of vehicles to export")
  optParser.add_option("-b", "--begin", dest="begin", 
                         type="float", help="Defines the first step to export")
  optParser.add_option("-e", "--end", dest="end", 
                         type="float", help="Defines the first step not longer to export")
  optParser.add_option("-d", "--delta-t", dest="delta", 
                         type="float", help="Defines the export step length")
  optParser.add_option("--gps-blur", dest="blur", default=0,
                         type="float", help="Defines the GPS blur")
  optParser.add_option("-s", "--seed", dest="seed", default=0,
                         type="float", help="Defines the randomizer seed")
  optParser.add_option("--base-date", dest="base", default=-1, type="int", help="Defines the base date")
  # PHEM
  optParser.add_option("--dri-output", dest="dri", metavar="FILE",
                         help="Defines the name of the PHEM .dri-file to generate")
  optParser.add_option("--str-output", dest="str", metavar="FILE",
                         help="Defines the name of the PHEM .str-file to generate")
  optParser.add_option("--fzp-output", dest="fzp", metavar="FILE",
                         help="Defines the name of the PHEM .fzp-file to generate")
  optParser.add_option("--flt-output", dest="flt", metavar="FILE",
                         help="Defines the name of the PHEM .flt-file to generate")
  # OMNET
  optParser.add_option("--omnet-output", dest="omnet", metavar="FILE",
                         help="Defines the name of the OMNET file to generate")
  # Shawn
  optParser.add_option("--shawn-output", dest="shawn", metavar="FILE",
                         help="Defines the name of the Shawn file to generate")
  # ns2
  optParser.add_option("--ns2activity-output", dest="ns2activity", metavar="FILE",
                         help="Defines the name of the ns2 file to generate")
  optParser.add_option("--ns2config-output", dest="ns2config", metavar="FILE",
                         help="Defines the name of the ns2 file to generate")
  optParser.add_option("--ns2mobility-output", dest="ns2mobility", metavar="FILE",
                         help="Defines the name of the ns2 file to generate")
  # GPSDAT
  optParser.add_option("--gpsdat-output", dest="gpsdat", metavar="FILE",
                         help="Defines the name of the gpsdat file to generate")
  # parse
  options, remaining_args = optParser.parse_args(args=args)
  
  if options.seed:
    random.seed(options.seed)
  ## ---------- process ----------
  net = None
  ## ----- check needed values
  if options.delta and options.delta<=0:
    print("delta-t must be a positive value.")
    return 1
  # phem
  if (options.dri or options.fzp or options.flt) and not options.fcd:
    print("A fcd-output from SUMO must be given using the --fcd-input.")
    return 1
  if (options.str or options.fzp or options.flt) and not options.net:
    print("A SUMO network must be given using the --net-input option.")
    return 1
  # omnet
  if options.omnet and not options.fcd:
    print("A fcd-output from SUMO must be given using the --fcd-input.")
    return 1
  ## ----- check needed values
  
  ## ----- OMNET
  if options.omnet: runMethod(options.fcd, options.omnet, omnet.fcd2omnet, options)
  ## ----- OMNET

  ## ----- Shawn
  if options.shawn: runMethod(options.fcd, options.shawn, shawn.fcd2shawn, options)
  ## ----- Shawn

  ## ----- GPSDAT
  if options.gpsdat: runMethod(options.fcd, options.gpsdat, gpsdat.fcd2gpsdat, options)
  ## ----- GPSDAT

  ## ----- ns2
  if options.ns2mobility or options.ns2config or options.ns2activity: 
    vIDm, vehInfo, begin, end, area = runMethod(options.fcd, options.ns2mobility, ns2.fcd2ns2mobility, options)
  if options.ns2activity: 
    o = _getOutputStream(options.ns2activity)
    ns2.writeNS2activity(o, vehInfo)
    _closeOutputStream(o)
  if options.ns2config: 
    o = _getOutputStream(options.ns2config)
    ns2.writeNS2config(o, vehInfo, options.ns2activity, options.ns2mobility, begin, end, area)
    _closeOutputStream(o)
  ## ----- ns2

  ## ----- PHEM
  # .dri
  if options.dri: runMethod(options.fcd, options.dri, phem.fcd2dri, options)
  # .str (we need the net for other outputs, too)
  if options.str or options.fzp or options.flt:
    if not options.net:
      print("A SUMO network must be given using the --net-input option.")
      return 1
    if not net: net = sumolib.net.readNet(options.net)
    o = _getOutputStream(options.str)
    sIDm = phem.net2str(net, o)
    _closeOutputStream(o)
  # .fzp
  if options.flt or options.fzp: 
    vIDm, vtIDm = runMethod(options.fcd, options.fzp, phem.fcd2fzp, options, {"phemStreetMap":sIDm})
  # .flt    
  if options.flt:
    o = _getOutputStream(options.flt)
    phem.vehicleTypes2flt(o, vtIDm)
    _closeOutputStream(o)
  ## ----- PHEM
  return 0


if __name__ == "__main__":
  sys.exit(main(sys.argv))
  

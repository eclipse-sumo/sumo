# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2023 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2021 University of Bologna - DICAM
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    sumopy_sim.py
# @author  Joerg Schweizer
# @date   2012

"""
sumopy_sim is a script to open and run a SUMOPy scenario

Usage: python sumopy_sim.py scenario.obj

"""
from agilepy.lib_base.logger import Logger
from coremodules.simulation import sumo
from coremodules.scenario import scenario
import sys
import os
import numpy as np

# point to the SUMOPy directory here
sys.path.append(os.path.join('..', '..'))


resultfilepath = None
if len(sys.argv) >= 2:
    simfilepath = sys.argv[1]
    if len(sys.argv) == 3:
        resultfilepath = sys.argv[2]
else:
    print __doc__
    sys.exit(0)

myscenario = scenario.load_scenario(simfilepath)
rootfilepath = myscenario.get_rootfilepath()
if resultfilepath is None:
    resultfilepath = rootfilepath+'.res.obj'

mylogger = Logger(  # filepath = os.path.join(dirpath,logfilepath),
    is_stdout=True,  # False
)

microsim = sumo.Sumo(myscenario,
                     guimode='nogui',  # 'sumopy','sumopy+map','native','openscene','nogui',
                     simtime_start=0,
                     simtime_end=600,
                     time_to_teleport=-1,
                     time_step=0.2,  # s
                     is_ballistic_integrator=True,
                     #
                     # routing options
                     #
                     is_dynaroute=True,  # = one shot assignment
                     is_rerouting=False,  # enable rerouting devices
                     probability_rerouting=0.4,
                     is_deterministic_rerouting=False,
                     period_rerouting=180,
                     preperiod_rerouting=180,
                     adaptationinterval_rerouting=180,
                     adaptationweight_rerouting=0.0,
                     adaptationsteps_rerouting=10,
                     #
                     # decide what results are needed
                     #
                     is_edgedata=True,
                     is_tripdata=True,
                     is_edgenoise=True,
                     is_edgesemissions=True,
                     time_warmup=0,  # s start sampling at this time
                     time_sample=60,  # s result sampling
                     #
                     # run options
                     #
                     # is_runnow = False,# run immediately after init
                     is_start=True,  # True,
                     is_quit_on_end=True,
                     #is_run_background = False,
                     #is_nohup = False,
                     #
                     # export options
                     #
                     is_export_net=True,
                     is_export_poly=False,
                     is_export_rou=True,
                     is_export_ptstops=True,
                     #
                     # other options
                     #
                     seed=1234,
                     is_collission_check_junctions=True,
                     collission_action='warn',
                     is_exclude_emptyedges=True,
                     is_exclude_emptylanes=True,
                     is_include_poly=True,
                     #logfilepath = sumologfilepath,
                     logger=mylogger,
                     )

microsim.do()  # now open SUMO and run simulation


if resultfilepath is not None:
    print 'saving results in', resultfilepath
    myscenario.simulation.results.save(resultfilepath)

# import all results from xml and put them into myscenario.simulation.results
microsim.import_results()
edgeres = myscenario.simulation.results.edgeresults
tripres = myscenario.simulation.results.tripresults

edgeres.export_csv(rootfilepath+'.edgeres.csv',
                   sep=',',
                   # ids = [ list wit ids to export],
                   show_parentesis=True,
                   # name_id='ID',
                   is_header=True)

tripres.export_csv(rootfilepath+'.tripres.csv',
                   sep=',',
                   # ids = [ list wit ids to export],
                   show_parentesis=True,
                   # name_id='ID',
                   is_header=True)

# do some analyses
ids_tripres = tripres.get_ids()
print 'numer of arrived vehicles:', len(ids_tripres)
print 'Total triplength: %.2fKm' % (0.001*np.mean(tripres.routeLength[ids_tripres]))
print 'Average speed: %.2fKm/s' % (3.6*np.mean(tripres.routeLength[ids_tripres]/tripres.duration[ids_tripres]))

ids_edgeres = edgeres.get_ids()
print 'Total fuel consumption: %.2f liter' % (0.001*np.sum(edgeres.fuel_abs[ids_edgeres]))

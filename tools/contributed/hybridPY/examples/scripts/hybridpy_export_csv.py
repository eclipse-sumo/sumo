# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2026 German Aerospace Center (DLR) and others.
# hybridPY module
# Copyright (C) 2012-2026 University of Bologna - DICAM, Technical University of Munich
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    hybridpy_export_csv.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012

"""
hybridPY_export_csv is a script to open a hybridPY scenario, load and export results

Usage: python hybridPY_export_csv.py myscenario.obj myscenario.res.obj

"""
from agilepy.lib_base.logger import Logger
from coremodules.simulation import results_mpl as results_mpl
from coremodules.simulation import results
from coremodules.simulation import sumo
from coremodules.scenario import scenario
import sys
import os
import numpy as np
import csv

# point to the hybridPY directory here
sys.path.append(os.path.join('..', '..'))


if len(sys.argv) >= 2:
    simfilepath = sys.argv[1]
    if len(sys.argv) == 3:
        resultfilepath = sys.argv[2]
else:
    print(__doc__)
    sys.exit(0)

myscenario = scenario.load_scenario(simfilepath)
rootfilepath = myscenario.get_rootfilepath()
if resultfilepath is None:
    resultfilepath = rootfilepath+'.res.obj'

mylogger = Logger(  # filepath = os.path.join(dirpath,logfilepath),
    is_stdout=True,  # False
)


print(rootfilepath)


# import all results from xml and put them into myscenario.simulation.results
myscenario.simulation.results = results.load_results(resultfilepath,
                                                     parent=myscenario.simulation,
                                                     logger=mylogger
                                                     )

edgeres = myscenario.simulation.results.edgeresults
tripres = myscenario.simulation.results.tripresults
######## MIE MODIFICHE#####################################################
edgeresevo = myscenario.simulation.results.edgeresultsevolution
print('[INFO: RESULT EVOLUTION]', edgeresevo)
# print dir (edgeresevo)
# print edgeres
# print type(tripres)
##############################################################################

if 0:
    results_mpl.EdgeResultEvolutionplotter(myscenario.simulation.results,
                                           framenumber_begin=0,
                                           framenumber_end=10,
                                           plottypes='polygons',
                                           edgeattrname="density",
                                           valuelabel="Density [veh/km]",
                                           is_save=True,
                                           is_create_anim=True,
                                           format_anim='mp4',  # ['mp4','gif']
                                           delay_anim=10,  # sec
                                           n_loops_anim=10,
                                           ).do()

######## MIE MODIFICHE#####################################################
if 1:
    edgeresevo.export_csv(rootfilepath+'.edgeresevo4.csv',
                          sep=',',
                          # ids = [ list wit ids to export],
                          show_parentesis=True,
                          is_timestamp=False,
                          is_ident=False,
                          # name_id='ID',
                          is_header=False)

##############################################################################
# do some analyses

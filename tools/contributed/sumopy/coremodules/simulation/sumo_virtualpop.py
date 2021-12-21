# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2021 German Aerospace Center (DLR) and others.
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

# @file    sumo_virtualpop.py
# @author  Joerg Schweizer
# @date   2012

"""
Script to update virtualpop, to prepare a simulation and to 
write the sumo commandline in cmlfile and to write 
simulation results to a resultfile.

Usage: python sumo_virtualpop.py <scenariofile> <cmlfile> <resultfile>

"""
# Import some modules

from agilepy.lib_base.logger import Logger
from coremodules.simulation import sumo
from coremodules.demand import virtualpop_results as vpr
from coremodules.scenario import scenario as scen
import agilepy.lib_base.arrayman as am
import agilepy.lib_base.classman as cm
import sys
import os
import numpy as np
try:
    APPDIR = os.path.dirname(os.path.abspath(__file__))
except:
    APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))
SUMOPYDIR = os.path.join(APPDIR, '..', '..', 'sumopy')
sys.path.append(SUMOPYDIR)

if __name__ == '__main__':

    filepath = sys.argv[1]
    cmlfilepath = sys.argv[2]
    if len(sys.argv) > 3:
        filepath_simstats = sys.argv[3]
    else:
        filepath_simstats = None

print 'sys.path', sys.path


#from coremodules.demand import vehicles as ve
#from coremodules.demand import virtualpop as vp

###########################################################


# total duration in seconds to be simulated
# if values is below zero the default values are used
simduration = 10000

# plan selection parameters
c_probit = 0.1
c_probit_ini = 0.0
fraction = 0.6
decrease = 0.1


# other
seed = 1234
logfilepath = 'log.txt'
sumologfilepath = 'log_sumo.txt'
###########################################################
print 'script_vp: initialize sim cmlfilepath=', cmlfilepath

# Load the scenario

# print 'Load scenario "%s"'%filepath

myscenario = scen.load_scenario(filepath)
rootfilepath = myscenario.get_rootfilepath()
if filepath_simstats is None:
    filepath_simstats = rootfilepath+'.vpiterres.obj'
dirpath = os.path.dirname(filepath)
mylogger = Logger(filepath=os.path.join(dirpath, logfilepath),
                  is_stdout=True,  # False
                  )

is_not_first_iter = os.path.isfile(cmlfilepath)

##############################################################################
# choose from lanechange-models['LC2013','JE2013','DK2008','SL2015']
myscenario.demand.vtypes.lanechangemodel.set_value('SL2015')

simtime_start = myscenario.demand.get_time_depart_first()

# if simduration > 0:
##    simtime_end = simtime_start+simduration
# else:
##    simtime_end = myscenario.demand.get_time_depart_last()

simtime_end = simtime_start+simduration
# export net only during first iteration
is_export_net = not is_not_first_iter

sim = sumo.Sumo(myscenario,
                cmlfilepath=cmlfilepath,
                #results = None,
                guimode='nogui',  # 'sumopy','sumopy+map','native','openscene','nogui',
                simtime_start=simtime_start,
                simtime_end=simtime_end,
                time_warmup=0,
                time_to_teleport=180,
                time_step=0.2,  # s
                #
                # decide what results are needed
                #
                is_edgedata=True,
                is_tripdata=True,
                is_edgenoise=True,
                is_edgesemissions=True,
                time_sample=60,
                #
                # pedestrian options
                #
                pedestrian_model='striping',
                width_pedestrian_striping=0.35,
                slowdownfactor_pedestrian_striping=0.2,
                jamtime_pedestrian_striping=10,
                #
                # run options
                #
                is_dynaroute=True,
                is_runnow=False,
                is_start=True,  # True,
                is_quit_on_end=True,
                is_run_background=False,
                is_nohup=False,
                #
                # export options
                #
                is_export_net=is_export_net,
                is_export_poly=is_export_net,
                is_export_rou=True,
                is_export_ptstops=is_export_net,
                #
                # other options
                #
                seed=seed,
                is_collission_check_junctions=True,
                collission_action='none',
                is_exclude_emptyedges=True,
                is_exclude_emptylanes=True,
                is_include_poly=True,
                logfilepath=sumologfilepath,
                logger=mylogger,
                )

##############################################################################

virtualpop = myscenario.demand.virtualpop
ids_pers = virtualpop.get_ids()

if is_not_first_iter:
    # load previous results, if cmlfilepath is present
    print '  importing and procerroessing previous results'

    # store plans before reselection
    ids_plan_before = virtualpop.ids_plan[ids_pers].copy()
    # load previous results
    simstats = cm.load_obj(filepath_simstats)
    # number of iteration
    n_iter = len(simstats)
    n_iter += 1
    # decrease c_probit and fraction for the nex iteration
    c_probit = c_probit/(decrease+1)**n_iter
    fraction = fraction/(decrease+1)**n_iter
    print 'n_iter:', n_iter, 'c_probit:', c_probit, 'fraction:', fraction
    # select plans according to last simulation results
    sim.import_results()  # will update plan travel times
    virtualpop.select_plans_min_time_exec_est(fraction=fraction,
                                              c_probit=c_probit)
    # get plans after selection
    ids_plan_after = virtualpop.ids_plan[ids_pers]
    # print '  simstats before adding results ',len(simstats)
    simstats.add_results(myscenario, ids_plan_before, ids_plan_after)
    # print '  simstats after adding results',len(simstats)


else:
    print '  first iteration'
    # select all plans with the minimum estimated time
    virtualpop.select_plans_min_time_est(fraction=1.0,
                                         c_probit=c_probit_ini)
    # initialize and safe simulation object
    simstats = vpr.IterateStatistics(myscenario)

cm.save_obj(simstats, filepath_simstats)

# safe scenario for next step
# myscenario.save(filepath)
myscenario.save()

# No simulation, only write configuration file .netc.xml
# and SUMO command line command in cmlfilepath
sim.do()

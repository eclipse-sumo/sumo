"""
hybridPY_evores is a script to open a hybridPY scenario, run simulation and export results

Usage: python hybridPY_evores.py myscenario.obj

"""
import sys, os
import numpy as np
import csv

# point to the hybridPY directory here
sys.path.append(os.path.join('..','..','hybridPY'))

from coremodules.scenario import scenario
from coremodules.simulation import sumo
from agilepy.lib_base.logger import Logger

resultfilepath = None
if len(sys.argv)>=2: 
    simfilepath = sys.argv[1]
    if len(sys.argv)==3:
         resultfilepath = sys.argv[2]
else:
    print(__doc__)
    sys.exit(0)

myscenario = scenario.load_scenario(simfilepath)
rootfilepath = myscenario.get_rootfilepath()
if resultfilepath is None:
    resultfilepath = rootfilepath+'_evotest'+'.res.obj'

mylogger = Logger(#filepath = os.path.join(dirpath,logfilepath),
                  is_stdout = True,#False
                  )

microsim = sumo.Sumo(    myscenario, 
                    guimode = 'nogui',# 'hybridPY','hybridPY+map','native','openscene','nogui',  
                    simtime_start = 8*3600,
                    simtime_end = 12*3600,
                    time_to_teleport = -1,
                    time_step = 0.5,#s
                    is_ballistic_integrator = True,
                    #
                    # routing options
                    #
                    is_dynaroute = False,# = one shot assignment
                    is_rerouting = False,# enable rerouting devices
                    probability_rerouting = 0.4,
                    is_deterministic_rerouting = False,
                    period_rerouting = 180,
                    preperiod_rerouting = 180,
                    adaptationinterval_rerouting = 180,
                    adaptationweight_rerouting = 0.0,
                    adaptationsteps_rerouting = 10,
                    #
                    # decide what results are needed
                    #
                    is_edgedata = True,
                    is_result_evolution = True,
                    time_interval_evolution = 900,
                    is_tripdata = True,
                    is_edgenoise = True,
                    is_edgesemissions = True,
                    time_warmup = 0,#s start sampling at this time 
                    time_sample = 900, #s result sampling
                    #
                    # run options
                    #
                    #is_runnow = False,# run immediately after init
                    is_start = True,#True,
                    is_quit_on_end = True,
                    #is_run_background = False,
                    #is_nohup = False,
                    #
                    # export options
                    #
                    is_export_net = True,
                    is_export_poly = False,
                    is_export_rou = True,
                    is_export_ptstops = True,
                    #
                    # other options
                    #
                    seed = 1234,
                    is_collission_check_junctions = True,
                    collission_action ='warn',
                    is_exclude_emptyedges = True,
                    is_exclude_emptylanes = True,
                    is_include_poly = True,
                    #logfilepath = sumologfilepath,
                    logger = mylogger,
                    )

microsim.do() # now open SUMO and run simulation

# import all results from xml and put them into myscenario.simulation.results
microsim.import_results()

if resultfilepath is not None:
    print('saving results in',resultfilepath)
    myscenario.simulation.results.save(resultfilepath)



if 0:
    myscenario.simulation.results = results.load_results(resultfilepath, 
                                    parent = myscenario.simulation, 
                                    logger = mylogger
                                    )#

edgeres = myscenario.simulation.results.edgeresults
tripres =  myscenario.simulation.results.tripresults
######## MIE MODIFICHE#####################################################
edgeresevo = myscenario.simulation.results.edgeresultsevolution
print('[INFO: RESULT EVOLUTION]', edgeresevo)
#print dir (edgeresevo)
#print edgeres
#print type(tripres)
##############################################################################

if 0:
    results_mpl.EdgeResultEvolutionplotter(myscenario.simulation.results,
                                           framenumber_begin = 0,
                                           framenumber_end = 10,
                                           plottypes ='polygons',
                                           edgeattrname="density",
                                           valuelabel = "Density [veh/km]",
                                           is_save = True,
                                           is_create_anim = True,
                                           format_anim = 'mp4',#['mp4','gif']
                                           delay_anim = 10,# sec
                                           n_loops_anim = 10,#
                                           ).do()

######## MIE MODIFICHE#####################################################
if 1:
    edgeresevo.export_csv(rootfilepath+'.edgeresevo3.csv',
                                        sep = ',',
                                        #ids = [ list wit ids to export], 
                                        show_parentesis = True,
                                        is_timestamp = False,
                                        is_ident = False,
                                        #name_id='ID',
                                        is_header = False)

##############################################################################
# do some analyses

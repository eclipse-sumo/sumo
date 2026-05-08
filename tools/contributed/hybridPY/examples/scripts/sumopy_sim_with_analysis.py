"""
hybridPY_sim is a script to open and run a hybridPY scenario

Usage: python hybridPY_sim.py scenario.obj

"""
import sys, os
import numpy as np

# point to the hybridPY directory here
sys.path.append(os.path.join('..','..'))

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
#myscenario = scenario.load_scenario("schwabing_Base_gtfs_ext-1000.obj")
rootfilepath = myscenario.get_rootfilepath()
if resultfilepath is None:
    resultfilepath = rootfilepath+'.res.obj'

mylogger = Logger(#filepath = os.path.join(dirpath,logfilepath),
                  is_stdout = True,#False
                  )

microsim = sumo.Sumo(    myscenario, 
                    guimode = 'hybridPY',# 'hybridPY','hybridPY+map','native','openscene','nogui',  
                    simtime_start = 21600.0,
                    simtime_end = 23600.0,
                    time_to_teleport = -1,
                    time_step = 1.0,#s
                    is_ballistic_integrator = True,
                    #
                    # routing options
                    #
                    is_dynaroute = True,# = one shot assignment
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
                    is_tripdata_unfinished = True,
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
    


edgeres = myscenario.simulation.results.edgeresults
edgeres_evolution = myscenario.simulation.results.edgeresultsevolution

#val = edgeres_evolution.entered[id_edge][interval]

tripres =  myscenario.simulation.results.tripresults

edgeres.export_csv(rootfilepath+'.edgeres.csv', 
                                    sep = ',', 
                                    #ids = [ list wit ids to export], 
                                    show_parentesis = True, 
                                    #name_id='ID',
                                    is_header = True)

tripres.export_csv(rootfilepath+'.tripres.csv', 
                                    sep = ',', 
                                    #ids = [ list wit ids to export], 
                                    show_parentesis = True, 
                                    #name_id='ID',
                                    is_header = True)

# do some analyses
ids_tripres =  tripres.get_ids()
print('numer of arrived vehicles:',len(ids_tripres))
print('Total triplength: %.2fKm'%(0.001*np.mean(tripres.routeLength[ids_tripres])))
print('Average speed: %.2fKm/h'%(3.6*np.mean(tripres.routeLength[ids_tripres]/tripres.duration[ids_tripres])))

ids_edgeres =  edgeres.get_ids()
print('Total fuel consumption: %.2f liter'%(0.001*np.sum(edgeres.fuel_abs[ids_edgeres])))

######### testing:

virtualpop = myscenario.demand.virtualpop
ids_pers = virtualpop.get_ids()
n_pers = len(ids_pers)

plans = virtualpop.get_plans()
ids_plan = plans.get_ids()
ids_strategy = plans.ids_strategy[ids_plan]
times_est = plans.times_est[ids_plan]
times_exec = plans.times_exec[ids_plan]
strategytable = virtualpop.get_strategies()

ids_strat = strategytable.get_ids()
n_strat = len(ids_strat)
index_strat = np.arange(n_strat)
names_strat = np.zeros(n_strat, dtype = np.object_)
meantraveltimes_est = np.zeros(n_strat, dtype = np.float32)
meantraveltimes_exec = np.zeros(n_strat, dtype = np.float32)

for i, id_strat, strategy in zip(index_strat, ids_strat, strategytable.strategies[ids_strat]):
    names_strat[i] = strategy.get_name() 
    inds_strat = np.flatnonzero((ids_strategy == id_strat)&(times_est>0)&(times_exec>0))
    meantraveltimes_est[i]  =  np.mean( times_est[inds_strat] )
    meantraveltimes_exec[i]  =  np.mean( times_exec[inds_strat] )
    print(('Mean travel time of stategy' + str(names_strat[i]) + ' = ' + str(meantraveltimes_exec[i])))

"""
hybridPY_export_csv is a script to open a hybridPY scenario, load and export results

Usage: python hybridPY_export_csv.py myscenario.obj myscenario.res.obj

"""
import sys, os
import numpy as np
import csv

# point to the hybridPY directory here
sys.path.append(os.path.join('..','..'))

from coremodules.scenario import scenario
from coremodules.simulation import sumo
from coremodules.simulation import results
from coremodules.simulation import results_mpl as results_mpl
from agilepy.lib_base.logger import Logger


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
    resultfilepath = rootfilepath+'.res.obj'

mylogger = Logger(#filepath = os.path.join(dirpath,logfilepath),
                  is_stdout = True,#False
                  )


print(rootfilepath)


# import all results from xml and put them into myscenario.simulation.results
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
    edgeresevo.export_csv(rootfilepath+'.edgeresevo4.csv',
                                        sep = ',',
                                        #ids = [ list wit ids to export], 
                                        show_parentesis = True,
                                        is_timestamp = False,
                                        is_ident = False,
                                        #name_id='ID',
                                        is_header = False)

##############################################################################
# do some analyses

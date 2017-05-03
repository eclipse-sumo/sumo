"""
@author Leonhard Luecken
@date   2017-04-09
"""

import os
import traci
import simpla

# TODO: platoon-split is delayed due to safety constraints, although members have separated long ago (see pkw47296, at 21780)  

if __name__=="__main__":    
    filepath = os.path.realpath(__file__)
    sumoCfgFile = os.path.join(os.path.dirname(filepath), "AIMsubnet", "AIMsubnet.sumocfg")
    simplaCfgFile = os.path.join(os.path.dirname(filepath), "AIMsubnet", "simpla_example.cfg.xml")
    traci.start(["sumo-guiD", "-c", sumoCfgFile, "--start"])
    #traci.simulationStep(1000*3600) # warm up simulation
    
    simpla.load(simplaCfgFile)

    for i in range(10000):
        traci.simulationStep()
        if not simpla._useStepListener:
            ## for SUMO version < 0.30
            simpla.update()
        if i%5 == 0:
            pass
        
    traci.close()
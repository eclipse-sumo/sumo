import os,sys
sys.path.append('/home/kobo/dlr/sumo/tools')
import traci

traci.start(['sumo', '-c', 'test.sumocfg', '--no-step-log'])
while traci.simulation.getMinExpectedNumber() > 0:
    traci.simulationStep()
    try:
        print(traci.simulation.getCurrentTime() / 1000.0,
                traci.vehicle.getDistance('Linie'),
                traci.vehicle.getRoadID('Linie'))
    except traci.TraCIException:
        pass
traci.close()

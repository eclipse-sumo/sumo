import sys
import os
try:
    sys.path.append(os.path.join(os.environ['SUMO_HOME'], 'tools'))  # tutorial in tests
    from sumolib import checkBinary

except (KeyError, ImportError):
    sys.exit(
        "please declare environment variable 'SUMO_HOME' as the root directory of your sumo installation (it should "
        "contain folders 'bin', 'tools' and 'docs')")

import traci


if __name__ == "__main__":
    binary = sys.argv[1]
    args = [binary, "-c", "buses.sumocfg"]
    traci.start(args)
    step = 0
    done = False
    bus_id = "bus"
    while True:
        step += 1
        try:
            if not traci.simulation.getMinExpectedNumber() > 0:
                break
        except traci.exceptions.FatalTraCIError as e:
            break
        if not done:
            next_stops = traci.vehicle.getNextStops(bus_id)
            if len(next_stops) > 0:
                stop = next_stops[0]
                bus_stop_pos = stop[1]
                if stop[2] == "busStop2" and traci.vehicle.getLaneID(bus_id) == stop[0]:
                    bus_pos = traci.vehicle.getLanePosition(bus_id)
                    if bus_stop_pos - bus_pos < 10:
                        traci.vehicle.setBusStop(bus_id, "busStop2", 0, 0, 0)
                        print("abort stop at busStop2 at t=%s" % traci.simulation.getTime())
                        done = True
        try:
            traci.simulationStep()
        except traci.exceptions.FatalTraCIError as e:
            break
    traci.close()


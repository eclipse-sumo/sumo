#!/usr/bin/env python
"""
@file    embedded.py
@author  Lena Kalleske
@author  Daniel Krajzewicz
@author  Michael Behrisch
@date    2009-03-26
@version $Id$

Tutorial for traffic light control via the TraCI interface.

SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
Copyright (C) 2009-2013 DLR/TS, Germany
All rights reserved
"""

import sys
import runner

# this is the main entry point of this script
if __name__ == "__main__":
    options = runner.get_options()

    if runner.traci.isEmbedded():
        # this script has been called from the sumo-internal python interpreter
        # only execute the main control procedure
        runner.run()
    else:
        # this script has been called from the command line. It will start sumo as a
        # server, then connect and run
        if options.nogui:
            sumoBinary = runner.checkBinary('sumo')
        else:
            sumoBinary = runner.checkBinary('sumo-gui')

        # first, generate the route file for this simulation
        runner.generate_routefile()

        # now execute sumo
        sumoConfig = 

        # call sumo with the request to run this very same script again in the internal interpreter
        # when this happens, the method traci.isEmbedded() in line 23 will evaluate to true
        # and then the run method will be called
        retCode = subprocess.call([sumoBinary, "-c", "data/cross.sumocfg", "--python-script", __file__], stdout=sys.stdout)
        sys.exit(retCode)

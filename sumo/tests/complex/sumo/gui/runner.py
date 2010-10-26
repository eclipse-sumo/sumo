#!/usr/bin/env python

import os,subprocess,sys,time
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools", "lib"))
import testUtil

guisimBinary = testUtil.checkBinary('sumo-gui')
for run in range(20):
    p = subprocess.Popen([guisimBinary, "-Q", "-N", "-c", "sumo.sumo.cfg"])
    time.sleep(1)
    testUtil.findAndClick(testUtil.PLAY)
    time.sleep(10)
    for step in range(3):
        testUtil.findAndClick(testUtil.STOP)
        time.sleep(1)
        testUtil.findAndClick(testUtil.PLAY)
        time.sleep(1)
    p.wait()
    subprocess.call(["diff", "v.xml", "vehroutes.xml"])

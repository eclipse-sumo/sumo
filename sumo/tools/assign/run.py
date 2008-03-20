#!/usr/bin/env python
"""
@file    run.py
@author  Michael.Behrisch@dlr.de
@date    2008-03-10
@version $Id$

Runs the assignment tests.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""
import glob, os, shutil, subprocess, time, optparse

def makeAndChangeDir(dirName):
    runID = 1
    fullName = "%s%03i" % (dirName, runID)
    while os.path.exists(fullName):
        runID += 1
        fullName = "%s%03i" % (dirName, runID)
    os.mkdir(fullName)
    os.chdir(fullName)
    return fullName

def execute(command):
    if options.verbose:
        print command 
    os.system(command)

optParser = optparse.OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-o", "--use-od2trips", action="store_true", dest="od2trips",
                     default=False, help="use od2trips instead of trips from incremental assignment")
optParser.add_option("-s", "--statistics", dest="stats", type="int",
                     default=0, help="use od2trips instead of trips from incremental assignment")
(options, args) = optParser.parse_args()

os.chdir("input")
netFile = "../input/" + glob.glob("*.net.xml")[0]
mtxNamesList = ",".join(["../input/" + item for item in glob.glob("*.fma")])
addFiles = ",".join(["../input/" + item for item in glob.glob("*.add.xml")])
pyAdds = ""
sumoAdds = ""
if addFiles:
    pyAdds = "-+ %s" % addFiles
    sumoAdds = "-a %s" % addFiles
if options.od2trips:
    trips = "trips"
else:
    trips = "successive"
routes = "../input/routes.rou.xml"

if options.stats == 0:
    succDir = makeAndChangeDir("../successive")
    execute("incrementalAssignment.py -d ../input/districts.xml -m %s -n %s -p ../parameter.txt -u ../CRcurve.txt" % (mtxNamesList, netFile))
    if not options.od2trips:
        shutil.copy("%s/routes.txt" % succDir, routes)
        execute("route2trips.py %s > ../input/successive.trips.xml" % routes)
    duaDir = makeAndChangeDir("../dua")
    duaProcess = subprocess.Popen("dua-iterate.py -C -n %s -t ../input/%s.trips.xml %s" % (netFile, trips, pyAdds), shell=True)
    clogDir = makeAndChangeDir("../clogit")
    execute("cLogit.py -d ../input/districts.xml -m %s -n %s -p ../clogit_parameter.txt -u ../CRcurve.txt" % (mtxNamesList, netFile))
    if options.od2trips:
        while not os.path.exists("%s/trips_0.rou.xml" % duaDir):
            time.sleep(1)
        shutil.copy("%s/trips_0.rou.xml" % duaDir, routes)
    shotDir = makeAndChangeDir("../oneshot")
    execute("one-shot.py -n %s -t %s %s" % (netFile, routes, pyAdds))
    duaProcess.wait()
else:
    succDir = "../successive%03i" % options.stats
    duaDir = "../dua%03i" % options.stats
    clogDir = "../clogit%03i" % options.stats
    shotDir = "../oneshot%03i" % options.stats
    
makeAndChangeDir("../routes")
for step in [0, 24, 49]:
    execute("sumo --no-step-log -n %s -e 90000 -r %s/%s_%s.rou.xml --vehroute-output vehroutes_dua_%s.xml %s -l sumo_dua_%s.log" % (netFile, duaDir, trips, step, step, sumoAdds, step))
    execute("networkStatistics.py -n %s -d ../input/districts.xml -x vehroutes_dua_%s.xml -o networkStatistics_%s_%s.txt" % (netFile, step, os.path.basename(duaDir), step))
for step in [-1, 1800, 300, 15]:
    shutil.copy("%s/vehroutes_%s.xml" % (shotDir, step), "vehroutes_oneshot%s.xml" % step)
    execute("networkStatistics.py -n %s -d ../input/districts.xml -x vehroutes_oneshot%s.xml -o networkStatistics_%s_%s.txt" % (netFile, step, os.path.basename(shotDir), step))
execute("sumo --no-step-log -n %s -e 90000 -r %s/routes.txt --vehroute-output vehroutes_successive.xml %s -l sumo_successive.log" % (netFile, succDir, sumoAdds))
execute("networkStatistics.py -n %s -d ../input/districts.xml -x vehroutes_successive.xml -o networkStatistics_%s.txt" % (netFile, os.path.basename(succDir)))
execute("sumo --no-step-log -n %s -e 90000 -r %s/routes.txt --vehroute-output vehroutes_clogit.xml %s -l sumo_clogit.log" % (netFile, clogDir, sumoAdds))
execute("networkStatistics.py -n %s -d ../input/districts.xml -x vehroutes_clogit.xml -o networkStatistics_%s.txt" % (netFile, os.path.basename(clogDir)))
os.chdir("..")

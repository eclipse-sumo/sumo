import glob, os, shutil, subprocess, time
from optparse import OptionParser

def makeAndChangeDir(dirName):
    runID = 1
    fullName = "%s%03i" % (dirName, runID)
    while os.path.exists(fullName):
        runID += 1
        fullName = "%s%03i" % (dirName, runID)
    os.mkdir(fullName)
    os.chdir(fullName)
    return fullName

optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-o", "--use-od2trips", action="store_true", dest="od2trips",
                     default=False, help="use od2trips instead of trips from incremental assignment")
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

succDir = makeAndChangeDir("../successive")
os.system("incrementalAssignment.py -d ../input/districts.xml -m %s -n %s -p ../parameter.txt -u ../CRcurve.txt" % (mtxNamesList, netFile))
if not options.od2trips:
    shutil.copy("%s/routes.txt" % succDir, routes)
    os.system("route2trips.py %s > ../input/successive.trips.xml" % routes)
duaDir = makeAndChangeDir("../dua")
duaProcess = subprocess.Popen("dua-iterate.py -C -n %s -t ../input/%s.trips.xml %s" % (netFile, trips, pyAdds), shell=True)
clogDir = makeAndChangeDir("../clogit")
os.system("cLogit.py -d ../input/districts.xml -m %s -n %s -p ../clogit_parameter.txt -u ../CRcurve.txt" % (mtxNamesList, netFile))
if options.od2trips:
    while not os.path.exists("%s/trips_0.rou.xml" % duaDir):
        time.sleep(1)
    shutil.copy("%s/trips_0.rou.xml" % duaDir, routes)
shotDir = makeAndChangeDir("../oneshot")
os.system("one-shot.py -n %s -t %s %s" % (netFile, routes, pyAdds))

duaProcess.wait()
makeAndChangeDir("../routes")
for step in [0, 24, 49]:
    os.system("sumo --no-step-log -n %s -e 90000 -r %s/%s_%s.rou.xml --vehroute-output vehroutes_dua_%s.xml %s -l sumo_dua_%s.log" % (netFile, duaDir, trips, step, step, sumoAdds, step))
    os.system("networkStatistics.py -n %s -d ../input/districts.xml -x vehroutes_dua_%s.xml -o networkStatistics_%s_%s.txt" % (netFile, step, os.path.basename(duaDir), step))
for step in [-1, 1800, 300, 15]:
    shutil.copy("%s/vehroutes_%s.xml" % (shotDir, step), "vehroutes_oneshot%s.xml" % step)
    os.system("networkStatistics.py -n %s -d ../input/districts.xml -x vehroutes_oneshot%s.xml -o networkStatistics_%s_%s.txt" % (netFile, step, os.path.basename(shotDir), step))
os.system("sumo --no-step-log -n %s -e 90000 -r %s/routes.txt --vehroute-output vehroutes_successive.xml %s -l sumo_successive.log" % (netFile, succDir, sumoAdds))
os.system("networkStatistics.py -n %s -d ../input/districts.xml -x vehroutes_successive.xml -o networkStatistics_%s.txt" % (netFile, os.path.basename(succDir)))
os.system("sumo --no-step-log -n %s -e 90000 -r %s/routes.txt --vehroute-output vehroutes_clogit.xml %s -l sumo_clogit.log" % (netFile, clogDir, sumoAdds))
os.system("networkStatistics.py -n %s -d ../input/districts.xml -x vehroutes_clogit.xml -o networkStatistics_%s.txt" % (netFile, os.path.basename(clogDir)))
os.chdir("..")

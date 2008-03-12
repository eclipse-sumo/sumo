import glob, os, shutil, subprocess, time
def makeAndChangeDir(dirName):
    runID = 1
    fullName = "%s%03i" % (dirName, runID)
    while os.path.exists(fullName):
        runID += 1
        fullName = "%s%03i" % (dirName, runID)
    os.mkdir(fullName)
    os.chdir(fullName)
    return fullName

os.chdir("input")
netFile = "../input/" + glob.glob("*.net.xml")[0]
mtxNamesList = ",".join(["../input/" + item for item in glob.glob("*.fma")])
addFiles = ",".join(["../input/" + item for item in glob.glob("*.add.xml")])
pyAdds = ""
sumoAdds = ""
if addFiles:
    pyAdds = "-+ %s" % addFiles
    sumoAdds = "-a %s" % addFiles

duaDir = makeAndChangeDir("../dua")
duaProcess = subprocess.Popen("dua-iterate.py -C -n %s -l 20 -t ../input/trips.trips.xml %s" % (netFile, pyAdds), shell=True)
succDir = makeAndChangeDir("../successive")
os.system("inc_assignsumo.py -c ../input/districts.xml -m %s -n %s -p ../parameter.txt -u ../CRcurve.txt" % (mtxNamesList, netFile))
while not os.path.exists("%s/trips_0.rou.xml" % duaDir):
    time.sleep(1)
shutil.copy("%s/trips_0.rou.xml" % duaDir, "../input/routes.rou.xml")
shotDir = makeAndChangeDir("../oneshot")
os.system("one-shot.py -n %s -t ../input/routes.rou.xml %s" % (netFile, pyAdds))

duaProcess.wait()
makeAndChangeDir("../routes")
os.system("sumo --no-step-log -n %s -e 90000 -r %s/trips_19.rou.xml --vehroute-output vehroutes_dua.xml %s" % (netFile, duaDir, sumoAdds))
os.system("GlobalMOE.py -n %s -c ../input/districts.xml -x vehroutes_dua.xml -o Global_MOE_%s.txt" % (netFile, os.path.basename(duaDir)))
for step in [-1, 3600, 1800, 900, 300, 150, 90, 60, 30, 15]:
    shutil.copy("%s/vehroutes_%s.xml" % (shotDir, step), "vehroutes_oneshot%s.xml" % step)
    os.system("GlobalMOE.py -n %s -c ../input/districts.xml -x vehroutes_oneshot%s.xml -o Global_MOE_%s_%s.txt" % (netFile, step, os.path.basename(shotDir), step))
os.system("sumo --no-step-log -n %s -e 90000 -r %s/routes.txt --vehroute-output vehroutes_successive.xml %s" % (netFile, succDir, sumoAdds))
os.system("GlobalMOE.py -n %s -c ../input/districts.xml -x vehroutes_successive.xml -o Global_MOE_%s.txt" % (netFile, os.path.basename(succDir)))
os.chdir("..")

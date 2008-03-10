import glob, os, shutil
def makeAndChangeDir(dirName):
    runID = 1
    while os.path.exists("%s%03i"%(dirName,runID)):
        runID += 1
    os.mkdir("%s%03i"%(dirName,runID))
    os.chdir("%s%03i"%(dirName,runID))

mtxNamesList = ",".join(["../" + item for item in glob.glob("input/mtx*.fma")])
addFiles = ",".join(["../" + item for item in glob.glob("input/*.add.xml")])
pyAdds = ""
sumoAdds = ""
if addFiles:
    pyAdds = "-+ %s" % addFiles
    sumoAdds = "-a %s" % addFiles

makeAndChangeDir("dua")
os.system("start dua-iterate.py -n ../input/net.net.xml -l 20 -t ../input/trips.trips.xml %s" %pyAdds)
shutil.copy("trips_0.rou.xml", "../input/routes.rou.xml")
makeAndChangeDir("../successive")
os.system("inc_assignsumo.py -c ../input/districts.xml -m %s -n ../input net.net.xml -p ../parameter.txt -u ../CRcurve.txt" % mtxNamesList)
makeAndChangeDir("../oneshot")
os.system("one-shot.py -n ../input/net.net.xml -t ../input/routes.rou.xml %s" % pyAdds)
makeAndChangeDir("../routes")

os.system("sumo --no-step-log -n ../input/net.net.xml -e 90000 -r ../dua/trips_19.rou.xml --vehroute-output vehroutes_dua.xml %s" % sumoAdds)
for step in [-1, 3600, 1800, 900, 300, 150, 90, 60, 30, 15]:
    shutil.copy("../oneshot/vehroutes_%s.xml" % step, "vehroutes_oneshot%s.rou.xml %s" % (step, sumoAdds))
os.system("sumo --no-step-log -n ../input/net.net.xml -e 90000 -r ../successive/routes.txt --vehroute-output vehroutes_successive.xml %s")
os.chdir("..")

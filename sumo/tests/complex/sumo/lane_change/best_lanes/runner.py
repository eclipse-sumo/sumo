#!/usr/bin/env python
import os,subprocess,sys,shutil
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', "tools"))
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', "tools", "lib"))
import traci, testUtil

sumoBinary = testUtil.checkBinary('sumo')
netconvertBinary = testUtil.checkBinary('netconvert')

DELTA_T = 1

srcRoot = os.path.join(os.path.dirname(sys.argv[0]), "data")
roots = []
for root, dirs, files in os.walk(srcRoot):
    if "input_edges.edg.xml" in files:
        roots.append(root)

for root in sorted(roots):
    print "-- Test: %s" % root[len(srcRoot)+1:]
    prefix = os.path.join(root, "input_")
    sys.stdout.flush()
    subprocess.call([netconvertBinary, "-n", prefix+"nodes.nod.xml", "-e", prefix+"edges.edg.xml",
                     "-x", prefix+"connections.con.xml", "-o", "./input_net.net.xml"], stdout=sys.stdout)
    sys.stdout.flush()
    shutil.copy(prefix + "routes.rou.xml", "./input_routes.rou.xml")
    shutil.copy(prefix + "additional.add.xml", "./input_additional.add.xml")

    sumoProcess = subprocess.Popen("%s -c sumo.sumo.cfg" % (sumoBinary), shell=True, stdout=sys.stdout)
    traci.init(8813)
    step = 0
    traci.simulationStep(DELTA_T)
    step += 1
    lanes = traci.vehicle.getBestLanes("0")
    sys.stdout.flush()
    for l in lanes:
       print "lane %s:" % (l[0])
       print "  length: %s" % (l[1])
       print "  offset: %s" % (l[3])
       print "  allowsContinuation: %s" % (l[4])
       print "  over: %s" % (l[5])
    traci.close()
    sys.stdout.flush()

    fdi = open(root + "/expected.txt")
    for i,l in enumerate(lanes):
        vals = fdi.readline().strip().split()
        length = int(vals[0])
        if ((int(l[1])+500)/500)*500==length:
            print "lane %s ok" % i
        else:
            print "lane %s mismatches" % i
    print "-" * 70
    print ""

#!/usr/bin/env python
import os,subprocess,sys,shutil
sumoHome = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..'))
sys.path.append(os.path.join(sumoHome, "tools", "traci"))
import traciControl

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', 'bin', 'sumo'))
netconvertBinary = os.environ.get("NETCONVERT_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', 'bin', 'netconvert'))

DELTA_T = 1

srcRoot = os.path.join("./data/")
for root, dirs, files in os.walk(srcRoot):
    if "input_edges.edg.xml" not in files:
        continue

    print "-- Test: %s" % root
    sys.stdout.flush()
    netconvertCall = [ "-n", root+"/input_nodes.nod.xml", "-e", root+"/input_edges.edg.xml", "-x", root+"/input_connections.con.xml", "-o", "./input_net.net.xml" ]
    subprocess.call([netconvertBinary]+netconvertCall, shell=(os.name=="nt"), stdout=sys.stdout, stderr=sys.stderr)
    sys.stdout.flush()
    shutil.copy(root + "/input_routes.rou.xml", "./input_routes.rou.xml")
    shutil.copy(root + "/input_additional.add.xml", "./input_additional.add.xml")

    sumoProcess = subprocess.Popen("%s -c sumo.sumo.cfg" % (sumoBinary), shell=True, stdout=sys.stdout)
    traciControl.initTraCI(8813)
    step = 0
    traciControl.cmdSimulationStep(DELTA_T)
    step += 1
    lanes = traciControl.cmdGetVehicleVariable_bestLanes("0")
    sys.stdout.flush()
    for l in lanes:
       print "lane %s:" % (l[0])
       print "  length: %s" % (l[1])
       print "  offset: %s" % (l[3])
       print "  allowsContinuation: %s" % (l[4])
       print "  over: %s" % (l[5])
    traciControl.cmdClose()
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




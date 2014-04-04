#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys,os,subprocess
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', "tools"))
from sumolib import checkBinary

EDC = checkBinary("emissionsDrivingCycle")
PHEMLIGHTp = os.path.join(os.environ["SUMO_HOME"], "data", "emissions", "PHEMlightV3")
if not os.path.exists(PHEMLIGHTp):
    PHEMLIGHTp = os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', "data", "emissions", "PHEMlight")

fd = open("classes.txt")
emissionClasses = fd.readlines()
fd.close() 

fdo = open("results.csv", "w")
for i,ec in enumerate(emissionClasses):
    ec = ec.strip()
    if len(ec) == 0:
      continue
    drivingCycle = sys.argv[1]
    print "Running '%s'" % ec
    sys.stdout.flush()
    sys.stderr.flush()
    call = [EDC, "-e", ec, "-t", drivingCycle, "-o", "tmp.csv", "-p", PHEMLIGHTp, "--kmh", "--compute-a"]
    retCode = subprocess.call(call)
    sys.stdout.flush()
    sys.stderr.flush()
    if retCode != 0:
        print "Error on building PHEMlight measurements"
        sys.exit(1)
    fd = open("tmp.csv")
    out = fd.readlines()
    fd.close()
    fdo.write("%s\n" % ec)
    for l in out:
      fdo.write(l)
    fdo.write("-----\n\n")
fdo.close()


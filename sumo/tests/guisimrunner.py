#!/usr/bin/env python
import os, subprocess, sys, shutil

sumoBinary = os.path.join(os.path.dirname(sys.argv[1]), "sumo")
sumoBinary = os.environ.get("SUMO_BINARY", sumoBinary)
guisimBinary = sys.argv[1]

#start sumo to generate config
subprocess.call(sumoBinary + " --message-log guisim.stdout --error-log guisim.stderr --save-configuration guisim.sumo.cfg " + " ".join(sys.argv[2:]), 
                shell=True, stdout=sys.stdout, stderr=sys.stderr)
ret = subprocess.call(guisimBinary + " -Q -c guisim.sumo.cfg", 
                      shell=True, stdout=sys.stdout, stderr=sys.stderr)
if os.path.exists("guisim.stderr"):
    f = open("guisim.stderr")
    shutil.copyfileobj(f, sys.stderr)
    f.close()
sys.exit(ret)

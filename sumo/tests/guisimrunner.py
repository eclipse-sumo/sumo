#!/usr/bin/env python
import os, subprocess, sys, shutil

sumoDir = os.path.join(os.path.dirname(sys.argv[0]), "..")
sumoBinary = os.path.join(sumoDir, "bin", "sumo.exe")
guisimBinary = os.path.join(sumoDir, "bin", "guisim.exe")
if os.name == 'posix':
    sumoBinary = os.path.join(sumoDir, "src", "sumo")
    guisimBinary = os.path.join(sumoDir, "src", "sumo-guisim")

#start sumo to generate config
subprocess.call(sumoBinary + " --error-log guisim.stderr --save-configuration guisim.cfg " + " ".join(sys.argv[1:]), 
                shell=True, stdout=sys.stdout, stderr=sys.stderr)
subprocess.call(guisimBinary + " -Q -c guisim.cfg", 
                shell=True, stdout=sys.stdout, stderr=sys.stderr)
#shutil.copyfileobj(open("guisim.stdout"), sys.stdout)
shutil.copyfileobj(open("guisim.stderr"), sys.stderr)

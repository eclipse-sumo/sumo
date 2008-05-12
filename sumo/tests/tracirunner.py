#!/usr/bin/env python
import os,subprocess,sys

#start sumo as server
serverprocess = subprocess.Popen(os.path.join(os.path.dirname(sys.argv[0]), "..", "bin", " ".join(sys.argv[1:12])),
                shell=True, stdout=sys.stdout, stderr=sys.stderr)
                
#start a traci client which connects to the server
subprocess.call(os.path.join(os.path.dirname(sys.argv[0]), "..", "bin", " ".join(sys.argv[12:])),
                shell=True, stdout=sys.stdout, stderr=sys.stderr)
                
#wait for the server to finish
serverprocess.wait()

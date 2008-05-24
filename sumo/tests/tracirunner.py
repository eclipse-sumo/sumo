#!/usr/bin/env python
import os,subprocess,sys

numParams = 1
serverParams = []
clientParams = []

for i in range(1, len(sys.argv)):
    if sys.argv[i] == "endsumo":
        numParams = numParams+1
        break
    if sys.argv[i] == "TraCITestClient.exe": 
        break
    else:
        numParams = numParams+1
        serverParams = serverParams[0:len(serverParams)] + [sys.argv[i]]
        
clientParams = sys.argv[numParams:len(sys.argv)]

#start sumo as server    
serverprocess = subprocess.Popen(os.path.join(os.path.dirname(sys.argv[0]), "..", "bin", " ".join(serverParams)), 
                shell=True, stdout=sys.stdout, stderr=sys.stderr)       
#serverprocess = subprocess.Popen(os.path.join(os.path.dirname(sys.argv[0]), "..", "bin", " ".join(sys.argv[1:12])),
#                shell=True, stdout=sys.stdout, stderr=sys.stderr)
    
subprocess.call(os.path.join(os.path.dirname(sys.argv[0]), "..", "bin", " ".join(clientParams)),
                shell=True, stdout=sys.stdout, stderr=sys.stderr)
#subprocess.call(os.path.join(os.path.dirname(sys.argv[0]), "..", "bin", " ".join(sys.argv[12:])),
#                shell=True, stdout=sys.stdout, stderr=sys.stderr)
                
#wait for the server to finish
serverprocess.wait()

#!/usr/bin/env python
import os,subprocess,sys,time

numParams = 1
serverParams = []
clientParams = []

for arg in sys.argv[1:]:
    if arg == "endsumo":
        numParams += 1
        break
    if arg == "TraCITestClient.exe": 
        break
    else:
        numParams += 1
        serverParams += [arg]
clientParams = sys.argv[numParams:]

sumoDir = os.path.join(os.path.dirname(sys.argv[0]), '..', 'bin')
if os.name == 'posix':
    sumoDir = sumoDir[:-3] + 'src'
    serverParams[0] = 'sumo'
    clientParams[0] = 'traci-testclient'
if "SUMO_BINARY" in os.environ:
    sumoDir, serverParams[0] = os.path.split(os.environ["SUMO_BINARY"])

#start sumo as server    
serverprocess = subprocess.Popen(os.path.join(sumoDir, " ".join(serverParams)), 
                                 shell=True, stdout=sys.stdout, stderr=sys.stderr)       
for retry in range(10):
    returnCode = subprocess.call(os.path.join(sumoDir, " ".join(clientParams)),
                                 shell=True, stdout=sys.stdout, stderr=sys.stderr)
    if returnCode == 0:
        break
    time.sleep(1)
                
#wait for the server to finish
serverprocess.wait()

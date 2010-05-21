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

sumoDir, serverParams[0] = os.path.split(sys.argv[1])
if os.name == 'posix':
    clientParams[0] = 'TraCITestClient'

#start sumo as server    
serverprocess = subprocess.Popen(os.path.join(sumoDir, " ".join(serverParams)), 
                                 shell=True, stdout=sys.stdout, stderr=sys.stderr)
for retry in range(10):
    clientProcess = subprocess.Popen(os.path.join(sumoDir, " ".join(clientParams)),
                                     shell=True, stdout=sys.stdout, stderr=sys.stderr)
    if serverprocess.poll() != None and clientProcess.poll() == None:
        time.sleep(10)
        if serverprocess.poll() != None and clientProcess.poll() == None:
            print >> sys.stderr, "Client hangs but server terminated for unknown reason"
            clientProcess.kill()
            break
    if clientProcess.wait() == 0:
        break
    time.sleep(1)

#wait for the server to finish
serverprocess.wait()

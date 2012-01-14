#!/usr/bin/env python

import os,subprocess,sys,time,threading,socket
sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', '..', '..', "tools"))
import sumolib

PORT = 8088
sumoBinary = sumolib.checkBinary('sumo')

def main():
    #create an INET, STREAMing socket
    serversocket = socket.socket(
        socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", PORT))
    serversocket.listen(5)
    (clientsocket, address) = serversocket.accept()
    while True:
        #accept connections from outside
        #now do something with the clientsocket
        msg = ''
        while len(msg) < 100:
            chunk = clientsocket.recv(100-len(msg))
            if chunk == '':
                sys.stdout.write(msg)
                return
            msg = msg + chunk
        sys.stdout.write(msg)

threading.Thread(target=main).start()
time.sleep(1)
subprocess.call([sumoBinary, "sumo.sumocfg"])

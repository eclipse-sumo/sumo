import subprocess, sys, socket, time, struct, random
from traciControl import initTraCI, simStep, close, getVehNoOfIndLoop, setPhase

PORT = 8813

NSGREEN = ["0101", "0101", "0000"]
NSYELLOW = ["0000", "0000", "0101"]
WEGREEN = ["1010", "1010", "0000"]
WEYELLOW = ["0000", "0000", "1010"]

PROGRAM = [WEYELLOW,WEYELLOW,WEYELLOW,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSGREEN,NSYELLOW,NSYELLOW,WEGREEN]

N = 9000


pWE = 1./10
pEW = 1./11
pNS = 1./30

routes = open("cross.rou.xml", "w")
print >> routes, '<routes>'
print >> routes, '  <vtype id="typeWE" accel="0.8" decel="4.5" sigma="0.5" length="7.5" maxspeed="16.67"/>'
print >> routes, '  <vtype id="typeNS" accel="0.8" decel="4.5" sigma="0.5" length="20" maxspeed="25"/>'
print >> routes, ""
print >> routes, '  <route id="right" edges="51o 1i 2o 52i" />'
print >> routes, '  <route id="left" edges="52o 2i 1o 51i" />'
print >> routes, '  <route id="down" edges="54o 4i 3o 53i" />'
print >> routes, ""
lastVeh = 0
vehNr = 0
for i in range(N):
    if random.uniform(0,1) < pWE:
        print >> routes, '  <vehicle id="%i" type="typeWE" route="right" depart="%i" />' % (vehNr, i)
        vehNr += 1
        lastVeh = i
    if random.uniform(0,1) < pEW:
        print >> routes, '  <vehicle id="%i" type="typeWE" route="left" depart="%i" />' % (vehNr, i)
        vehNr += 1
        lastVeh = i
    if random.uniform(0,1) < pNS:
        print >> routes, '  <vehicle id="%i" type="typeNS" route="down" depart="%i" color="1,0,0"/>' % (vehNr, i)
        vehNr += 1
        lastVeh = i

print >> routes, "</routes>"
routes.close()
    


sumoExe = "guisim"
sumoConfig = "cross.sumo.cfg"
sumoProcess = subprocess.Popen("%s -c %s" % (sumoExe, sumoConfig), shell=True, stdout=sys.stdout)


initTraCI(PORT)

programPointer = len(PROGRAM)-1
veh = []
step = 0
while not (step > lastVeh and veh == []):
    veh = simStep(1)
    programPointer = min(programPointer+1, len(PROGRAM)-1)
    no = getVehNoOfIndLoop("0")
    if no > 0:
        programPointer = (0 if programPointer == len(PROGRAM)-1 else 3)
    setPhase("0", PROGRAM[programPointer])
    step += 1
    
close()
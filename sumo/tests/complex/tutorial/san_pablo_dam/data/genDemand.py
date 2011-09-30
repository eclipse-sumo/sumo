import math
import random
import sys, os
import numpy as np

class Ddict(dict):
    def __init__(self, default=None):
        self.default = default

    def __getitem__(self, key):
        if not self.has_key(key):
            self[key] = self.default()
        return dict.__getitem__(self, key)

if len(sys.argv)<2:
	dataPath = "./"
	outputFile = "./spd-road.rou.xml"
else:
	dataPath = sys.argv[1]
	outputFile = sys.argv[2]
	

# read data of observer #1 from file
fp = open(os.path.join(dataPath, 'obstimes_1_1.txt'),'r')
obs1 = fp.readlines()
fp.close()
N = len(obs1)
t1Times = []

for i in range(N):
	if len(obs1[i])>2:
		ll = obs1[i].split(':')
		hh = int(ll[0])
		mm = int(ll[1])
		ss = float(ll[2])
		t1Times.append(int(3600.0*hh+60.0*mm+ss))

fp = open(os.path.join(dataPath, 'sumo-parameters.txt'),'r')
params = fp.readlines()
fp.close()

dPara = {'dummy': 0.0}
for i in range(0,len(params)):
	ll = params[i].split(' ')
	dPara[ll[0]] = float(ll[1])

#for k,v in dPara.iteritems():
#	print k,v

fRou = open(outputFile, 'w')
fRou.write('<routes>\n')
# one type of vehicles

# passenger cars
s0 = '\t<vType accel="' + repr(dPara['aMax']) + '" decel="' + repr(dPara['bMax']) + '" id="pass"'
s0 = s0 +' length="' + repr(dPara['lCar']) + '" minGap="0" maxSpeed="' + repr(dPara['vMax'])
print >> fRou, s0 + '" sigma="' + repr(dPara['sigA']) + '" tau="' + repr(dPara['tTau']) + '" />'

# the one and only route...
fRou.write('\t<route id="route01" edges="1to7 7to8"/>\n')

# now, the vehicles...
vehID = 0
for t in range(len(t1Times)): 
	vehID += 1
	s = '\t<vehicle depart="' + repr(t1Times[t]) + '" arrivalPos="-1" id="' + repr(vehID) +'" route="route01"'
	s = s + ' type="pass" departSpeed="max" />'
	print >> fRou, s
print >> fRou, '</routes>'
fRou.close()

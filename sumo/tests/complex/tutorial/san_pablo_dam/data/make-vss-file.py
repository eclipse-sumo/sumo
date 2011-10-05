import math
import random
import sys, os
import numpy as np

fp = open('obstimes_1_7.txt','r')
obs7 = fp.readlines()
fp.close()
N7 = len(obs7)
fp = open('obstimes_1_8.txt','r')
obs8 = fp.readlines()
fp.close()
N8 = len(obs8)

t7Times = np.zeros( N7, dtype=float)
t8Times = np.zeros( N8, dtype=float)

for i in range(0,N7-1):
	ll = obs7[i].split(':')
	hh = int(ll[0])
	mm = int(ll[1])
	ss = float(ll[2])
	t7Times[i] = int(3600.0*hh+60.0*mm+ss)
	

for i in range(0,N8-1):
	ll = obs8[i].split(':')
	hh = int(ll[0])
	mm = int(ll[1])
	ss = float(ll[2])
	t8Times[i] = int(3600.0*hh+60.0*mm+ss)
	
print 'data read: ', N7, N8

fp = open('spd-road.vss.xml','w')
lObs8 = 337.5
print >> fp, '<vss>'
for i in range(0,N7-1):
	v = lObs8/(t8Times[i] - t7Times[i])
	t = math.trunc(t7Times[i])
	if i!=N7-1 and t!=math.trunc(t7Times[i+1]):
		print >> fp, '\t<step time ="' + repr(t) + '" speed="' + repr(v) + '"/>'
print >> fp, '</vss>'
fp.close()


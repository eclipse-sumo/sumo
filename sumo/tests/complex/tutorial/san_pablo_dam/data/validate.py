import sys, os, math, csv, subprocess

dDay = 1

# generating route-file with the car-following parameters in sumo-parameters.txt
exec(compile(open('genDemand.py').read(), 'genDemand.py', 'exec')) 
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', '..', "tools", "lib"))
from sumolib import checkBinary

# running the simulation...
sumoBinary = checkBinary('sumo')
retcode = subprocess.call([sumoBinary, "-c", "spd-road.sumocfg"], stdout=sys.stdout, stderr=sys.stderr)
sys.stdout.flush()
sys.stderr.flush()

# analyzing the results...
# read the empirical times
obsTimes = {}
simTimes = {}
for i in range (0,9): 
   obsTimes[i] = {}
   simTimes[i] = {}
#   for j in range (0, lineValue): the number of lines in the given file
#       obsTimes[i][j] = 0.

for i in range(1,8):
	if dDay==1 and i==5:
		continue
	if dDay==2 and i==6:
		continue
	fname = 'obstimes_' + repr(dDay) + '_' + repr(i) + '.txt'
	fp = open(fname,'r')
	obs = fp.readlines()
	fp.close()
	N = len(obs)
	for j in range(N):
		ll = obs[j].split(':')
		hh = int(ll[0])
		mm = int(ll[1])
		ss = float(ll[2])
		obsTimes[i][j] = int(3600.0*hh+60.0*mm+ss)

# read the simulated times
f = open('detector.xml','r')
data = f.readlines()
f.close()

obs2Nr = {'obs1': 1, 'obs2': 2, 'obs3': 3, 'obs4': 4, 'obs5': 5, 'obs6': 6, 'obs7': 7}  
cnt = [0,0,0,0,0,0,0,0,0]

for i in range(1,len(data)):
	if data[i].find('<interval')!=-1:
		ll = data[i].split('"')
		nn = int(ll[7])
		tIn = float(ll[1])
		iObs = obs2Nr[ll[5]]
		if nn>0:
			simTimes[iObs][cnt[iObs]] = tIn
			cnt[iObs] += 1

# convert obsTimes[][] into travel-times:
cntObs = [0,0,0,0,0,0,0,0,0]

for i in range(1,8):
	ni = len(obsTimes[i])
	nip = len(obsTimes[i+1])
	cntObs[i] = ni
	if ni==nip and ni > 100: 
		for j in range(ni):
			obsTimes[i][j] = obsTimes[i+1][j] - obsTimes[i][j]

# convert simTimes[][] into travel-times:
for i in range(1,8):
	ni = len(simTimes[i])
	nip = len(simTimes[i+1])
	if ni==nip and ni>100:
		for j in range(ni):
			simTimes[i][j] = simTimes[i+1][j] - simTimes[i][j]

# compute final statistics
err = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
averTT = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
errAll = 0.0
cntAll = 0
f = open('sumo-obs-error.txt','w')
for i in range(1,7):
	if cntObs[i]<=100 or cntObs[i+1]<=100:
		continue
	if len(obsTimes[i]) == len(simTimes[i]):
		tmp = 0.0
		for j in range(cntObs[i]):
			d = obsTimes[i][j] - simTimes[i][j]
			tmp += d*d
		err[i] = math.sqrt(tmp/cntObs[i])
		print >> f, "%s %s" %(i, err[i])
		errAll += err[i]
		cntAll += 1

f.close()
# save the single final result in a file, so that the calibrate.py script
# can read it.
f = open('sumo-rmse.txt','w')
print >> f, "%s" %(errAll/cntAll)
f.close()

# finally, write the individual travel times into a csv-file
# this is not really needed when validate is called from calibrate as an intermediate
# step, but it makes analyzing of the result more simple.
# first the header
c = open('compare-tt.csv', 'w')
c.write('# indx;')
for i in range(1,7):
        if cntObs[i]>100 and cntObs[i+1]>100:
                c.write('obs'+repr(i)+';sim'+repr(i)+';')
c.write('\n')

# then the data, of course on the ones which are useable
for line in range(len(simTimes[1])):
        c.write(repr(line)+';')
        for i in range(1,7):
                if cntObs[i]>100 and cntObs[i+1]>100:
                        ttObs = int(obsTimes[i][line])
                        ttSim = int(simTimes[i][line])
                        c.write(repr(ttObs)+';'+repr(ttSim)+';')
        c.write('\n')
c.close()

#!/usr/bin/env python
# -*- coding: utf-8 -*-

# call jtrrouter twice and check that the output differs
import sys,os,subprocess,random,shutil
sys.path.append(os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', "tools"))
from sumolib import checkBinary

flow1def = "0;2000;200".split(";")
flow2def = "0;2000;200".split(";")
fillSteps = 3600#3600
measureSteps = 10000#36000 
simSteps = fillSteps + measureSteps 

try: os.mkdir("results")
except: pass
try: os.mkdir("gfx")
except: pass

def buildDemand(simSteps, pWE, pEW, pNS, pSN):
    fd = open("input_routes.rou.xml", "w")
    #---routes---
    print >> fd, """<routes>
		
			<vType id="type1" accel="2.0" decel="5.0" sigma="0.0" length="6.5" maxSpeed="70"/>
		
			<route id="WE" edges="1i 3o 5o"/>
			<route id="NS" edges="2i 4o 6o"/>
			<route id="EW" edges="3i 1o 7o"/>
			<route id="SN" edges="4i 2o 8o"/>
			
		"""
    lastVeh = 0
    vehNr = 0
    for i in range(simSteps):
        if random.uniform(0,1) < pWE: # Poisson distribution
            print >> fd, '    <vehicle id="%i" type="type1" route="WE" depart="%i" departSpeed="13.89" />' % (vehNr, i)
            vehNr += 1
            lastVeh = i
        if random.uniform(0,1) < pNS:
            print >> fd, '    <vehicle id="%i" type="type1" route="NS" depart="%i" departSpeed="13.89" />' % (vehNr, i)
            vehNr += 1
            lastVeh = i
        if random.uniform(0,1) < pEW:
            print >> fd, '    <vehicle id="%i" type="type1" route="EW" depart="%i" departSpeed="13.89" />' % (vehNr, i)
            vehNr += 1
            lastVeh = i	
        if random.uniform(0,1) < pSN:
            print >> fd, '    <vehicle id="%i" type="type1" route="SN" depart="%i" departSpeed="13.89" />' % (vehNr, i)
            vehNr += 1
            lastVeh = i
    print >> fd, "</routes>"
    fd.close()  
  
def patchTLSType(ifile, itype, ofile, otype):
    fdi = open(ifile)
    fdo = open(ofile, "w")
    for line in fdi:
        line = line.replace(itype, otype)
        fdo.write(line)
    fdo.close()
    fdi.close()
    
      
sumo = ".\\sumo"#checkBinary('sumo')
assert(sumo)


for f1 in range(int(flow1def[0]), int(flow1def[1]), int(flow1def[2])):
    pWE = float(f1)/3600 # [veh/s]
    pEW = pWE
    for f2 in range(int(flow2def[0]), int(flow2def[1]), int(flow2def[2])):
        pNS = float(f2)/3600 # [veh/s]
        pSN = pNS
        print "Computing for %s<->%s" % (f1, f2)
        buildDemand(simSteps, pWE, pEW, pNS, pSN)
        for t in ["static", "agentbased", "actuated", "sotl_phase", "sotl_platoon", "sotl_request", "sotl_wave", "sotl_marching", "swarm"]:
            print " for tls-type %s" % t
            patchTLSType('input_additional_template.add.xml', '%tls_type%', 'input_additional.add.xml', t)
            args = [sumo,
              '--net-file', 'input_net.net.xml',
              '--route-files', 'input_routes.rou.xml',
              '--additional-files', 'input_additional.add.xml',
              '--tripinfo-output', 'results/tripinfos_%s_%s_%s.xml' % (t, f1, f2),
              '--summary-output', 'results/summary_%s_%s_%s.xml' % (t, f1, f2),
              '--device.emissions.probability', '1',
              '--queue-output', 'results/queue_%s_%s_%s.xml' % (t, f1, f2),
            ]
            retCode = subprocess.call(args)
            shutil.move("results/e2_output.xml", "results/e2_output_%s_%s_%s.xml" % (t, f1, f2))
            shutil.move("results/e2_tl0_output.xml", "results/e2_tl0_output_%s_%s_%s.xml" % (t, f1, f2))
            shutil.move("results/edgeData_3600.xml", "results/edgeData_3600_%s_%s_%s.xml" % (t, f1, f2))
            shutil.move("results/laneData_3600.xml", "results/laneData_3600_%s_%s_%s.xml" % (t, f1, f2))
            shutil.move("results/edgesEmissions_3600.xml", "results/edgesEmissions_3600_%s_%s_%s.xml" % (t, f1, f2))
            shutil.move("results/lanesEmissions_3600.xml", "results/lanesEmissions_3600_%s_%s_%s.xml" % (t, f1, f2))
            shutil.move("results/TLSStates.xml", "results/TLSStates_%s_%s_%s.xml" % (t, f1, f2))
            shutil.move("results/TLSSwitchTimes.xml", "results/TLSSwitchTimes_%s_%s_%s.xml" % (t, f1, f2))
            shutil.move("results/TLSSwitchStates.xml", "results/TLSSwitchStates_%s_%s_%s.xml" % (t, f1, f2))
         
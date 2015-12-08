#!/usr/bin/env python

import os,subprocess,sys,time,shutil
import sumodump, sumoInductLoop, sumotripinfos, sumodetectors



def call(command, log):
    print >> log, "-" * 79
    print >> log, command
    log.flush()
    print command
    retCode = subprocess.call(command, stdout=sys.stdout, stderr=sys.stderr)
    if retCode != 0:
        print >> sys.stderr, "Execution of %s failed. Look into %s for details." % (command, log.name)
        sys.exit(retCode) 


def iterateGreenTimes(emissionClass, outputFile, T, MIN_GREEN, log):
    fd = open("input_routes.rou.xml", "w")
    print >> fd, '<routes>'
    print >> fd, '    <!-- vType id="t1" accel="0.8" decel="4.5" sigma="0.5" length="7.5" maxSpeed="36"/ -->	'
    print >> fd, '    <route id="r1" multi_ref="x" edges="e2 e w w2"/>'
    for i in range(0, 1000):
        print >> fd, '    <vehicle id="v.%s" depart="%s" departspeed="13.89" departpos="0" route="r1" type="t1"/>' % (i, i*5)
    print >> fd, '</routes>'
    fd.close()

    fdo = open(outputFile, "w")
    for i in range(MIN_GREEN, T-MIN_GREEN):
        print ">>> Building plan (at t=%s)" % (i)
        fd = open("input_additional.add.xml", "w")
        print >> fd, '<additional>'
        print >> fd, '   <vType id="t1" accel="0.8" decel="4.5" sigma="0.5" length="7.5" maxSpeed="36" aemissionClass="%s"/>' % emissionClass
        print >> fd, '   <tlLogic id="c" type="static" programID="my" offset="0.00">'
        print >> fd, '      <phase duration="%s" state="GGrr"/>' % (i)
        print >> fd, '      <phase duration="%s" state="rrGG"/>' % (T-i)
        print >> fd, '   </tlLogic>'
        print >> fd, '   <laneData id="traffic" freq="100000" file="aggregated_traffic.xml"/>'
        print >> fd, '   <laneData id="noise" freq="100000" type="harmonoise" file="aggregated_noise.xml"/>'
        print >> fd, '   <laneData id="emissions" freq="100000" type="hbefa" file="aggregated_emissions.xml"/>'
        print >> fd, '   <e2Detector id="e2" lane="e_0" pos=".1" length="9999.8" freq="100000" file="e2.xml" friendlyPos="x"/>'    
        print >> fd, '   <e2Detector id="e2_tls" tl="c" to="w_0" lane="e_0" pos=".1" length="9999.8" freq="100000" file="e2_tls.xml" friendlyPos="x"/>'
        print >> fd, '</additional>'
        fd.close()

        print ">>> Simulating (at t=%s)" % (i)
        call(sumoBinary + " -c sumo.sumo.cfg -v", log)

#    shutil.copy("aggregated_traffic.xml", "backup/" + str(i) + "_aggregated_traffic.xml")
#    shutil.copy("aggregated_noise.xml", "backup/" + str(i) + "_aggregated_noise.xml")
#    shutil.copy("aggregated_emissions.xml", "backup/" + str(i) + "_aggregated_emissions.xml")
#    shutil.copy("tripinfos.xml", "backup/" + str(i) + "_tripinfos.xml")
#    shutil.copy("e2.xml", "backup/" + str(i) + "_e2.xml")
#    shutil.copy("e2_tls.xml", "backup/" + str(i) + "_e2_tls.xml")
#    shutil.copy("netstate.xml", "backup/" + str(i) + "_netstate.xml")
    
        dumpEmissions = sumodump.readDump("aggregated_emissions.xml", ["CO_perVeh", "CO2_perVeh", "HC_perVeh", "PMx_perVeh", "NOx_perVeh", "fuel_perVeh"])
        dumpNoise = sumodump.readDump("aggregated_noise.xml", ["noise"])
        dumpTraffic = sumodump.readDump("aggregated_traffic.xml", ["waitingTime", "speed"])
        tripinfos = sumotripinfos.readTripinfos("tripinfos.xml", ["waitSteps"])
        e2 = sumodetectors.readAreal("e2.xml", ["maxJamLengthInVehicles", "jamLengthInVehiclesSum", "meanHaltingDuration", "startedHalts"])
    
        vals = tripinfos.get("waitSteps")
        meanV = 0
        for v in vals:
            meanV += vals[v]
        meanV = meanV / len(vals)
    
        wtv = dumpTraffic.get("waitingTime")[-1]["e_0"]
        nv = dumpNoise.get("noise")[-1]["e_0"]
        cov = dumpEmissions.get("CO_perVeh")[-1]["e_0"] / 1000.
        co2v = dumpEmissions.get("CO2_perVeh")[-1]["e_0"] / 1000.
        hcv = dumpEmissions.get("HC_perVeh")[-1]["e_0"] / 1000.
        pmv = dumpEmissions.get("PMx_perVeh")[-1]["e_0"] / 1000.
        nov = dumpEmissions.get("NOx_perVeh")[-1]["e_0"] / 1000.
        fv = dumpEmissions.get("fuel_perVeh")[-1]["e_0"] / 1000.
        msv = dumpTraffic.get("speed")[-1]["e_0"]
    
        mjlv = e2.get("maxJamLengthInVehicles")[-1]["e2"]
        jlsv = e2.get("jamLengthInVehiclesSum")[-1]["e2"]
        mhdv = e2.get("meanHaltingDuration")[-1]["e2"]
        shv = e2.get("startedHalts")[-1]["e2"]
    
        print >> fdo, "%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s" % (T-i, wtv, nv, cov, co2v, hcv, pmv, nov, fv, meanV, msv, 10000./msv-10000./13.82, mjlv, jlsv, mhdv, shv)
    fdo.close()


MIN_GREEN = 10
T = 180
log = open("logfile.txt", "w")

netconvertBinary = ".\\netconvert.exe"#os.environ.get("NETCONVERT_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', 'bin', 'netconvert'))
call(netconvertBinary + " -c netconvert.netc.cfg -v", log)
sumoBinary = ".\\sumo.exe"

emissionClasses = []
fd = open("all.txt")
for line in fd.readlines():
  line = line.strip()
  if len(line)<1:
    continue
  emissionClasses.append(line)

for e in emissionClasses:
  of = e + "_results.csv"
  iterateGreenTimes(e, of, T, MIN_GREEN, log)


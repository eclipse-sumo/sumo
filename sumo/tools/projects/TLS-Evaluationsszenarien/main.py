#!/usr/bin/env python

import subprocess, sys, os, re, shutil
from genRoutes import *
from evaluator import *
from changeNet import *
from ConfigParser import *
from plotter import *


def readIni(nb):
    global K, N, cut, gui, distrWE, distrNS, vehphWEA, vehphNSA, maxSumFlow, tlType, intergreenLength, GSum, phaseMinWE, phaseMaxWE, phaseMinNS, phaseMaxNS, maxGap, detPos
    filename = 'input' + str(nb).zfill(2) + '.ini'
    ini = ConfigParser()
    ini.read(filename)
    
    K = ini.getint("general", "K")
    N = ini.getint("general", "N")

    cut = ini.getboolean("general", "cut")
        
    gui = ini.getboolean("general", "gui")
    
    
    distrWE = ini.get("demand", "distrWE")
    distrNS = ini.get("demand", "distrNS")
    
    vehphWEA = eval(ini.get("demand", "vehphWEA"))
    vehphNSA = eval(ini.get("demand", "vehphNSA"))
    
    maxSumFlow = ini.getint("demand", "maxSumFlow")
    
    tlType = ini.get("TL", "tlType")
    
    intergreenLength = ini.getint("TL", "intergreenLength")
    GSum = ini.getfloat("TL", "GSum")
    
    [phaseMinWE, phaseMaxWE] = eval(ini.get("TL", "phaseMinMaxWE"))
    [phaseMinNS, phaseMaxNS] = eval(ini.get("TL", "phaseMinMaxNS"))
    maxGap = ini.getfloat("TL", "maxGap")
    detPos = ini.getfloat("TL", "detPos")

    return filename

for ini in range(1,5):    
    iniFilename = readIni(ini)
    
    
    data = os.listdir('data')
    A = filter(lambda d: re.match("[0-9]*\.dat",d),data)
    newNr = str(max([int(a[0:-4]) for a in A])+1).zfill(3)
    name = "data/%s" % newNr
    
    shutil.copyfile(iniFilename, name+".ini")
    
    avgDelayFile = open(name + ".dat", "w")

    setTLType(os.path.join('sumoConfig','one_intersection','cross.net.xml'),tlType)
    
    if (tlType == 'static'):
        satFlow = getSaturationFlow()
        
    if (tlType == 'actuated'):
        setPhaseMinMax(os.path.join('sumoConfig','one_intersection','cross.net.xml'),phaseMinWE, phaseMaxWE, phaseMinNS, phaseMaxNS)
    
    for i in range(K):    
        for vehphWE in vehphWEA:
            for vehphNS in vehphNSA:
                
                if (cut & (vehphWE!=vehphNS)) | ((vehphWE + vehphNS) > maxSumFlow):
                    continue
                
                if (tlType == 'static'):
                    satFlowRateWE = vehphWE / satFlow
                    satFlowRateNS = vehphNS / satFlow
                    
                    if (satFlowRateWE + satFlowRateNS) < 1:
                        [greenWE, greenNS] = getOptGreenTimes(satFlowRateWE, satFlowRateNS, intergreenLength, intergreenLength)
                        if (greenWE + greenNS > GSum):
                            greenWE = GSum / ((vehphNS * 1.0 / vehphWE) + 1)
                            greenNS = GSum / ((vehphWE * 1.0 / vehphNS) + 1)
                    else:
                        greenWE = GSum / ((vehphNS * 1.0 / vehphWE) + 1)
                        greenNS = GSum / ((vehphWE * 1.0 / vehphNS) + 1)
                
                    print greenWE
                    print greenNS
                
                    changePhaseDurations(os.path.join('sumoConfig','one_intersection','cross.net.xml'),int(round(greenWE)), intergreenLength, int(round(greenNS)), intergreenLength)
                
                genRoutes(N, distrWE, vehphWE, distrWE, 0, distrNS, vehphNS, distrNS, 0)
            
                if gui:
                    sumoExe = "guisim"
                    sumoConfig = os.path.join('sumoConfig','one_intersection','cross.sumocfg')
                    sumoProcess = subprocess.Popen("%s -c %s" % (sumoExe, sumoConfig), shell=True, stdout=sys.stdout)
                else:
                    sumoExe = "sumo"
                    sumoConfig = os.path.join('sumoConfig','one_intersection','cross.sumocfg')
                    sumoProcess = subprocess.Popen("%s -c %s --no-duration-log --no-step-log --time-to-teleport 10000000 --actuated-tl.max-gap %f --actuated-tl.detector-pos %f" % (sumoExe, sumoConfig, maxGap, detPos), shell=True, stdout=sys.stdout)
                
               
                sumoProcess.wait()
                
                [flowWE, flowNS, qWE, qNS] = evalTrips(N * 0.2, N)
                if (tlType == 'static'):
                    [greenWE, greenNS] = [int(round(greenWE)), int(round(greenNS))]
                else:
                    [greenWE, greenNS] = getAvgGreenTime(intergreenLength,intergreenLength)
         
                print vehphWE, " <-> ", flowWE, " (q = ", qWE, ")"
                print vehphNS, " <-> ", flowNS, " (q = ", qNS, ")"
                
                
                print >> avgDelayFile, flowWE, flowNS, qWE, qNS, getAvgDelayWE(), getAvgDelayNS(), getAvgDelay(), greenWE, greenNS
    
    
    avgDelayFile.close()
    
    plotDiagram(name)

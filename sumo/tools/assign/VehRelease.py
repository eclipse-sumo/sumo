"""
@file    VehRelease.py
@author  Yun-Pang.Wang@dlr.de
@date    2008-03-14
@version $Id: cVehRelease.py 2008-03-18$

This script is to generate the releasing time of each vehicle according to the Poisson distribution.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, string, sys, random, math
from network import Net
from elements import Vertex, Edge, Vehicle

def VehRelease(net, verbose, Parcontrol, departtime, CurrentMatrixSum):
    if verbose:
        print 'RandomVehRelease:', Parcontrol[(len(Parcontrol)-3)]
        print 'CurrentMatrixSum:', CurrentMatrixSum
        foutprob = file('probability.txt', 'w')
        foutrelease = file('release.txt', 'w')
        
    if int(Parcontrol[(len(Parcontrol)-3)]) == 0:
        for veh in net._vehicles:                                                       # generate the departure time for each vehicle uniform randomly
            if veh.depart == 0:
                veh.depart = random.randint(departtime, departtime + 3600)
    else:
        random.shuffle(net._vehicles)
        beta = float(3600. / CurrentMatrixSum)    # hourly matrix                       # generate the departure time for each vehicle poisson randomly
        releasetime = departtime
        for veh in net._vehicles:
            if veh.depart == 0.:
                probability = random.random()
                releasetime = releasetime + (-beta * (math.log(1.0 - probability)))
                veh.depart = releasetime
                if verbose: 
                    foutprob.write('%f\n' %probability)
                    foutrelease.write('%f\n' % veh.depart)
    if verbose:     
        foutprob.close()
        foutrelease.close()

        
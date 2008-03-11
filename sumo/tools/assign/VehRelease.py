#!/usr/bin/env python
# This script is to generate the releasing time of each vehicle according to the Poisson distribution.

import os, string, sys, random, math
from network import Net
from elements import Vertex, Edge, Vehicle

def VehRelease(net, Parcontrol, departtime, CurrentMatrixSum):
    print 'RandomVehRelease:', Parcontrol[(len(Parcontrol)-3)]
    print 'CurrentMatrixSum:', CurrentMatrixSum

#    foutprob = file('probability.txt', 'w')
#    foutrelease = file('release.txt', 'w')
    if int(Parcontrol[(len(Parcontrol)-3)]) == 0:
        for veh in net._vehicles:                                                       # generate the departure time for each vehicle uniform randomly
            if veh.depart == 0:
                veh.depart = random.randint(departtime, departtime + 3600)
    else:
        random.shuffle(net._vehicles)
        beta = float(3600. / CurrentMatrixSum)    # hourly matrix                       # generate the departure time for each vehicle poisson randomly
        print 'beta:', beta
        releasetime = departtime
        for veh in net._vehicles:
            if veh.depart == 0.:
                probability = random.random()
                releasetime = releasetime + (-beta * (math.log(1.0 - probability)))
                veh.depart = releasetime 
#                foutprob.write('%f\n' %probability)
#                foutrelease.write('%f\n' % veh.depart)
#        foutprob.close()
#        foutrelease.close()

        
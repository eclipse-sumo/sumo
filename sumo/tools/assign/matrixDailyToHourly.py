"""
@file    getHourlyMatrix.py
@author  Yun-Pang.Wang@dlr.de
@date    2008-08-20
@version $Id: getHourlyMatrix.py 5792 2008-07-15 14:53:05Z yunpangwang $

This script is to generate hourly matrices from a VISUM daily matrix. 

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, sys, operator
from optparse import OptionParser

OUTPUTDIR="./input/"    
optParser = OptionParser()
optParser.add_option("-m", "--matrix-file", dest="mtxpsfile", 
                     help="read OD matrix for passenger vehicles(long dist.) from FILE (mandatory)", metavar="FILE")
optParser.add_option("-t", "--timeSeries-file", dest="timeseries",
                     help="read hourly traffic demand rate from FILE", metavar="FILE")
optParser.add_option("-d","--dir", dest="OUTPUTDIR", default=OUTPUTDIR, help="Directory to store the output files. Default: "+OUTPUTDIR)
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")

(options, args) = optParser.parse_args()

# read the analyzed matrix         
def getMatrix(verbose, matrix):#, mtxplfile, mtxtfile):
    matrixPshort = []
    startVertices = []
    endVertices = []
    Pshort_EffCells = 0
    periodList = []
    MatrixSum = 0.
    if verbose:
        print 'matrix:', str(matrix)                                 
    ODpairs = 0
    origins = 0
    dest= 0
    CurrentMatrixSum = 0.0
    skipCount = 0
    zones = 0
    for line in open(matrix):
        if line[0] == '$':
            visumCode = line[1:4]
            if visumCode != 'VMR':
                skipCount += 1
        elif line[0] != '*' and line[0] != '$':
            skipCount += 1
            if skipCount == 2:
                for elem in line.split():
                    periodList.append(float(elem))
                print 'periodList:', periodList
            elif skipCount > 3:
                if zones == 0:
                    for elem in line.split():
                        zones = int(elem)
                        print 'zones:', zones
                elif len(startVertices) < zones:
                    for elem in line.split():
                        if len(elem) > 0:
                            startVertices.append(elem)
                            endVertices.append(elem)
                    origins = len(startVertices)
                    dest = len(endVertices)
                elif len(startVertices) == zones:
                    if ODpairs % origins == 0:
                        matrixPshort.append([])
                        subttotal = 0.
                    for item in line.split():
                        matrixPshort[-1].append(float(item))
                        ODpairs += 1
                        MatrixSum += float(item)
                        CurrentMatrixSum += float(item) 
                        if float(item) > 0.0:
                            Pshort_EffCells += 1                      
    begintime = int(periodList[0])
    if verbose:
        foutlog = file('log.txt', 'w')
        foutlog.write('Number of zones:%s, Number of origins:%s, Number of destinations:%s, begintime:%s, \n' %(zones, origins, dest, begintime))
        foutlog.write('CurrentMatrixSum:%s, total O-D pairs:%s, effective O-D pairs:%s\n' %(CurrentMatrixSum, ODpairs, Pshort_EffCells))
        print 'Number of zones:', zones
        print 'Number of origins:', origins
        print 'Number of destinations:', dest
        print 'begintime:', begintime
        print 'CurrentMatrixSum:', CurrentMatrixSum
        print 'total O-D pairs:', ODpairs
        print 'Effective O-D Cells:', Pshort_EffCells
        print 'len(startVertices):', len(startVertices)
        print 'len(endVertices):', len(endVertices)
        foutlog.close()

    return matrixPshort, startVertices, endVertices, Pshort_EffCells, MatrixSum, CurrentMatrixSum, begintime, zones
      
def main():
    if not options.mtxpsfile:
        optParser.print_help()
        sys.exit()
    MTX_STUB = "mtx%02i_%02i.fma"
    matrix = options.mtxpsfile
    if options.OUTPUTDIR:
        OUTPUTDIR = options.OUTPUTDIR
   
    matrixPshort, startVertices, endVertices, Pshort_EffCells, MatrixSum, CurrentMatrixSum, begintime, zones = getMatrix(options.verbose, matrix)
    timeSeriesList = []
    hourlyMatrix = []
    subtotal = 0.
    
    if options.verbose:
        foutlog = file('log.txt', 'a')
    
    for i in range (0, len(startVertices)):
        hourlyMatrix.append([])
        for j in range (0, len(endVertices)):
            hourlyMatrix[-1].append(0.)
    
    if options.timeseries:
        for line in open(options.timeseries):
            for elem in line.split():
                timeSeriesList.append(float(elem))
    else:
        factor = 1./24.
        for i in range(0,24):
            timeSeriesList.append(factor)
            
    for hour in range (0, 24):
        for i in range (0, len(startVertices)):
            for j in range (0, len(endVertices)):
                hourlyMatrix[i][j] = matrixPshort[i][j] * timeSeriesList[0]
        
        filename = MTX_STUB % (hour, hour+1)
#        if hour < 10:
#            if hour+1 != 10:
#                filename = "matrix0%s_0%s.fma" % (hour, hour+1)
#            else:
#                filename = "matrix0%s_%s.fma" % (hour, hour+1)
#        else:
#            filename = "matrix%s_%s.fma" % (hour, hour+1)

        foutmatrix=file(OUTPUTDIR+filename,'w')  # /input/filename
#        foutmatrix.write('$V;Y5\n')
        foutmatrix.write('$VMR;D2\n')
        foutmatrix.write('* Verkehrsmittelkennung\n')
        foutmatrix.write('   1\n')
        foutmatrix.write('*  ZeitIntervall\n')
        foutmatrix.write('    %s.00  %s.00\n' %(hour, hour+1))
        foutmatrix.write('*  Faktor\n')
        foutmatrix.write('   1.000000\n')
        foutmatrix.write('*  Anzahl Bezirke\n')
        foutmatrix.write('   %s\n' %zones)
        foutmatrix.write('*  BezirksNummern \n')
        
        for count, start in enumerate(startVertices):
            count += 1
            if count == 1:
                foutmatrix.write('          %s ' %start)
            else:
                foutmatrix.write('%s ' %start)
            if count != 1 and count % 10 == 0:
                foutmatrix.write('\n')
                foutmatrix.write('          ')
            elif count % 10 != 0 and count == len(startVertices): # count == (len(startVertices) -1):
                foutmatrix.write('\n')
                
        for i, start in enumerate(startVertices):
            subtotal = 0.
            foutmatrix.write('*  %s\n' % startVertices[i])
            foutmatrix.write('         ')
            for j, end in enumerate(endVertices):
                k = j + 1
                foutmatrix.write('        %.4f' % hourlyMatrix[i][j])
                subtotal += hourlyMatrix[i][j]
                if k % 10 == 0:
                    foutmatrix.write('\n')
                    foutmatrix.write('         ')
                elif k % 10 != 0 and j == (len(endVertices)-1):
                    foutmatrix.write('\n')
            if options.verbose:
                print 'origin:', startVertices[i]
                print 'subtotal:',subtotal
                foutlog.write('origin:%s, subtotal:%s\n' %(startVertices[i], subtotal))
        foutmatrix.close()
    if options.verbose:
        print 'done with generating', filename
        
    if options.verbose:
        foutlog.close()
main()
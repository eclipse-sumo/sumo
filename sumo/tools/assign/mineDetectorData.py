#!/usr/bin/env python
"""
@file    mineDetecterData.py
@author  Yun-Pang.Wang@dlr.de
@date    2008-06-04
@version $Id: mineDetecterData.py 731 2008-06-16 21:50:47Z behr_mi $

generate the hourly and daily traffic counts from the database.

Copyright (C) 2008 DLR/TS, Germany
All rights reserved
"""

import os, sys, MySQLdb, math, operator
import datetime
from optparse import OptionParser

class Data:
    def __init__(self, date):
        self.label = date
        self.detectorNum = 1
        self.hourlyFlowSet = {}
        self.pgrAlldayFlow = 0.0
        self.truckAlldayFlow = 0.0
        self.allDayPgrValid = True
        self.allDayTruckValid = True
        
    def __repr__(self):
        return "%s_<%s|%s|%s|%s>" % (self.label, self.pgrAlldayFlow, self.truckAlldayFlow, \
                                     self.allDayPgrValid, self.allDayTruckValid)
    def initialPeriodicFlowSet(self):
        flows0610 = Flow(610)
        flows1115 = Flow(1115)
        flows1620 = Flow(1620)
        flows2024 = Flow(2024)
        self.hourlyFlowSet[flows0610.label] = flows0610
        self.hourlyFlowSet[flows1115.label] = flows1115
        self.hourlyFlowSet[flows1620.label] = flows1620
        self.hourlyFlowSet[flows2024.label] = flows2024
        
class Flow:
    def __init__(self, hour):
        self.label = hour
        self.pgrFlow = 0.0
        self.truckFlow = 0.0
        self.pgrValid = True
        self.truckValid = True

    def __repr__(self):
        return "%s_<%s|%s|%s|%s>" % (self.label, self.pgrFlow, self.truckFlow, self.pgrValid, self.truckValid)
       
def correctFlow(pcounts15min, tcounts15min, correctPCounts, correctTCounts, validPgrCountInterval, validTruckCountInterval, minuteInterval, detectedInterval):
    bound = minuteInterval[1]
    if detectedInterval == 1. or bound == 1 or bound == 3:
        dataNum = math.floor(15./detectedInterval)
    elif bound == 0 or bound == 2:
        dataNum = math.ceil(15./detectedInterval)
        
    if correctPCounts > (dataNum*0.8):
        pcounts15min = pcounts15min/(correctPCounts/dataNum)
        validPgrCountInterval += 1.
    else:
        pcounts15min = 0.
    if correctTCounts > (dataNum*0.8):
        tcounts15min = tcounts15min/(correctTCounts/dataNum)
        validTruckCountInterval += 1.
    else:
        tcounts15min = 0.
    return pcounts15min, tcounts15min, validPgrCountInterval, validTruckCountInterval

def saveAggFlow(hour, data, pCounts15min, tCounts15min):
    if not hour in data.hourlyFlowSet:
        newflow = Flow(hour)
        data.hourlyFlowSet[newflow.label] = newflow
          
    data.hourlyFlowSet[hour].pgrFlow += pCounts15min
    data.hourlyFlowSet[hour].truckFlow += tCounts15min

    return data
    
def resetCounts(correctPCounts, correctTCounts, pCounts15min, tCounts15min):
    correctPCounts= 0.
    correctTCounts= 0.
    pCounts15min= 0.
    tCounts15min= 0.
    return correctPCounts, correctTCounts, pCounts15min, tCounts15min

def calAggFlow(row, pCounts15min, tCounts15min, correctPCounts, correctTCounts):
    if row[8] >= 80:
        if row[4] != None:
            pCounts15min += float(row[4])
            correctPCounts += 1.
        if row[5] != None:
            tCounts15min += float(row[5])
            correctTCounts += 1.
    return pCounts15min, tCounts15min, correctPCounts, correctTCounts

def getWeekday(wday):
    if wday == 0:
        weekday = 'MON'
    elif wday == 1:
        weekday = 'TUE'
    elif wday == 2:
        weekday = 'WED'
    elif wday == 3:
        weekday = 'THU'
    elif wday == 4:
        weekday = 'FRI'
    elif wday == 5:
        weekday = 'SAT'
    elif wday == 6:
        weekday = 'SUN'
    return weekday
        
def checkDimension(edgeID, groupID, aggFlow):
    if not edgeID in aggFlow:
        aggFlow[edgeID] = {}
    for weekday in range (0,7):
        if not weekday in aggFlow[edgeID]:
            aggFlow[edgeID][weekday]={}
        if not groupID in aggFlow[edgeID][weekday]:
            aggFlow[edgeID][weekday][groupID] = []
    
def checkdate(row, edgeID, wDay, aggFlow):
    groupID = row[2]
    date = row[3].date()
    hour = row[3].hour
    alreadyExist = False

    for elem in aggFlow[edgeID][wDay][groupID]:
        if elem.label == row[3].date():
            newdata = elem
            alreadyExist = True
            break
    if not alreadyExist:
        newdata = Data(date)
        newdata.initialPeriodicFlowSet()
        aggFlow[edgeID][wDay][groupID].append(newdata)
        newflow = Flow(hour)
        newdata.hourlyFlowSet[newflow.label] = newflow
    return newdata
        
                           
def main(conn, startDate, endDate, groupIDSet, detectorNum):
    cursor = conn.cursor()
    if not os.path.exists('groupIds.xml'):
#        cursor.execute("""SELECT navteq_id, group_id
#            FROM detector_group dg, road_section r, navteq_2_section n
#            WHERE dg.section_id=r.section_id AND n.section_id=r.section_id""")

#        groupIDSet= []
#        for row in cursor.fetchall():
#            groupIDSet.append(row[1])
#        groupIDSet.sort()
#        print 'Got the list of all group Ids!'
        
        foutlist = file('groupIds.xml', 'w')
        print >> foutlist, """<?xml version="1.0"?>
        <!-- generated on %s by $Id: mineDetecterData.py 5487 2008-04-30 14:51:16Z yunpangwang $ -->
        """ % datetime.datetime.now()
        foutlist.write('<groups>\n')
        for groupid in groupIDSet[:]:
            foutlist.write('    <group id="%s"/>\n' % groupid)
        foutlist.write('</groups>')
        foutlist.close()
        
        for i in range(0,7):
            indexSet = []
            day = getWeekday(i)
            entityname = "detectedFlowEntityDef_%s.xml" % day
            foutfiles = file(entityname, 'w')
            print >> foutfiles, """<?xml version="1.0"?>
        <!-- generated on %s by $Id: mineDetecterData.py 5487 2008-04-30 14:51:16Z yunpangwang $ -->
        """ % datetime.datetime.now()
            foutfiles.write('<!DOCTYPE detectedFlows [\n')
            for groupid in groupIDSet[:]:
                groupindex = day+'_'+str(groupid)
                foutfiles.write('  <!ENTITY %s SYSTEM "N:\Benutzer\U-Z\Yunpang\matrixTest\detectedFlows_%s.xml">\n' %(groupindex,groupindex))
                indexSet.append(groupindex)
            foutfiles.write(']>\n')
            foutfiles.write('<detectedFlows>\n')            
            for index in indexSet[:]:
                foutfiles.write('    &%s;\n' % index)
            foutfiles.write('</detectedFlows>')
            foutfiles.close()
        indexSet = []
    
        print 'Done with the generation of entity-files!'
        
    cursor.execute("""
        SELECT group_id, count(*) FROM detector GROUP BY group_id""")
    
#    detectorNum = {}
    if not os.path.exists('NumberofDetectors.xml'):
        foutdetector = file('NumberofDetectors.xml', 'w')
        print >> foutdetector, """<?xml version="1.0"?>
        <!-- generated on %s by $Id: mineDetecterData.py 5487 2008-04-30 14:51:16Z yunpangwang $ -->
        """ % datetime.datetime.now()            
        foutdetector.write('<groups>\n')
        for row in cursor.fetchall():
            detectorNum[row[0]] = row[1]
            foutdetector.write('    <group id="%s" detectors="%s"/>\n' %(row[0], row[1]))
        foutdetector.write('</groups>')
        foutdetector.close()
            
        print 'Done with the counting of detectors for each groudID!'
        
    print 'Start retriving historic data base!'
    cursor.execute("""
    SELECT n.navteq_id, d.detector_id, d.group_id, time, q_pkw, q_lkw, v_pkw, v_lkw, quality
        FROM value_corrected v, detector d, detector_group dg, road_section r, navteq_2_section n
        WHERE time >= '%s' AND time < '%s' AND
              v.detector_id=d.detector_id AND d.group_id=dg.group_id AND dg.section_id=r.section_id AND n.section_id=r.section_id
        ORDER BY n.navteq_id, d.group_id, detector_id, time""" % (startDate, endDate))
    print 'Got historic data base!'       
    edgeID = ""
    aggFlow = {}
    correctPCounts = 0.
    correctTCounts = 0.
    pCounts15min = 0.
    tCounts15min = 0.
    validPgrCountInterval = 0.   # 0-3
    validTruckCountInterval = 0. 
    periodSet = [(6,10), (11,15), (16,20), (20,24)]
    needAggFlow = False
    
    for row in cursor.fetchall():
        minutecheck = math.modf(row[3].minute/15.)    # return (fractional part, integer part)
        if options.verbose and row[3].weekday() == 5 and row[2] == 279:
            foutdatabank.write('%s %s %s %s %s %s %s %s %s\n' %(row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7], row[8]))
        if needAggFlow:
            if row[0] != edgeID or row[1] != detectorID or row[2] != groupID or row[3].date() != date or row[3].hour != hour:
                pCounts15min, tCounts15min, validPgrCountInterval, validTruckCountInterval = correctFlow(pCounts15min, tCounts15min, correctPCounts, correctTCounts, validPgrCountInterval, validTruckCountInterval, minuteInterval, options.detectedInterval)
                newdata = saveAggFlow(hour, newdata, pCounts15min, tCounts15min)
    
                correctPCounts, correctTCounts, pCounts15min, tCounts15min = resetCounts(correctPCounts, correctTCounts, pCounts15min, tCounts15min)
                needAggFlow = False

                if validPgrCountInterval != 4.:
                    newdata.hourlyFlowSet[hour].pgrValid = False
                    newdata.allDayPgrValid =  False
                    for interval in periodSet:
                        if hour>= interval[0] and hour < interval[1]:
                            timeInterval = str(interval[0])+str(interval[1])
                            timeInterval = int(timeInterval)
                            newdata.hourlyFlowSet[timeInterval].pgrValid = False
                            
                if validTruckCountInterval != 4.:
                    newdata.hourlyFlowSet[hour].truckValid = False
                    newdata.allDayTruckValid = False
                    for interval in periodSet:
                        if hour>= interval[0] and hour < interval[1]:
                            timeInterval = str(interval[0])+str(interval[1])
                            timeInterval = int(timeInterval)
                            newdata.hourlyFlowSet[timeInterval].truckValid = False
                            
                validPgrCountInterval = 0.
                validTruckCountInterval = 0.
            elif minutecheck[1] != minuteInterval[1]:
                pCounts15min, tCounts15min, validPgrCountInterval, validTruckCountInterval = correctFlow(pCounts15min, tCounts15min, correctPCounts, correctTCounts, validPgrCountInterval, validTruckCountInterval, minuteInterval, options.detectedInterval)
                newdata = saveAggFlow(hour, newdata, pCounts15min, tCounts15min)

                correctPCounts, correctTCounts, pCounts15min, tCounts15min = resetCounts(correctPCounts, correctTCounts, pCounts15min, tCounts15min)
                needAggFlow = False

        if row[0] != '' and row[0] != edgeID:
#            print 'edgeID:', row[0]
            edgeID = row[0]
            detectorID = row[1]
            groupID = row[2]
            date = row[3].date()
            wDay = row[3].weekday()
            hour = row[3].hour
            
            minuteInterval = math.modf(row[3].minute/15.)    #(fractional part,integer part)
               
            checkDimension(edgeID, groupID, aggFlow)
    
            newdata = checkdate(row, edgeID, wDay, aggFlow)
            newdata.initialPeriodicFlowSet()
                 
            correctPCounts, correctTCounts, pCounts15min, tCounts15min = resetCounts(correctPCounts, correctTCounts, pCounts15min, tCounts15min)
            pCounts15min, tCounts15min, correctPCounts, correctTCounts = calAggFlow(row, pCounts15min, tCounts15min, correctPCounts, correctTCounts)    

            needAggFlow = True
        else:
            wDay = row[3].weekday()
            hour = row[3].hour
            minuteInterval = math.modf(row[3].minute/15.)
            
            if row[2] != groupID:
                groupID = row[2]
                checkDimension(edgeID, groupID, aggFlow)
    
                date = row[3].date()
                newdata = checkdate(row, edgeID, wDay, aggFlow)
                newdata.initialPeriodicFlowSet()

                correctPCounts, correctTCounts, pCounts15min, tCounts15min = resetCounts(correctPCounts, correctTCounts, pCounts15min, tCounts15min)
                pCounts15min, tCounts15min, correctPCounts, correctTCounts = calAggFlow(row, pCounts15min, tCounts15min, correctPCounts, correctTCounts)    
                  
            elif detectorID != row[1]:
                detectorID = row[1]
                if row[3].date() != date:
                    tempdata = newdata
                    newdata = checkdate(row, edgeID, wDay, aggFlow)
                    if newdata.label == tempdata.label:
                        newdata.detectorNum += 1
                else:
                    newdata.detectorNum += 1 
                
                correctPCounts, correctTCounts, pCounts15min, tCounts15min = resetCounts(correctPCounts, correctTCounts, pCounts15min, tCounts15min)
                pCounts15min, tCounts15min, correctPCounts, correctTCounts = calAggFlow(row, pCounts15min, tCounts15min, correctPCounts, correctTCounts)    
                          
            elif row[3].date() != date:
                newdata = checkdate(row, edgeID, wDay, aggFlow)
    
                correctPCounts, correctTCounts, pCounts15min, tCounts15min = resetCounts(correctPCounts, correctTCounts, pCounts15min, tCounts15min)
                pCounts15min, tCounts15min, correctPCounts, correctTCounts = calAggFlow(row, pCounts15min, tCounts15min, correctPCounts, correctTCounts)    
    
            detectorID = row[1]
            groupID = row[2]
            date = row[3].date()
            
            pCounts15min, tCounts15min, correctPCounts, correctTCounts = calAggFlow(row, pCounts15min, tCounts15min, correctPCounts, correctTCounts)
            needAggFlow = True
    
    print 'generating periodic data!'
    # aggregate period data
    for edgeID in aggFlow:
        for wday in range(0,7):
            weekday = getWeekday(wday)
            for group in aggFlow[edgeID][wday]:
                for data in aggFlow[edgeID][wday][group]:
                    data.pgrAlldayFlow = 0.
                    data.truckAlldayFlow = 0.

                    for i in range (0, 24):
                        if not i in data.hourlyFlowSet:
                            data.allDayPgrValid = False
                            data.allDayTruckValid = False
                            for interval in periodSet:
                                if i >= interval[0] and i < interval[1]:
                                    timeInterval = str(interval[0])+str(interval[1])
                                    timeInterval = int(timeInterval)
                                    data.hourlyFlowSet[timeInterval].pgrValid = False
                                    data.hourlyFlowSet[timeInterval].truckValid = False
                                    
                    for hour in data.hourlyFlowSet.itervalues():
                        if hour.label != 610 and hour.label != 1115 and hour.label != 1620 and hour.label != 2024:
                            if hour.label < 10 and hour.label >= 6:
                                if hour.pgrValid and data.hourlyFlowSet[610].pgrValid:
                                    data.hourlyFlowSet[610].pgrFlow += hour.pgrFlow
                                if hour.truckValid and data.hourlyFlowSet[610].truckValid:
                                    data.hourlyFlowSet[610].truckFlow += hour.truckFlow

                            elif hour.label < 15 and hour.label >= 11:
                                if hour.pgrValid and data.hourlyFlowSet[1115].pgrValid:
                                    data.hourlyFlowSet[1115].pgrFlow += hour.pgrFlow
                                if hour.truckValid and data.hourlyFlowSet[1115].truckValid:
                                    data.hourlyFlowSet[1115].truckFlow += hour.truckFlow

                            elif hour.label < 20 and hour.label >= 16:
                                if hour.pgrValid and data.hourlyFlowSet[1620].pgrValid:
                                    data.hourlyFlowSet[1620].pgrFlow += hour.pgrFlow
                                if hour.truckValid and data.hourlyFlowSet[1620].truckValid:
                                    data.hourlyFlowSet[1620].truckFlow += hour.truckFlow

                            elif hour.label < 24 and hour.label >= 20:
                                if hour.pgrValid and data.hourlyFlowSet[2024].pgrValid:
                                    data.hourlyFlowSet[2024].pgrFlow += hour.pgrFlow
                                if hour.truckValid and data.hourlyFlowSet[2024].truckValid:
                                    data.hourlyFlowSet[2024].truckFlow += hour.truckFlow

                            if hour.pgrValid and data.allDayPgrValid:
                                data.pgrAlldayFlow += hour.pgrFlow

                            if hour.truckValid and data.allDayTruckValid:
                                data.truckAlldayFlow += hour.truckFlow
          
# output for each weekday-groudID
    print 'writing data in the files!'
    for edgeID in aggFlow:
        for wday in range(0,7):
            weekday = getWeekday(wday)
            for group in aggFlow[edgeID][wday]:
                filename = "detectedFlows_%s_%s.xml" % (weekday, group)
                if os.path.exists(filename):
                    foutdata = file(filename, 'a')
                else:
                    foutdata = file(filename, 'w')
                    print >> foutdata, """<?xml version="1.0"?>
    <!-- generated on %s by $Id: mineDetecterData.py 5487 2008-04-30 14:51:16Z yunpangwang $ -->
    <detectedFlows>""" % datetime.datetime.now()
                for data in aggFlow[edgeID][wday][group]:
                    if detectorNum[group] == data.detectorNum:
                        foutdata.write('    <dataset edgeid="%s" groupid="%s" detectors="%s" date="%s">\n' %(edgeID, group, data.detectorNum, data.label))
                        for hour in data.hourlyFlowSet.itervalues():
                            foutdata.write('            <flows weekday-time="%s" passengercars="%s" truckflows="%s" pgrValid="%s" truckValid="%s"/>\n' %((str(weekday)+'-'+str(hour.label)), hour.pgrFlow, hour.truckFlow, hour.pgrValid, hour.truckValid)) 
                            if hour.pgrValid and hour.truckValid and hour.label != 610 and hour.label != 1115 and hour.label != 1620 and hour.label != 2024 and (hour.pgrFlow/float(data.detectorNum) > 1800. or hour.truckFlow/float(data.detectorNum) > 1800.):
                                fouterror.write( '**** groupID="%s" date="%s" detector="%s" hour="%s" hour.pgrFlow="%s" hour.truckFlow="%s\n' %(group, data.label, detectorNum[group], hour.label, hour.pgrFlow, hour.truckFlow))
                        foutdata.write('            <flows weekday-time="%s" passengercars="%s" truckflows="%s" pgrValid="%s" truckValid="%s"/>\n' %(weekday+'-Allday', data.pgrAlldayFlow, data.truckAlldayFlow, data.allDayPgrValid, data.allDayTruckValid))    
                        foutdata.write('    </dataset>\n')
                        
                        if data.allDayPgrValid == True and data.allDayTruckValid == True:
                            foutcorrect.write('    <dataset edgeid="%s" groupid="%s" detectors="%s" date="%s">\n' %(edgeID, group, data.detectorNum, data.label))
                            foutcorrect.write('            <flows weekday-time="%s" passengercars="%s" truckflows="%s" pgrValid="%s" truckValid="%s"/>\n' %(weekday+'-Allday', data.pgrAlldayFlow, data.truckAlldayFlow, data.allDayPgrValid, data.allDayTruckValid))
                            foutcorrect.write('    </dataset>\n')
                    else:
                        fouterror.write( 'groupID="%s" date="%s" active detector="%s" total detector="%s"\n' %(group, data.label, data.detectorNum, detectorNum[group]))
                        print 'active detectors != total detectors for groudID:', group

                foutdata.close()

optParser = OptionParser(usage="usage: %prog [options] <prefix>")
optParser.add_option("-b", "--database", dest="conn",
                     help="write to database connection, "
                          "[default: %default]",
                     default="tester:test:129.247.218.69:delphi_historisch",
                     metavar="user:passwd:host:db")
optParser.add_option("-i", "--detectedInterval", dest="detectedInterval", type="float",
                     default=1., help="tell me the detected interval(min)")
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-w", "--warn", action="store_true", dest="warn",
                     default=False, help="print all warnings")
(options, args) = optParser.parse_args()
dbargs = options.conn.split(":")
start = datetime.datetime(2008, 9, 1)   #2/21
end = datetime.datetime(2008, 9, 30)     #3/11
interval = datetime.timedelta(1)
begintime = datetime.datetime.now()
print options.detectedInterval
if options.verbose:
    foutdatabank = file('database.txt', 'w')
outputfile = "correctAlldayData_%s_%s.xml" % (start.date(), end.date())

conn = MySQLdb.connect(host = dbargs[2], user = dbargs[0],
                       passwd = dbargs[1], db = dbargs[3])
cursor = conn.cursor()
cursor.execute("""SELECT navteq_id, group_id
    FROM detector_group dg, road_section r, navteq_2_section n
    WHERE dg.section_id=r.section_id AND n.section_id=r.section_id""")

groupIDSet= []
detectorNum = {}
for row in cursor.fetchall():
    groupIDSet.append(row[1])
groupIDSet.sort()
conn.close()
print 'Got the list of all group Ids!'


while start < end:  
    if os.path.exists('faultyDetectorData.txt'):
        fouterror = file('faultyDetectorData.txt', 'a')
    else:
        fouterror = file('faultyDetectorData.txt', 'w')
    
    if  os.path.exists(outputfile):
        foutcorrect = file(outputfile, 'a')
    else:
        foutcorrect = file(outputfile, 'w')
        print >> foutcorrect, """<?xml version="1.0"?>
        <!-- generated on %s by $Id: mineDetecterData.py 5487 2008-04-30 14:51:16Z yunpangwang $ -->
        <detectedFlows>""" % datetime.datetime.now()    
    
    print "Retrieving data for %s to %s" % (start, min(start + interval, end))
    conn = MySQLdb.connect(host = dbargs[2], user = dbargs[0],
                           passwd = dbargs[1], db = dbargs[3])
    main(conn, start, min(start + interval, end), groupIDSet, detectorNum)
    conn.close()
    start += interval
    
if options.verbose:
    foutdatabank.close()
    
fouterror.close()
foutcorrect.close()
print 'requried CPU time:', datetime.datetime.now() - begintime

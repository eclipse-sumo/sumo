# -*- coding: cp1252 -*-
# DetectorTest, um die Werte von Detectoren zu überprüfen
# @Autor Danilo Boyom 09.12.2004


import xml.dom.minidom
from xml.sax.saxutils import escape

#---------------------------------------------------------------------
# Die Klasse DetectorTest ist die wichtige Klasse,
# Sie stellt die Methoden um alle überprüfungen zu machen dar

class  DetectorTest:

    # Konstruktor
    def __init__(self, dump, output):
        # Alle Werte erst mal zurücksetezn, als Parameter, bekommt den
        # Konstruktor die Dump.xml-File und den Detector-Output-File
        self.init()
        self.dump_datei = dump
        self.det_output = output
        # Die frequenz(Die Detector Frequenz)
        self.frequenz = 1
        # Initialisierung Des Passenden Detector
        self.detector = Detector("","",0,0,"")

    # Setz erst mal alle Werte auf "NULL"
    def init(self):
        # Dictionary of Detector  detectorId => Detector
        self.detectorDic = {}
        # Dictionary of TimeStep  TimeStepId => TimeStep
        self.timeStepDic = {}

        # Dictionary, wo die Detector-output Werte drin gespeichert sind,
        # notwendig für ein Späterer Vergleich.
        self.Intervals = {}        #TimeStepId => TimeStep

    # Diese Methode liest ein add.xml-File und
    # Speichert die Detectoren in eine Dictionary
    def loadDetector(self, file):
        try:
            dom = xml.dom.minidom.parse(file)
            first = dom.getElementsByTagName("sumo-detectors")
            self.detectorDic = self.getDetectors(dom, "detector", "id")

        except: print" Problem beim Laden von Datector-File", file

    # Hilf-Methode für die Method LoadDetector
    def getDetectors(self, dom, tagname, attrname):
       nodelist = dom.getElementsByTagName(tagname)
       result = {}
       if nodelist:
           index = 0
           while index < len(nodelist):
               node = nodelist[index]
               id   = node.getAttribute(attrname).strip()
               laneId = node.getAttribute("lane").strip()
               posi = float(node.getAttribute("pos").strip())
               freq = node.getAttribute("freq").strip()
               file = node.getAttribute("file").strip()
               det = Detector(id, laneId, posi, freq, file)
               result[id] = det
               index = index + 1
               if file == "output/"+self.det_output:
                   # Der Detector mit der Passende output-File
                   self.detector = det
           return result
       else:
          return result



    def loadTimeStep(self, file):
        try:
            dom = xml.dom.minidom.parse(file)
            first = dom.getElementsByTagName("sumo-netstate")
            self.getTimeSteps(dom, "timestep", "id")

        except: print" Problem beim Laden von TimeStep-File ", file


    # gib ein dictionary, wo alle timeStep gespeichert sind
    def getTimeSteps(self, dom, tagname, attrname):
       timelist = dom.getElementsByTagName(tagname)
       print "TimeStep length: ",len(timelist)
       if timelist:
           index1 = 0
           while index1 < len(timelist):
               time = timelist[index1]
               index1 = index1 + 1
               timeid   = time.getAttribute(attrname).strip()
               timestep = TimeStep(timeid)
               lanelist = time.getElementsByTagName("lane")
               print "timestep ", timeid
               if lanelist:
                   index2 = 0
                   while index2 < len(lanelist):
                       lan = lanelist[index2]
                       index2 = index2 + 1
                       laneid = lan.getAttribute("id")
                       lane = Lane(laneid)
                       print "      laneId", laneid
                       vehlist = lan.getElementsByTagName("vehicle")
                       if vehlist:
                           index3 = 0
                           while index3 < len(vehlist):
                               node = vehlist[index3]
                               vehid = node.getAttribute("id")
                               vehpos = float(node.getAttribute("pos"))
                               vehspeed = float(node.getAttribute("speed"))
                               veh = Vehicle(vehid, vehpos, vehspeed)
                               lane.vehicles[vehid] = veh
                               index3 = index3 + 1
                       timestep.lanes[laneid] = lane
               self.timeStepDic[int(timeid)] = timestep


    # Method zu berechnen alle Werte, die zu vergleichen sind
    def calculate(self, freq):
        index = 0
        key = self.timeStepDic.keys()
        datei = open("detectorTest-output.xml", "w")
        datei.write(self.getXMLHeader())
        str = '<detector type="inductionloop" id="%s" ' %self.detector.Id
        str = str+'lane="%s" ' %self.detector.laneId
        str = str+'pos="%s">\n' %int(self.detector.pos)
        datei.write(str)
        key.sort()
        while index < len(self.timeStepDic):
            step = self.timeStepDic[key[index]]
            step.calculateAllValue(self.detector)# berechnet für jeder Step die Werte
            str2 = '<interval start="%s" stop="%s" nVehContrib="%s" '%(step.start,step.stop,int(step.nVehContrib))
            str2 = str2+'flow="%s" occupancy="%s" speed="%s" ' %(step.flow,step.occupancy, step.speed)
            str2 = str2+'speedsquare="%s" length="%s" nVehCrossed="%s" />\n' %(step.speedsquare, step.length,int(step.nVehCrossed))
            datei.write(str2)
            index = index + 1
        datei.write( '</detector>')
        datei.close()


    def getXMLHeader(self):
        s = '<?xml version="1.0" standalone="yes" ?>\n'
        s = s + '<!--\n'
        s = s + ' - nVehContrib is the number of vehicles that passed the detector during the\n'
        s = s + '   current interval.\n'
        s = s + ' - flow [veh/h] denotes the same quantity in [veh/h]\n'
        s = s + ' - occupancy [%] is the time the detector was occupied by vehicles.\n'
        s = s + ' - speed [m/s] is the average speed of the nVehContib vehicles.\n'
        s = s + '   If nVehContrib==0, speed is set to -1.\n'
        s = s + ' - speedsquare [(m/s)^2]\n'
        s = s + '   If nVehContrib==0, speedsquare is set to -1.\n'
        s = s + ' - length [m] is the average vehicle length of the nVehContrib vehilces.\n'
        s = s + '   If nVehContrib==0, length is set to -1.\n'
        s = s + '-->\n'
        return s


    # Überpruft , ob die Beide Datei inhaltlich gleich sind
    def compare(self,file):
         try:
            dom = xml.dom.minidom.parse(file)
            nodelist = dom.getElementsByTagName("detector")
            if nodelist:
                assert len(nodelist) == 1
                node = nodelist[0]
                type = node.getAttribute("type").strip()
                id = node.getAttribute("id").strip()
                lane = node.getAttribute("lane").strip()
                pos  = node.getAttribute("pos").strip()
                self.Intervals = self.getIntervalsValue(dom, "interval", "start")
                compareResult = self.compareAllValue(self.Intervals, self.timeStepDic)

                print "==========Ergebniss der Simulation=============\n"
                if(compareResult == 1):
                    print "Beide Output sind gleich\n"
                else:
                    print "Beide output sind Unterschiedlich\n"
                print "==============================================\n"

         except:
             print" Problem beim Laden von outputFile-File", file


      # Hilf-Methode für die Method LoadDetector
    def getIntervalsValue(self, dom, tagname, attrname):
       nodelist = dom.getElementsByTagName(tagname)
       result = {}
       if nodelist:
           index = 0
           while index < len(nodelist):
               node = nodelist[index]
               id   = node.getAttribute(attrname).strip()
               interval= TimeStep(id)
               interval.start = id
               interval.stop = node.getAttribute("stop").strip()
               interval.nVehContrib = int(node.getAttribute("nVehContrib").strip())
               interval.flow = node.getAttribute("flow").strip()
               interval.occupancy = node.getAttribute("occupancy").strip()
               interval.speed = node.getAttribute("speed").strip()
               interval.speedsquare = node.getAttribute("speedsquare").strip()
               interval.length = node.getAttribute("length").strip()
               interval.nVehCrossed = node.getAttribute("nVehCrossed").strip()
               result[id] = interval
               index = index + 1
           return result
       else:
          return result


    # Vergleich für alle Intervals, unserer Werte, mit der Output-Werte von Detectoren
    # gib true, falls alle Werte gleich sind
    def compareAllValue(self, intervals, timeStepDic):
        #erst mal müssen die Beide gleich lang sein
       # assert(len(intervals) == len(timeStepDic))
        intervalsKey = intervals.keys()
        index1 = 0
        result = 1
        while index1 < len(intervals):
            step1 = intervals[intervalsKey[index1]]
            step2 = timeStepDic[int(step1.Id)]
            index1 = index1 + 1
            print "step1 ", step1.Id, " step2 ", step2.Id
            if int(step1.Id) != int(step2.Id) or int(step1.stop) != int(step2.stop):
                result = 0
        return result

    # Ein art Main-method
    def __main__(self):
         #1. Detectoren Laden
         self.loadDetector("../cross3ltl.add.xml")
         if self.detector.output == "output/"+self.det_output:
             self.loadTimeStep(self.dump_datei)
             self.calculate(self.frequenz)
             self.compare(self.det_output)
         else:
             print "Die File: ",self.det_output," ist Output keinen unserer Detector"

#---------------------------------------------------------------------
# Die Klasse TimeStep, representiert einen gewöhnlichen TimeStep
# mit alle sein Attributen

class TimeStep:

    # Konstruktor
    def __init__(self, nId):
        # Alle Werte erst mal zurücksetezn, n steht für "new"
        self.init()
        self.Id = nId

    # init, setz erst mal alle Werte auf "NULL"
    def init(self):
        self.Id =""
        self.lanes = {} # Dictionatry von laneId => Lane
        self.passedFornVehc = [] # liste von Vehicle, die den Detector schon passiert haben
        self.passedForSpeed = [] # liste von Vehicle, die den Detector schon passiert haben
        #Ausgaben
        self.start = 0
        self.stop  = 0
        self.nVehContrib = 0 # the number of vehicles that passed the detector during the current interval.
        self.nVehCrossed = 0 #  -----------//---------------
        self.flow = 0      # the number of vehicles that passed the detector during 1 hour
        self.occupancy = 0 # the time the detector was occupied by vehicles.
        self.speed = -1
        self.speedsquare = -1
        self.length = -1

    # berechnet alle Ausgang-Werte
    def calculateAllValue(self, det):
        self.start = int(self.Id)
        self.stop = int(self.Id) + 1
        self.nVehContrib = self.nVehContrib + self.getMyVehicleContrib(det)
        self.nVehCrossed = self.nVehCrossed + self.getMyVehicleContrib(det)
        self.flow = 60 * self.nVehContrib
        self.occupancy = self.getMyVehicleContrib(det) * 100
        self.speed = self.getMySpeed(det)
        self.speedsquare = self.speed * self.speed
        self.length = 5*self.nVehContrib


    # berechnet die Wert nVehContrib
    def getMyVehicleContrib(self, detector):
        result = 0
        index = 0
        key = self.lanes.keys() # erst mal alle Lane, die zu diese Zeit gefahren werden
        while index < len(self.lanes):
            lane = self.lanes[key[index]]
            index = index + 1
            if detector.laneId == lane.Id: # wenn der Detector auf eine diese Lane ist, dann
                 index1 = 0
                 key1 = lane.vehicles.keys()
                 while index1 < len(lane.vehicles):
                     veh = lane.vehicles[key1[index1]]
                     index1 = index1 + 1
                     if (veh.pos >= detector.pos) and (self.isNotIn1(veh.Id)):
                         result = result + 1
                         self.passedFornVehc.append(veh.Id)
        return result


    def getMySpeed(self, detector):
        result = 0
        index = 0
        key = self.lanes.keys()
        while index < len(self.lanes):
            lane = self.lanes[key[index]]
            index = index + 1
            if detector.laneId == lane.Id:
                index1 = 0
                key1 = lane.vehicles.keys()
                while index1 < len(lane.vehicles):
                    veh = lane.vehicles[key1[index1]]
                    index1 = index1 + 1
                    if(veh.pos >= detector.pos + 5) and self.isNotIn2(veh.Id):
                        result = result + veh.speed
                        self.passedForSpeed.append(veh.Id)
        return result


    #gibt true, falls element nicht in Passed-liste ist, sonst false
    def isNotIn1(self, element):
        index = 0
        result = 1
        while index < len(self.passedFornVehc):
            if element == self.passedFornVehc[index]:
                result = 0
                break
            else:
                index = index + 1
        return result


    #gibt true, falls element in Passed-liste ist, ist sonst false
    def isNotIn2(self, element):
        index = 0
        result = 1
        while index < len(self.passedForSpeed):
            if element == self.passedForSpeed[index]:
                result = 0
                break
            else:
                index = index + 1
        return result




#------------------------------Klasse Detector----------------------------------
# Die Klasse Detector, representiert einen gewöhnlichen e1-Detector
# mit alle seiner Attributen entsprechend die XML-File

class Detector:

    # Konstruktor
    def __init__(self, nId, nlaneId, npos, nfreq, noutput):
        # Alle Werte erst mal zurücksetezn, n steht für "new"
        self.init()
        self.Id = nId
        self.laneId = nlaneId
        self.pos = npos
        self.freq = nfreq
        self.output = noutput

    # init, setz erst mal alle Werte auf "NULL"
    def init(self):
        self.Id =""
        self.laneId = ""
        self.pos = 0
        self.freq = 0
        self.output =""

#--------------------------klasse Vehicle----------------------------------------
# Die Klasse Vehicle, representiert ein gewöhnliches Fahrzeug
# bei SUMO mit alle seiner Attributen entsprechend die XML-File

class Vehicle:

    # Konstruktor
    def __init__(self, nId, nPos, nSpeed):
        # Alle Werte erst mal zurücksetezn
        self.init()
        self.Id = nId
        self.pos = nPos
        self.speed = nSpeed

    # init, setz erst mal alle Werte auf "NULL"
    def init(self):
        self.Id =""
        self.pos = 0
        self.speed = 0




#----------------------------Klasse Lane-----------------------------------------
# Die Klasse Lane, representiert eine gewöhnliche Lane
# bei SUMO, mit alle seiner Attributen entsprechend die XML-File

class Lane:

    # Konstruktor
    def __init__(self, nId):
        # Alle Werte erst mal zurücksetezn
        self.init()
        self.Id = nId

    # init, setz erst mal alle Werte auf "NULL"
    def init(self):
        self.Id =""
        self.vehicles = {} # jeder Lane hat 0 oder n Vehicles, die zu einer Zeit-Step drauf stehen

    def getMyVehicles(self):
        index = 0
        key = self.vehicles.keys()
        while index < len(self.vehicles):
            print key[index]," ===> ",(self.vehicles[key[index]]).Id
            index = index + 1




#-------------------------ProgrammsAufruf--------------------------

det = DetectorTest("dump.xml", "1si_0.xml")
det.__main__()















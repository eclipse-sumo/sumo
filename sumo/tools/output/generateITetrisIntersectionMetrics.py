from optparse import OptionParser
import os
from pylab import *
from xml.sax import saxutils, make_parser, handler
sys.path.append(os.path.join(os.path.abspath(os.path.dirname(sys.argv[0])), "../lib"))
import sumonet

def getBasicStats(net, lanesInfo, T):
    tlsInfo = {}
    for tl in net._tlss:
        tlID = tl._id
        mQueueLen = []
  #      mWaitTime = []
        nbStops = []
        tWaitTime = []
        seenLanes = set()
        for conn in tl._connections:
            lane = conn[0]
            if lane in seenLanes:
                continue
            seenLanes.add(lane)
            mQueueLenInfo = sum(lanesInfo[lane.getID()]['mQueueLen'])
            mQueueLen.append(mQueueLenInfo)
  #              mWaitTimeInfo = mean(lanesInfo[lane.getID()]['mWaitTime'])
  #              mWaitTime.append(mWaitTimeInfo)
            nbStopsInfo = sum(lanesInfo[lane.getID()]['nbStops'])
            nbStops.append(nbStopsInfo)
            tWaitTimeInfo = sum(lanesInfo[lane.getID()]['tWaitTime'])
            tWaitTime.append(tWaitTimeInfo)
        tlsInfo[tlID] = {}
        tlsInfo[tlID]['mQueueLen'] = mean(mQueueLen) / T
        tlsInfo[tlID]['mWaitTime'] = mean(tWaitTime) / T
        tlsInfo[tlID]['nbStops'] = sum(nbStops)
        tlsInfo[tlID]['tWaitTime'] = sum(tWaitTime)
    return tlsInfo

def getStatisticsOutput(tlsInfo, outputfile):
    opfile = file(outputfile, 'w')
    for tl in tlsInfo.keys():
        opfile.write('Traffic Light %s\n' % tl)
        opfile.write('=================\n')
        opfile.write('mean queue length in front of the junction: %s\n' % tlsInfo[tl]['mQueueLen'])
        opfile.write('mean waiting time in front of the junction: %s\n' % tlsInfo[tl]['mWaitTime'])
        opfile.write('number of stops: %s\n' % tlsInfo[tl]['nbStops'])
        opfile.write('total waiting time at junction: %s\n\n' % tlsInfo[tl]['tWaitTime'])
    

class E2OutputReader(handler.ContentHandler):
    def __init__(self):
        self._lanes = {}
        self._maxT = 0

            
    def startElement(self, name, attrs):
        if name == 'interval':
            detID = attrs['id']
            laneID = detID[6:len(detID)]
            if not self._lanes.has_key(laneID):
                self._lanes[laneID] = {}
                self._lanes[laneID]['mQueueLen'] = []
 #               self._lanes[laneID]['mWaitTime'] = []
                self._lanes[laneID]['nbStops'] = []
                self._lanes[laneID]['tWaitTime'] = []
            if float(attrs['end']) < 100000000:
                self._lanes[laneID]['mQueueLen'].append(float(attrs['jamLengthInMetersSum']))
 #               self._lanes[laneID]['mWaitTime'].append(float(attrs['meanHaltingDuration']))
                self._lanes[laneID]['nbStops'].append(float(attrs['startedHalts']))
                self._lanes[laneID]['tWaitTime'].append(float(attrs['haltingDurationSum']))
                self._maxT = max(float(attrs['end']), self._maxT)


# initialise 
optParser = OptionParser()
optParser.add_option("-n", "--netfile", dest="netfile",
                     help="name of the netfile (f.e. 'inputs\\pasubio\\a_costa.net.xml')", metavar="<FILE>", type="string")
optParser.add_option("-p", "--path", dest="path",
                     help="name of folder to work with (f.e. 'inputs\\a_costa\\')", metavar="<FOLDER>", type="string", default="./")

optParser.set_usage('\n-n inputs\\pasubio\\pasubio.net.xml -p inputs\\pasubio\\')
# parse options
(options, args) = optParser.parse_args()
if not options.netfile:
    print "Missing arguments"
    optParser.print_help()
    exit()

netfile = options.netfile
e2OutputFile = os.path.join(options.path, 'e2_output.xml')

net = sumonet.NetReader()
parser = make_parser()
parser.setContentHandler(net)
parser.parse(netfile)
net = net.getNet()

e2Output = E2OutputReader()
parser.setContentHandler(e2Output)
parser.parse(e2OutputFile)
tlsInfo = getBasicStats(net, e2Output._lanes, e2Output._maxT)
getStatisticsOutput(tlsInfo, os.path.join(options.path, "intersection_metrics_summary.txt"))

print 'The calculation is done!'
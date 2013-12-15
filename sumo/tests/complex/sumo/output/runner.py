#!/usr/bin/env python
from __future__ import print_function
import os,subprocess,sys
from xml.sax import parse, handler
from collections import defaultdict

class OutputHandler(handler.ContentHandler):
    
    def __init__(self, lanes):
        self.lanes = lanes
        self.speed = dict([(l, defaultdict(dict)) for l in lanes])
        self.intervals = set()

    def startElement(self, name, attrs):
        if "id" in attrs:
            lane = attrs["id"][3:]
        if name == "interval":
            self.interval = (float(attrs["begin"]), float(attrs["end"]))
            self.intervals.add(self.interval)
            if "nVehContrib" in attrs:
                self.speed[lane]["e1"][self.interval] = float(attrs["speed"])
            if "nSamples" in attrs:
                self.speed[lane]["e2"][self.interval] = float(attrs["meanSpeed"])
            if "meanSpeedWithin" in attrs:
                self.speed[lane]["e3"][self.interval] = float(attrs["meanSpeedWithin"])
                if self.speed[lane]["e3"][self.interval] == -1.:
                    self.speed[lane]["e3"][self.interval] = float(attrs["meanSpeed"])
        if name == "edge" and "id" in attrs and attrs["id"]+"_0" in self.lanes and "speed" in attrs:
            self.speed[attrs["id"]+"_0"]["edge"][self.interval] = float(attrs["speed"])
        if name == "lane" and "id" in attrs and attrs["id"] in self.lanes and "speed" in attrs:
            self.speed[attrs["id"]]["lane"][self.interval] = float(attrs["speed"])

def generateDetectorDef(out, freq, enableLoop, laneIDs):
    print('<additional xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="http://sumo-sim.org/xsd/additional_file.xsd">', file=out)
    for laneId in laneIDs:
        if enableLoop:
            print('    <e1Detector id="e1_%s" lane="%s" pos="200" freq="%s" file="detector.xml"/>' % (laneId, laneId, freq), file=out)
        print("""    <e2Detector id="e2_%s" lane="%s" pos="0" length="30000" friendlyPos="true" freq="%s" file="detector.xml"/>
        <e3Detector id="e3_%s" freq="%s" file="detector.xml">
            <detEntry lane="%s" pos="0"/>
            <detExit lane="%s" pos="30000" friendlyPos="true"/>
        </e3Detector>""" % (laneId, laneId, freq, laneId, freq, laneId, laneId), file=out)
    print("""    <edgeData id="dump_15" freq="%s" file="meandataedge.xml" excludeEmpty="true"/>
    <laneData id="dump_15" freq="%s" file="meandatalane.xml" excludeEmpty="true"/>
</additional>""" % (freq, freq), file=out)


def checkOutput(args, withLoop, lanes):
    handler = OutputHandler(lanes)
    for f in ["detector.xml", "meandataedge.xml", "meandatalane.xml"]:
        if os.path.exists(f):
            parse(f, handler)
    for i in sorted(handler.intervals):
        for lane in lanes:
            if withLoop:
                vals = [handler.speed[lane][type].get(i, -1.) for type in ["e1", "e2", "e3", "edge", "lane"]]
            else:
                vals = [handler.speed[lane][type].get(i, -1.) for type in ["e2", "e3", "edge", "lane"]]
            for v in vals[:-1]:
                if abs(v - vals[-1]) > 0.001:
                    print("failed", args, lane, i, vals)
                    return
    print("success", args, lanes)

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', 'bin', 'sumo'))
sumoArgStart = len(sys.argv)
for idx, arg in enumerate(sys.argv):
    if arg[0] == "-":
        sumoArgStart = idx
        break
lanes = sys.argv[1:sumoArgStart]
for stepLength in [".1", "1"]:
    for end in ["51", "100"]:
        args = sys.argv[sumoArgStart:] + ["--step-length", stepLength, "--end", end]
        for freq in [.1, 1, 10, 100]:
            withLoop = freq > 50
            for numLanes in range(1, len(lanes) + 1):
                with open("input_additional.add.xml", 'w') as out:
                    generateDetectorDef(out, freq, withLoop, lanes[:numLanes])
                subprocess.call([sumoBinary, "-c", "sumo.sumocfg"] + args, shell=(os.name=="nt"), stdout=sys.stdout, stderr=sys.stderr)
                sys.stdout.flush()
                checkOutput(args, withLoop, lanes[:numLanes])
                sys.stdout.flush()

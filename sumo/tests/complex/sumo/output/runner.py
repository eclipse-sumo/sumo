#!/usr/bin/env python
import os,subprocess,sys
from xml.sax import parse, handler

class OutputHandler(handler.ContentHandler):
    e1speed = {}
    e2speed = {}
    e3speed = {}
    edgespeed = {}
    lanespeed = {}
    intervals = set()

    def startElement(self, name, attrs):
        if name == "interval":
            self.interval = (float(attrs["begin"]), float(attrs["end"]))
            self.intervals.add(self.interval)
            if "nVehContrib" in attrs:
                self.e1speed[self.interval] = float(attrs["speed"])
            if "meanSpeedWithin" in attrs:
                self.e3speed[self.interval] = float(attrs["meanSpeedWithin"])
                if self.e3speed[self.interval] == -1.:
                    self.e3speed[self.interval] = float(attrs["meanSpeed"])
            if "nSamples" in attrs:
                self.e2speed[self.interval] = float(attrs["meanSpeed"])
        if name == "edge" and "id" in attrs and attrs["id"]=="3/1to2/1" and "speed" in attrs:
            self.edgespeed[self.interval] = float(attrs["speed"])
        if name == "lane" and "id" in attrs and attrs["id"]=="3/1to2/1_0" and "speed" in attrs:
            self.lanespeed[self.interval] = float(attrs["speed"])


sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', 'bin', 'sumo'))
subprocess.call([sumoBinary]+sys.argv[1:], shell=(os.name=="nt"), stdout=sys.stdout, stderr=sys.stderr)
handler = OutputHandler()
for f in ["e1detector.xml", "e2detector.xml", "e3detector.xml", "meandataedge.xml", "meandatalane.xml"]:
    if os.path.exists(f):
        parse(f, handler)

for i in sorted(handler.intervals):
    vals = [handler.e1speed.get(i, -1.), handler.e2speed.get(i, -1.), handler.e3speed.get(i, -1.), handler.edgespeed.get(i, -1.), handler.lanespeed.get(i, -1.)]
    for v in vals[:-1]:
        if abs(v - vals[-1]) > 0.001:
            print i, vals
            break

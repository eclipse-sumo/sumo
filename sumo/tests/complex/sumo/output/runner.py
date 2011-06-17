#!/usr/bin/env python
import os,subprocess,sys
from xml.sax import parse, handler

class OutputHandler(handler.ContentHandler):
    e1speed = -1
    e2speed = -1
    e3speed = -1
    edgespeed = -1
    lanespeed = -1

    def startElement(self, name, attrs):
        if name == "interval":
            if "nVehContrib" in attrs:
                self.e1speed = attrs["speed"]
            if "meanSpeed" in attrs:
                if "nSamples" in attrs:
                    self.e2speed = attrs["meanSpeed"]
                else:
                    self.e3speed = attrs["meanSpeed"]
        if name == "edge" and "id" in attrs and attrs["id"]=="3/1to2/1" and "speed" in attrs:
            self.edgespeed = attrs["speed"]
        if name == "lane" and "id" in attrs and attrs["id"]=="3/1to2/1_0" and "speed" in attrs:
            self.lanespeed = attrs["speed"]


sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(os.path.dirname(sys.argv[0]), '..', '..', '..', '..', 'bin', 'sumo'))
subprocess.call([sumoBinary]+sys.argv[1:], shell=(os.name=="nt"), stdout=sys.stdout, stderr=sys.stderr)
handler = OutputHandler()
for f in ["e1detector.xml", "e2detector.xml", "e3detector.xml", "meandataedge.xml", "meandatalane.xml"]:
    if os.path.exists(f):
        parse(f, handler)

print handler.e1speed, handler.e2speed, handler.e3speed, handler.edgespeed, handler.lanespeed

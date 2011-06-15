#!/usr/bin/env python
import os,subprocess,sys
from xml.sax import parse, handler

class OutputHandler(handler.ContentHandler):
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
parse("e1detector.xml", handler)
parse("e2detector.xml", handler)
parse("e3detector.xml", handler)
parse("meandataedge.xml", handler)
parse("meandatalane.xml", handler)

print handler.e1speed, handler.e2speed, handler.e3speed, handler.edgespeed, handler.lanespeed

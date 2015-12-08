

import cPickle
import sys
from xml.sax import saxutils, make_parser, handler

NOT_FLOAT_TRIPINFO_ATTRS = [ "departLane", "arrivalLane", "devices", "vtype", "vaporized" ]

class TripinfosReader(handler.ContentHandler):
    def __init__(self, toCollect):
        self._values = {}
        self._toCollect = toCollect
        for v in toCollect:
            self._values[v] = {} 

    def startElement(self, name, attrs):
        if name == 'tripinfo':
            id = attrs['id']
            for a in self._toCollect:
                val = attrs[a]
                if a not in NOT_FLOAT_TRIPINFO_ATTRS:
                    try:
                        val = float(val)
                    except:
                        print "%s=%s" % (val, a)
                self._values[a][id] = val

    def get(self, name):
        return self._values[name]

def readTripinfos(inputFile, toCollect):
    parser = make_parser()
    tripinfos = TripinfosReader(toCollect)
    parser.setContentHandler(tripinfos)
    parser.parse(inputFile)
    return tripinfos

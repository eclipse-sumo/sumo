from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
import math

	

# written into the net. All members are "private".
class NetDistrictConnectionCountingHandler(handler.ContentHandler):
	def __init__(self):
		self._currentID = ""
		self._districtSinkNo = {}
		self._districtSourceNo = {}

	def startElement(self, name, attrs):
		if name == 'district':	
			self._currentID = attrs['id']
		elif name == 'dsink':
			if self._currentID in self._districtSinkNo:
				self._districtSinkNo[self._currentID] = self._districtSinkNo[self._currentID] + 1
			else:
				self._districtSinkNo[self._currentID] = 1
		elif name == 'dsource':
			if self._currentID in self._districtSinkNo:
				self._districtSourceNo[self._currentID] = self._districtSourceNo[self._currentID] + 1
			else:
				self._districtSourceNo[self._currentID] = 1


	def writeResults(self, output):
		fd = open(output, "w")
		for district in self._districtSourceNo:
			fd.write(district + ";" + str(self._districtSourceNo[district ]) + ";" + str(self._districtSinkNo[district ]) + "\n")
		fd.close()
				
		

optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-n", "--net-file", dest="netfile",
                     help="read SUMO network(s) from FILE(s) (mandatory)", metavar="FILE")
optParser.add_option("-o", "--output", dest="output",
                     help="read SUMO network(s) from FILE(s) (mandatory)", metavar="FILE")
(options, args) = optParser.parse_args()

parser = make_parser()
reader = NetDistrictConnectionCountingHandler()
parser.setContentHandler(reader)
if options.verbose:
	print "Reading net '" + options.netfile + "'"
parser.parse(options.netfile)
reader.writeResults(options.output)


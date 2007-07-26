from xml.sax import saxutils, make_parser, handler
from optparse import OptionParser
import math


def parseShape(shape):
	poses = shape.split()
	ret = []
	for pos in poses:
		xy = pos.split(",")
		ret.append((float(xy[0]), float(xy[1])))
	return ret


# All members are "private".
class JunctionPositionsReader(handler.ContentHandler):
	def __init__(self):
		self._xPos = {}
		self._yPos = {}

	def startElement(self, name, attrs):
		if name == 'junction':
			self._xPos[attrs['id']] = float(attrs['x'])
			self._yPos[attrs['id']] = float(attrs['y'])

	def getJunctionXPoses(self, junctions1):
		ret = []
		for junction in junctions1:
			ret.append(self._xPos[junction])
		return ret

	def getJunctionYPoses(self, junctions1):
		ret = []
		for junction in junctions1:
			ret.append(self._yPos[junction])
		return ret



# All members are "private".
class DistrictMapper(handler.ContentHandler):
	def __init__(self):
		self._haveDistrict = False
		self._parsingDistrictShape = False
		self._districtShapes = {}
		self._currentID = ""
		self._shape = ""

	def startElement(self, name, attrs):
		if name == 'district':	
			self._haveDistrict = True
			self._currentID = attrs['id']
		elif name == 'shape' and self._haveDistrict:
			self._parsingDistrictShape = True

	def characters(self, content):
		if self._parsingDistrictShape:
			self._shape += content

	def endElement(self, name):
		if name == 'district':	
			self._haveDistrict = False
		elif name == 'shape' and self._haveDistrict:
			self._parsingDistrictShape = False
			if self._shape!='':
				self._districtShapes[self._currentID] = parseShape(self._shape)
				self._shape = ""

	def convertShapes(self, xoff1, xoff2, xscale, yoff1, yoff2, yscale):
		for district in self._districtShapes:
			shape = self._districtShapes[district]
			nshape = []
			for i in range(0, len(shape)):
				nx = ((shape[i][0]-xoff1) * xscale + xoff2)
				ny = ((shape[i][1]-yoff1) * yscale + yoff2)
				nshape.append((nx, ny))
			self._districtShapes[district] = nshape

	def writeResults(self, output, color):
		fd = open(output, "w")
		fd.write("<districts>\n")
		for district in self._districtShapes:
			shape = self._districtShapes[district]
			shapeStr = ""
			for i in range(0, len(shape)):
				if i!=0:
					shapeStr = shapeStr + " "
				shapeStr = shapeStr + str(shape[i][0]) + "," + str(shape[i][1])
			fd.write("   <district id=\"" + district + "\">\n")
			fd.write("      <shape>" + shapeStr + "</shape>\n")
			fd.write("   </district>\n")
			fd.write("   <poly id=\"" + district + "\" color=\"" + color + "\">" + shapeStr + "</poly>\n")
		fd.write("</districts>\n")
		fd.close()


		

optParser = OptionParser()
optParser.add_option("-v", "--verbose", action="store_true", dest="verbose",
                     default=False, help="tell me what you are doing")
optParser.add_option("-1", "--net-file1", dest="netfile1",
                     help="read first SUMO network from FILE (mandatory)", metavar="FILE")
optParser.add_option("-2", "--net-file2", dest="netfile2",
                     help="read second SUMO network from FILE (mandatory)", metavar="FILE")
optParser.add_option("-o", "--output", dest="output",
                     help="write results to FILE (mandatory)", metavar="FILE")
optParser.add_option("-a", "--junctions1", dest="junctions1",
                     default=False, help="Triplet of junctions to use from first network (mandatory)")
optParser.add_option("-b", "--junctions2", dest="junctions2",
                     default=False, help="Triplet of junctions to use from second network (mandatory)")
optParser.add_option("-c", "--color", dest="color",
                     default=False, help="Assign this color to districts (mandatory)")
(options, args) = optParser.parse_args()

parser = make_parser()
if options.verbose:
	print "Reading net#1"
reader1 = JunctionPositionsReader()
parser.setContentHandler(reader1)
parser.parse(options.netfile1)
if options.verbose:
	print "Reading net#2"
reader2 = JunctionPositionsReader()
parser.setContentHandler(reader2)
parser.parse(options.netfile2)

junctions1 = options.junctions1.split(",")
junctions2 = options.junctions2.split(",")
xposes1 = reader1.getJunctionXPoses(junctions1)
yposes1 = reader1.getJunctionYPoses(junctions1)
xposes2 = reader2.getJunctionXPoses(junctions2)
yposes2 = reader2.getJunctionYPoses(junctions2)

xmin1 = min(xposes1)
xmax1 = max(xposes1)
ymin1 = min(yposes1)
ymax1 = max(yposes1)
xmin2 = min(xposes2)
xmax2 = max(xposes2)
ymin2 = min(yposes2)
ymax2 = max(yposes2)

width1 = xmax1 - xmin1
height1 = ymax1 - ymin1
width2 = xmax2 - xmin2
height2 = ymax2 - ymin2

reader = DistrictMapper()
parser.setContentHandler(reader)
parser.parse(options.netfile1)
reader.convertShapes(xmin1, xmin2, width1/width2, ymin1, ymin2, height1/height2)
reader.writeResults(options.output, options.color)


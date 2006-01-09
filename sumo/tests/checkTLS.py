from xml.sax import saxutils
from xml.sax import make_parser
from xml.sax.handler import feature_namespaces
import os
import sys

invalid = 0


class SUMOTLSChecker1(saxutils.DefaultHandler):
	def startElement(self, name, attrs):
		if name != 'phase':
			return
		phase = attrs.get('phase', None)
		brake = attrs.get('brake', None)
		yellow = attrs.get('yellow', None)
		# check for invalid combinations
		for i in range(len(phase)):
			ip = phase[i:i+1]
			ib = brake[i:i+1]
			iy = yellow[i:i+1]
			if ip=='0':
				if ib=='0':
					if iy=='0':
						# may not pass, but does not have to brake--> invalid!!
						print "Invalid combination '000'"
						invalid = invalid + 1
					else:
						# may not pass, but does not have to brake--> invalid!!
						print "Invalid combination '001'"
						invalid = invalid + 1
				else:
					if iy=='0':
						# the vehicle must brake in front of the tls to regard other traffic and must not pass it --> equivalent to a red light
						pass
					else:
						# the vehicle must brake in front of the tls to regard other traffic and must not pass it, but the "yellow" value is set, so if the vehicle is too fast to stop in front, it will be passed --> equivalent to a yellow light
						pass
			else:
				if ib=='0':
					if iy=='0':
						# the vehicle may pass the junction without braking (regarding other traffic) --> equivalent to green light for streams that may pass the junction without regarding other traffic (higher priorised roads)
						pass
					else:
						# invalid combination
						print "Invalid combination '101'"
						invalid = invalid + 1
				else:
					if iy=='0':
						# the vehicle may pass the junction but has also to slw done in front of the tls to let vehicles on roads with a higher priority pass if any --> equivalent to green light for streams that have to regard other traffic (lower priorised roads)
						pass
					else:
						# invalid combination
						print "Invalid combination '111'"
						invalid = invalid + 1


# Create a parser
parser = make_parser()

# Tell the parser we are not interested in XML namespaces
parser.setFeature(feature_namespaces, 0)

# Create the handler
dh = SUMOTLSChecker1()

# Tell the parser to use our handler
parser.setContentHandler(dh)

# Parse the input
parser.parse(sys.argv[1])

if(invalid!=0):
	print str(invalid) + " invalid combinations found in '" + sys.argv[1] + "'"

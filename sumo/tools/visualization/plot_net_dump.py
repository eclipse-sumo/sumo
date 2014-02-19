                    
import os, subprocess, sys, random, helpers
from xml.sax import make_parser
from xml.sax.handler import ContentHandler
from matplotlib import rcParams
from pylab import *
from matplotlib.ticker import FuncFormatter as ff
import matplotlib.pyplot as plt

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'tools'))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(__file__), '..', '..')), 'tools'))
import sumolib.output


class WeightsReader(ContentHandler):
    """Reads the dump file"""
    def __init__(self, value):
        self._edge2value = {}
        self._value = value
        self._intervals = []

    def startElement(self, name, attrs):
        if name == 'interval':
            self._time = float(attrs['begin'])
            self._edge2value[self._time] = {}
            self._intervals.append(self._time)
        if name == 'edge':
            id = attrs['id']
            if attrs.has_key(self._value): self._edge2value[self._time][id] = float(attrs[self._value])
  



def main(args=None):
  """The main function; parses options and plots"""
  ## ---------- build and read options ----------
  from optparse import OptionParser
  optParser = OptionParser()
  optParser.add_option("-n", "--net", dest="net", metavar="FILE",
                         help="Defines the network to read")
  optParser.add_option("-i", "--dump-inputs", dest="dumps", metavar="FILE",
                         help="Defines the dump-output files to use as input")
  optParser.add_option("-m", "--measures", dest="measures", 
                         default="speed,entered", help="Define which measure to plot")
  optParser.add_option("-w", "--default-width", dest="defaultWidth", 
                         type="float", default=.1, help="Defines the default edge width")
  optParser.add_option("-c", "--default-color", dest="defaultColor", 
                         default='k', help="Defines the default edge color")
  optParser.add_option("--min-width", dest="minWidth", 
                         type="float", default=.5, help="Defines the minimum edge width")
  optParser.add_option("--max-width", dest="maxWidth", 
                         type="float", default=3, help="Defines the maximum edge width")
  optParser.add_option("--log-colors", dest="logColors", action="store_true",
                         default=False, help="If set, colors are log-scaled")
  optParser.add_option("--log-widths", dest="logWidths", action="store_true",
                         default=False, help="If set, widths are log-scaled")
  optParser.add_option("--min-color-value", dest="colorMin", 
                         type="float", default=None, help="If set, defines the minimum edge color value")
  optParser.add_option("--max-color-value", dest="colorMax", 
                         type="float", default=None, help="If set, defines the maximum edge color value")
  optParser.add_option("--min-width-value", dest="widthMin", 
                         type="float", default=None, help="If set, defines the minimum edge width value")
  optParser.add_option("--max-width-value", dest="widthMax", 
                         type="float", default=None, help="If set, defines the maximum edge width value")
  optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="If set, the script says what it's doing")
  # standard plot options
  helpers.addInteractionOptions(optParser)
  helpers.addPlotOptions(optParser)
  # parse
  options, remaining_args = optParser.parse_args(args=args)

  if options.net==None: 
    print "Error: a network to load must be given."
    return 1
  if options.verbose: print "Reading network from '%s'" % options.net
  net = sumolib.net.readNet(options.net)
  
  if options.measures==None: 
    print "Error: a dump file must be given."
    return 1
  
  times = []
  hc = None
  if options.dumps.split(",")[0]!="":  
    if options.verbose: print "Reading colors from '%s'" % options.dumps.split(",")[0]
    hc = WeightsReader(options.measures.split(",")[0])
    sumolib.output.parse_sax(options.dumps.split(",")[0], hc)
    times = hc._edge2value
  
  hw = None
  if options.dumps.split(",")[1]!="":  
    if options.verbose: print "Reading widths from '%s'" % options.dumps.split(",")[1]
    hw = WeightsReader(options.measures.split(",")[1])
    sumolib.output.parse_sax(options.dumps.split(",")[1], hw)
    times = hw._edge2value

  for t in times: 
    colors = {}
    maxColorValue = None
    minColorValue = None
    for e in net._id2edge:
      if hc and t in hc._edge2value and e in hc._edge2value[t]: 
        if options.colorMax!=None and hc._edge2value[t][e]>options.colorMax: hc._edge2value[t][e] = options.colorMax
        if options.colorMin!=None and hc._edge2value[t][e]<options.colorMin: hc._edge2value[t][e] = options.colorMin
        if maxColorValue==None or maxColorValue<hc._edge2value[t][e]: maxColorValue = hc._edge2value[t][e] 
        if minColorValue==None or minColorValue>hc._edge2value[t][e]: minColorValue = hc._edge2value[t][e]
        colors[e] = hc._edge2value[t][e] 
    if options.colorMax!=None: maxColorValue = options.colorMax 
    if options.colorMin!=None: minColorValue = options.colorMin 
    if options.logColors: 
      helpers.logNormalise(colors, maxColorValue)
    else:
      helpers.linNormalise(colors, minColorValue, maxColorValue)
    for e in colors:
      colors[e] = helpers.getColor(options, colors[e], 1.)
    if options.verbose: print "Color values are between %s and %s" % (minColorValue, maxColorValue)
  
    widths = {}
    maxWidthValue = None
    minWidthValue = None
    for e in net._id2edge:
      if hw and t in hw._edge2value and e in hw._edge2value[t]:
        v = abs(hw._edge2value[t][e]) 
        if options.widthMax!=None and v>options.widthMax: v = options.widthMax
        if options.widthMin!=None and v<options.widthMin: v = options.widthMin
        if not maxWidthValue or maxWidthValue<v: maxWidthValue = v 
        if not minWidthValue or minWidthValue>v: minWidthValue = v
        widths[e] = v
    if options.widthMax!=None: maxWidthValue = options.widthMax 
    if options.widthMin!=None: minWidthValue = options.widthMin 
    if options.logWidths: 
      helpers.logNormalise(widths, options.colorMax)
    else:
      helpers.linNormalise(widths, minWidthValue, maxWidthValue)
    for e in widths:
      widths[e] = options.minWidth + widths[e] * (options.maxWidth-options.minWidth)
    if options.verbose: print "Width values are between %s and %s" % (minWidthValue, maxWidthValue)
  
    fig, ax = helpers.openFigure(options)
    ax.set_aspect("equal", None, 'C')
    helpers.plotNet(net, colors, widths, options)
    
    
    # drawing the legend, at least for the colors
    sm = matplotlib.cm.ScalarMappable(cmap=get_cmap(options.colormap), norm=plt.normalize(vmin=minColorValue, vmax=maxColorValue))
    # "fake up the array of the scalar mappable. Urgh..." (pelson, http://stackoverflow.com/questions/8342549/matplotlib-add-colorbar-to-a-sequence-of-line-plots)
    sm._A = []
    plt.colorbar(sm)
    
    options.nolegend = True
    helpers.closeFigure(fig, ax, options)
    return 0 # !!! hack 
  return 0

if __name__ == "__main__":
  sys.exit(main(sys.argv))
    
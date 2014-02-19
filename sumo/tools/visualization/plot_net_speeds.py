                    
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




def main(args=None):
  """The main function; parses options and plots"""
  ## ---------- build and read options ----------
  from optparse import OptionParser
  optParser = OptionParser()
  optParser.add_option("-n", "--net", dest="net", metavar="FILE",
                         help="Defines the network to read")
  optParser.add_option("--edge-width", dest="defaultWidth", 
                         type="float", default=1, help="Defines the edge width")
  optParser.add_option("--edge-color", dest="defaultColor", 
                         default='k', help="Defines the edge color")
  optParser.add_option("--minV", dest="minV", 
                         type="float", default=None, help="Define the minimum value boundary")
  optParser.add_option("--maxV", dest="maxV", 
                         type="float", default=None, help="Define the maximum value boundary")
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

  speeds = {}
  minV = None
  maxV = None
  for e in net._id2edge:
    v = net._id2edge[e]._speed
    if minV==None or minV>v:
        minV = v
    if maxV==None or maxV<v:
        maxV = v
    speeds[e] = v
  if options.minV!=None: minV = options.minV 
  if options.maxV!=None: maxV = options.maxV 
  #if options.logColors: 
#    helpers.logNormalise(colors, maxColorValue)
#  else:
#    helpers.linNormalise(colors, minColorValue, maxColorValue)
  
  helpers.linNormalise(speeds, minV, maxV)
  for e in speeds:
    speeds[e] = helpers.getColor(options, speeds[e], 1.)
  fig, ax = helpers.openFigure(options)
  ax.set_aspect("equal", None, 'C')
  helpers.plotNet(net, speeds, {}, options)

  # drawing the legend, at least for the colors
  print "%s -> %s" % (minV, maxV)
  sm = matplotlib.cm.ScalarMappable(cmap=get_cmap(options.colormap), norm=plt.normalize(vmin=minV, vmax=maxV))
  # "fake up the array of the scalar mappable. Urgh..." (pelson, http://stackoverflow.com/questions/8342549/matplotlib-add-colorbar-to-a-sequence-of-line-plots)
  sm._A = []
  plt.colorbar(sm)
  options.nolegend = True
  helpers.closeFigure(fig, ax, options)


if __name__ == "__main__":
  sys.exit(main(sys.argv))
    
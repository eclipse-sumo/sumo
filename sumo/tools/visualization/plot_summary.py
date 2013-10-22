                    
import os, subprocess, sys, random, helpers
from matplotlib import rcParams
from pylab import *
from matplotlib.ticker import FuncFormatter as ff

sys.path.append(os.path.join(os.path.dirname(__file__), '..', '..', 'tools'))
sys.path.append(os.path.join(os.environ.get("SUMO_HOME", os.path.join(os.path.dirname(__file__), '..', '..')), 'tools'))
import sumolib.output


  
def readValues(files, verbose, measure):
  ret = {}
  for f in files:
    if verbose: print "Reading '%s'..." % f
    ret[f] = sumolib.output.parse_sax__asList(f, "step", [measure])
  return ret



def main(args=None):
  """The main function; parses options and plots"""
  ## ---------- build and read options ----------
  from optparse import OptionParser
  optParser = OptionParser()
  optParser.add_option("-i", "--summary-inputs", dest="summary", metavar="FILE",
                         help="Defines the summary-output files to use as input")
  optParser.add_option("-v", "--verbose", dest="verbose", action="store_true",
                         default=False, help="If set, the script says what it's doing")
  optParser.add_option("-m", "--measure", dest="measure", 
                         default="running", help="Define which measure to plot")
  # standard plot options
  helpers.addInteractionOptions(optParser)
  helpers.addPlotOptions(optParser)
  # parse
  options, remaining_args = optParser.parse_args(args=args)
  
  if options.summary==None:
    print "Error: at least one summary file must be given"
    sys.exit(1)

  minV = 0
  maxV = 0
  files = options.summary.split(",")
  nums = readValues(files, options.verbose, options.measure) 
  for f in files:
    maxV = max(maxV, len(nums[f]))
  ts = range(minV, maxV+1)

  fig, ax = helpers.openFigure(options)
  for i,f in enumerate(files):
    v = sumolib.output.toList(nums[f], options.measure)
    c = helpers.getColor(options, i, len(files))
    l = helpers.getLabel(f, i, options)
    plot(ts[0:len(v)], v, label=l, color=c)
  helpers.closeFigure(fig, ax, options)

if __name__ == "__main__":
  sys.exit(main(sys.argv))
    
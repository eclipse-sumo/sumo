import sys
import sumolib.output
import helpers
from pylab import *


f = {}
pd = sumolib.output.parse(sys.argv[1], "vehicle")
for v in pd:
  t = int(float(v.depart))
  e = v["route"][0].edges.split(" ")[0]
  if e not in f:
    f[e] = [0] * 86400
  f[e][t] = f[e][t] + 1


AGG = 3600
fa = {}
for e in f:
  fa[e] = [0] * (86400/AGG)
  for th in range(0, 86400/AGG):
    for tl in range(0, AGG):
      fa[e][th] = fa[e][th] + f[e][th*AGG+tl]
ts = range(0, 86400/AGG)



for e in f:
  plot(ts, fa[e], 'o-', label=e)   
legend()
show()


import sys, os, math
import numpy as np
from scipy.optimize import fmin
from scipy.optimize import fmin_cobyla

dDay = 1
fpLog = open('all-the-results.txt','w')

# definition of gof() function to be given to fmin_cobyla() or fmin() 
def gof(p):
	f = open('sumo-parameters.txt','w')
	print >> f, 'vMax %s'%p[0]
	print >> f, 'aMax %s'%p[1]
	print >> f, 'bMax %s'%p[2]
	print >> f, 'lCar %s'%p[3]
	print >> f, 'sigA %s'%p[4]
	print >> f, 'tTau %s'%p[5]
	f.close()
	pstr = '{0:.3f} '.format(p[0])
	for i in range(1,len(p)):
		pstr += '{0:.3f} '.format(p[i])
	print '# simulation with: '+pstr
	exec(compile(open('validate.py').read(), 'validate.py', 'exec'))
	f = open('sumo-rmse.txt','r')
	str = f.readline()
	f.close()
	result = float(str)
	print '#### yields rmse: '+'{0:.4f} '.format(result)
	print >> fpLog, "%s %s" % (pstr, result)
	fpLog.flush()
	return result

# defining all the constraints
def conVmax(params):
# vMax < 25
	return 25.0 - params[0]
def conTtau(params):
# tTau > 1.1
	return params[5] - 1.1
def conSigA(params):
# sigA > 0.1
	return params[4] - 0.1
def conSigA2(params):
# sigA < 1.0
	return 1.0 - params[4]
def conAmax(params):
# aMax > 0.1
	return params[1] - 0.1
	
params = [22.0, 2.0, 2.0, 7.5, 0.5, 1.5]
# call to (unconstrained) Nelder Mead; does not work correctly, because 
# method very often stumples over unrealistic input parameters (like tTau<1),
# which causes SUMO to behave strangely.
# fmin(gof, params)
fmin_cobyla(gof, params, [conVmax, conAmax, conTtau, conSigA, conSigA2], rhoend=1.0e-4)

fpLog.close()

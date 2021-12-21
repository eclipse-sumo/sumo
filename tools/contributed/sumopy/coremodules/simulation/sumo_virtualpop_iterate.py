# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2021 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2021 University of Bologna - DICAM
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    sumo_virtualpop_iterate.py
# @author  Joerg Schweizer
# @date   2012

"""
Simulates <n_iter> times the <scenariofile>, calling
script <simscriptfile> before each simulation run.

Usage:
python sumo_virtualpop_iterate.py  <scenariofile> <n_iter> <simscriptfile>

"""

import sys
import os
import subprocess
import time


def start_iterations(scenariofilepath, n_iter, simscriptfilepath):
    print 'sumo_virtualpop_iterate.run', scenariofilepath
    cmlfilepath = os.path.join(os.path.dirname(simscriptfilepath), 'sumo_virtualpop_cml.bash')

    P = '"'
    if os.path.isfile(cmlfilepath):
        os.remove(cmlfilepath)

    cml_script = 'python '+P+simscriptfilepath+P+' '+P+scenariofilepath+P+' '+cmlfilepath

    for i in range(1, n_iter+1):

        print '  Start preparation of iteration %d.' % i
        proc = subprocess.Popen(cml_script, shell=True)
        proc.wait()
        if proc.returncode == 0:
            print '  Preparation of iteration %d successful' % i
            f = open(cmlfilepath, "r")
            cml = f.readline()
            f.close()

            # time.sleep(10)

            print '  Start SUMO microsimulator'
            print '  cml=', cml
            proc = subprocess.Popen(cml, shell=True)
            proc.wait()
            if proc.returncode == 0:
                print '  Microsimulation of iteration %d successful.' % i
            else:
                print '  Error in microsimulation of iteration %d.' % i
        else:
            print '  Error in preparation of iteration %d successful' % i
    print '  Start preparation of iteration %d.' % i
    proc = subprocess.Popen(cml_script, shell=True)
    proc.wait()
    if proc.returncode == 0:
        print 'Save last results.'
        f = open(cmlfilepath, "r")
        cml = f.readline()
        f.close()
    else:
        print'error on the last data backup'

        # time.sleep(10)


if __name__ == '__main__':
    try:
        APPDIR = os.path.dirname(os.path.abspath(__file__))
    except:
        APPDIR = os.path.dirname(os.path.abspath(sys.argv[0]))

    scenariofilepath = os.path.abspath(sys.argv[1])
    n_iter = int(sys.argv[2])
    if len(sys.argv) > 3:
        simscriptfilepath = sys.argv[3]
    else:
        simscriptfilepath = os.path.join(APPDIR, 'sumo_virtualpop.py')

    start_iterations(scenariofilepath, n_iter, simscriptfilepath)

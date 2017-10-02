# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2017 German Aerospace Center (DLR) and others.
# SUMOPy module
# Copyright (C) 2012-2017 University of Bologna - DICAM
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    logger.py
# @author  Joerg Schweizer
# @date    
# @version $Id$

import types


class Logger:

    def __init__(self, filepath=None, is_stdout=True):
        self._filepath = filepath
        self._logfile = None
        self._callbacks = {}
        self._is_stdout = is_stdout

    def start(self, text=''):
        if self._filepath != None:
            self._logfile = open(self._filepath, 'w')
            self._logfile.write(text + '\n')
        else:
            self._logfile = None
            print text

    def add_callback(self, function, key='message'):
        self._callbacks[key] = function

    def progress(self, percent):
        pass

    def w(self, data, key='message', **kwargs):
        # print 'w:',data,self._callbacks
        if self._logfile != None:
            self._logfile.write(str(data) + '\n')
        elif self._callbacks.has_key(key):
            kwargs['key'] = key
            self._callbacks[key](data, **kwargs)
        # elif type(data)==types.StringType:
        #    print data
        if self._is_stdout:
            print str(data)

    def stop(self, text=''):
        if self._logfile != None:
            self._logfile.write(text + '\n')
            self._logfile.close()
            self._logfile = None

        else:
            print text

# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2023 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    step.py
# @author  Michael Behrisch
# @author  Lena Kalleske
# @author  Mario Krumnow
# @author  Daniel Krajzewicz
# @author  Jakob Erdmann
# @date    2008-10-09

from __future__ import print_function
from __future__ import absolute_import
import warnings
import abc
from functools import wraps


class StepManager:

    """Contains the step listeners and the tracing management.
    """

    def __init__(self):
        self._stepListeners = {}
        self._nextStepListenerID = 0
        self._traceFile = None

    def manageStepListeners(self, step):
        listenersToRemove = []
        for listenerID, listener in self._stepListeners.items():
            keep = listener.step(step)
            if not keep:
                listenersToRemove.append(listenerID)
        for listenerID in listenersToRemove:
            self.removeStepListener(listenerID)

    def addStepListener(self, listener):
        """addStepListener(traci.StepListener) -> int

        Append the step listener (its step function is called at the end of every call to traci.simulationStep())
        Returns the ID assigned to the listener if it was added successfully, None otherwise.
        """
        if issubclass(type(listener), StepListener):
            listener.setID(self._nextStepListenerID)
            self._stepListeners[self._nextStepListenerID] = listener
            self._nextStepListenerID += 1
            # print ("traci: Added stepListener %s\nlisteners: %s"%(_nextStepListenerID - 1, _stepListeners))
            return self._nextStepListenerID - 1
        warnings.warn(
            "Proposed listener's type must inherit from traci.StepListener. Not adding object of type '%s'" %
            type(listener))
        return None

    def removeStepListener(self, listenerID):
        """removeStepListener(traci.StepListener) -> bool

        Remove the step listener from traci's step listener container.
        Returns True if the listener was removed successfully, False if it wasn't registered.
        """
        # print ("traci: removeStepListener %s\nlisteners: %s"%(listenerID, _stepListeners))
        if listenerID in self._stepListeners:
            self._stepListeners[listenerID].cleanUp()
            del self._stepListeners[listenerID]
            # print ("traci: Removed stepListener %s"%(listenerID))
            return True
        warnings.warn("Cannot remove unknown listener %s.\nlisteners:%s" % (listenerID, self._stepListeners))
        return False

    def startTracing(self, traceFile, traceGetters, domains):
        result = False
        if self._traceFile is None:
            result = True
            for domain in domains:
                for attrName in dir(domain):
                    if not attrName.startswith("_"):
                        attr = getattr(domain, attrName)
                        if (callable(attr)
                                and attrName not in [
                                    "wrapper",
                                    "getAllSubscriptionResults",
                                    "getAllContextSubscriptionResults",
                                    "removeStages",
                        ]
                                and not attrName.endswith('makeWrapper')
                                and (traceGetters or not attrName.startswith("get"))):
                            domainName = None
                            if hasattr(domain, "__name__"):
                                domainName = domain.__name__
                            if hasattr(domain, "_name"):
                                domainName = domain._name
                            setattr(domain, attrName, self._addTracing(attr, domainName))
        self._traceFile = open(traceFile, 'w')
        return result

    def _addTracing(self, method, domain=None):
        if domain:
            name = "%s.%s" % (domain, method.__name__)
        else:
            name = method.__name__

        @wraps(method)
        def tracingWrapper(*args, **kwargs):
            if self._traceFile is not None and not self._traceFile.closed:
                kwargRepr = ["%s=%s" % (n, repr(v)) for n, v in kwargs.items()]
                self.write(name, ', '.join(list(map(repr, args)) + kwargRepr))
            return method(*args, **kwargs)
        return tracingWrapper

    def write(self, method, args=""):
        self._traceFile.write("traci.%s(%s)\n" % (method, args))

    def close(self, write=False):
        if self._traceFile is not None and not self._traceFile.closed:
            if write:
                self.write("close")
            self._traceFile.close()


class StepListener:
    __metaclass__ = abc.ABCMeta

    @abc.abstractmethod
    def step(self, t=0):
        """step(int) -> bool

        After adding a StepListener 'listener' with traci.addStepListener(listener),
        TraCI will call listener.step(t) after each call to traci.simulationStep(t)
        The return value indicates whether the stepListener wants to stay active.
        """
        return True

    def cleanUp(self):
        """cleanUp() -> None

        This method is called at removal of the stepListener, allowing to schedule some final actions
        """
        pass

    def setID(self, ID):
        self._ID = ID

    def getID(self):
        return self._ID

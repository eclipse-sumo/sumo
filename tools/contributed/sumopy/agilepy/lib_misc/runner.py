# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2016-2023 German Aerospace Center (DLR) and others.
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

# @file    runner.py
# @author  Joerg Schweizer
# @date    2012


class Runner:
    """Class to manage the execution of processes in a script.
    The class initialized with the following list of tasks:
    [
    ('taskname1', True|False),
    ('taskname2', True|False),
    :
    :
    ]
    The first argument is the task name, the second is whether the task should be executed or not.

    """

    def __init__(self, tasks):
        self._tasknames = []
        self._taskinfos = []
        self._taskindex = -1
        for taskname, info in tasks:
            self._tasknames.append(taskname)
            self._taskinfos.append(info)

    def has_task(self, taskname):
        """Returns True if taskname must be executed"""
        if self._tasknames.count(taskname) > 0:
            ind = self._tasknames.index(taskname)

            # check if info foresees this task
            if self._taskinfos[ind]:  # could be modified in the future
                self._taskindex = ind
                return True

            else:
                return False

        else:
            return False

    def get_index_current(self):
        """
        Returns index of currently executed task
        """
        return self._taskindex

    def format_task_current(self):
        # print 'format_task_current',self._taskindex,self._tasknames
        return "%03d_%s" % (self._taskindex, self._tasknames[self._taskindex])

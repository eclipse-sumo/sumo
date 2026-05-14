# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
# Copyright (C) 2016-2026 German Aerospace Center (DLR) and others.
# hybridPY module
# Copyright (C) 2012-2026 University of Bologna - DICAM, Technical University of Munich
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    petrinet.py
# @author  Joerg Schweizer
# @author  Fabian Schuhmann
# @author  Ngoc An Nguyen
# @author  Cristian Poliziani
# @date    2012

import numpy as np


class PetrinetMixin:
    """
    Some basic methods to facilitate Petrinet simulation.
    """

    def init_petrinet(self, is_save=False):
        self._places = list()
        self._placeattrnames = list()
        self._placeshortnames = list()
        self._transitions = list()
        self._transitionattrnames = list()
        self._is_save_petrinet = is_save
        if not is_save:
            self.do_not_save_attrs(['_places', '_placeattrnames', '_placeshortnames',
                                   '_transitions', '_transitionattrnames', '_is_save_petrinet'])

    def add_place(self, attrname, default=None, shortname=''):
        print('add_place', attrname)  # ,self._placeshortnames

        self._placeattrnames.append(attrname)

        if shortname == '':
            shortname = attrname
        self._placeshortnames.append(shortname)

        setattr(self, attrname, default)
        # print '   getattr',getattr(self,attrname)
        self._places.append(getattr(self, attrname))

        if not self._is_save_petrinet:
            self.do_not_save_attrs([attrname])

    def add_places(self, placedata):
        """ placedata = [[attrname1, default1, shortname1],[attrname2, default2, shortname2],...]"""
        # print 'add_places',len(placedata)
        for attrname, default, shortname in placedata:
            self.add_place(attrname, default, shortname)

    def add_transition(self, methodname,  shortname=''):
        """Transition method itselved must be defined under the self class"""
        print('add_transition', methodname)
        if shortname == '':
            shortname = methodname
        self._transitionattrnames.append(methodname)
        self._transitions.append(getattr(self, methodname))

    def add_transitions(self, transitiondata):
        """ transitiondata = [methodname1,methodname2,],...]"""
        for methodname in transitiondata:
            self.add_transition(methodname)

    def iterate_transitions(self):
        # execute transitions
        for transition in self._transitions:
            transition()

    def print_places(self, ids=None):

        if ids is None:
            # probe length of place vector
            n = len(self._places[0])
            ids = np.arange(n)
        print(79*'_')
        # print '  self._placeshortnames',self._placeshortnames
        line = '  id '
        for placename in self._placeshortnames:
            line += ',%06s' % placename
        print(line)

        for _id in ids:
            line = '%04sB' % _id
            for place in self._places:
                if place[_id] < 0:
                    p = ''
                else:
                    p = place[_id]

                line += ',%06s' % p

            print(line)

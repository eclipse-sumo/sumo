# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2022 German Aerospace Center (DLR) and others.
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# https://www.eclipse.org/legal/epl-2.0/
# This Source Code may also be made available under the following Secondary
# Licenses when the conditions for such availability set forth in the Eclipse
# Public License 2.0 are satisfied: GNU General Public License, version 2
# or later which is available at
# https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
# SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later

# @file    _frictioncoefficientsign.py
# @author  Matthias Schwamborn
# @date    2020-08-18

from __future__ import absolute_import
from . import constants as tc
from .domain import Domain


class FrictionCoefficientSignDomain(Domain):

    def __init__(self):
        Domain.__init__(self, "frictioncoefficientsign",
                        tc.CMD_GET_FRICTIONCOEFFICIENT_VARIABLE, tc.CMD_SET_FRICTIONCOEFFICIENT_VARIABLE,
                        tc.CMD_SUBSCRIBE_FRICTIONCOEFFICIENT_VARIABLE, tc.RESPONSE_SUBSCRIBE_FRICTIONCOEFFICIENT_VARIABLE,
                        tc.CMD_SUBSCRIBE_FRICTIONCOEFFICIENT_CONTEXT, tc.RESPONSE_SUBSCRIBE_FRICTIONCOEFFICIENT_CONTEXT)

    def getLanes(self, cofID):
        """getLanes(string) -> list(string)

        Returns a list of all lanes controlled by the frictioncoefficientsign.
        """
        return self._getUniversal(tc.VAR_LANES, cofID)

# -*- coding: utf-8 -*-
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2011-2017 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html

# @file    collectinghandler.py
# @author  Karol Stosiek
# @date    2011-10-26
# @version $Id$

from __future__ import absolute_import

import logging


class CollectingHandler(logging.Handler):

    """ Handler for loggers from logging module. Collects all log messages. """

    def __init__(self, level=0):
        """ Constructor. The level parameter stands for logging level. """

        self.log_records = []
        logging.Handler.__init__(self, level)

    def handle(self, record):
        """ See logging.Handler.handle(self, record) docs. """

        self.log_records.append(record)

    def emit(self, record):
        """ See logging.Handler.emit(self, record) docs. """

        pass  # do not emit the record. Other handlers can do that.

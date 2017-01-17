# -*- coding: utf-8 -*-
"""
@file    collectinghandler.py
@author  Karol Stosiek
@date    2011-10-26
@version $Id$

Handler for loggers from logging module. Collects all log messages.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2011-2017 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
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

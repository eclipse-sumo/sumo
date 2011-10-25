""" Handler for loggers from logging module. Collects all log messages. """

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

        pass # do not emit the record. Other handlers can do that. 


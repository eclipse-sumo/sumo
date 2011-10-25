#!/usr/bin/python
# -*- encoding: utf8 -*-

""" Generates turn definitions XML file based on
    connections XML file. """

import connections
import turndefinitions
import logging
import optparse

LOGGER = logging.getLogger(__name__)

if __name__ == "__main__":

    # pylint: disable-msg=C0103

    logging.basicConfig(level="INFO")

    option_parser = optparse.OptionParser()
    option_parser.add_option("-c", "--connections-file",
        dest="connections_file",
        help="Read connections defined in CONNECTIONS_FILE. Mandatory.",
        metavar="CONNECTIONS_FILE")
    option_parser.add_option("-t", "--turn-definitions-file",
        dest="turn_definitions_file",
        help="Write the resulting turn definitions to TURN_DEFINITIONS_FILE. "
             "Mandatory.",
        metavar="TURN_DEFINITIONS_FILE")

    (options, args) = option_parser.parse_args()

    if options.connections_file is None:
        LOGGER.fatal("Missing CONNECTIONS_FILE.\n" +
            option_parser.format_help())
        exit()

    if options.turn_definitions_file is None:
        LOGGER.fatal("Missing TURN_DEFINITIONS_FILE.\n" +
            option_parser.format_help())
        exit()

    connections_file = open(options.connections_file, "r")
    turn_definitions_file = open(options.turn_definitions_file, "w")

    connections = connections.from_stream(connections_file)
    turn_definitions = turndefinitions.from_connections(connections)
    turn_definitions_xml = turndefinitions.to_xml(turn_definitions)
    turn_definitions_file.write(turn_definitions_xml)

    connections_file.close()
    turn_definitions_file.close()


#ifndef help_h
#define help_h

char *help[] = {
    "Usage: sumo-router [OPTION]*",
    "Simulation of Urban MObility v0.7 - Routing Module",
    "  http://sumo.sourceforge.net",
    "Examples: ",
    " sumo-",
    " sumo-router -c routes_munich.cfg",
    " ",
    "Options:",
    " Input files:",
    "   -n, --node-file FILE           FILE contains node definitions ",
    "   -e, --edge-file FILE           FILE contains edge definitions ",
    "   -t, --type-file FILE           FILE contains type definitions ",
    "   -c, --configuration FILE       FILE will be used as configuration",
    "                                    1. Default: ./sumo-netcovert.cfg",
    " ",
    " Output files:",
    "   -o, --output-file FILE         The generated net will be written to FILE",
    " ",
    " Setting Defaults:",
    "   -T, --type NAME                The default name for an edges type",
    "   -L, --lanenumber INT           The default number of lanes in an edge",
    "   -S, --speed DOUBLE             The default speed on an edge (in m/s)",
    "   -P, --priority INT             The default priority of an edge",
    " ",
    " Processing Options:",
    "   -C, --no-config                No configuration file is used",
    "   -v, --verbose                  SUMO-Netconvert reports what it does",
    "   -p, --print-options            Prints option values before processing",
    "   --help                         This screen",
    0 };

#endif

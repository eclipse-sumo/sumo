#ifndef help_h
#define help_h

char *help[] = {
    "Usage: sumo [OPTION]*",
    "Simulation of Urban MObility v0.7 - Simulation Module",
    "  http://sumo.sourceforge.net",
    "Examples: ",
    "sumo -b 0 -e 1000 -n net.xml -r routes.xml",
    "sumo -c munich_config.cfg",
    " ",
    "Options:",
    " Input files:",
    "   -n, --net-files FILE[;FILE]*    FILE is a network file",
    "   -r, --route-files FILE[;FILE]*  FILE is a route file",
    "   -r, --detectors FILE[;FILE]*    FILE is a detector file",
    "   -s, --source-files FILE[;FILE]* FILE is a source file",
    "   -j, --junctions FILE[;FILE]*    FILE should contain the junction logics",
    "   -c, --configuration FILE        FILE will be used as configuration",
    "                                    1. Default: ./sumo.cfg",
    " ",
    " Output files:",
    "   -o, --output-file FILE          FILE is the raw output destination file",
    "                                    Default: stdout",
    " ",
    " Simulation timing:",
    "   -b, --begin INT                 First time step of the simulation",
    "   -e, --end INT                   Last time step of the simulation",
    " ",
    " Processing Options:",
    "   -R, --no-raw-output             No raw output will be generated",
    "   -C, --no-config                 No configuration file will be used",
    "   -v, --verbose                   SUMO will report what it does",
    "   -p, --print-options             Prints option values before processing",
    "   --help                          This screen",
    0 };

#endif

/****************************************************************************/
/// @file    tracitestclient_main.h
/// @author  Friedemann Wesner <wesner@itm.uni-luebeck.de>
/// @date    2008/04/07
/// @version $Id$
///
/// Main method for TraCITestClient
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>
#include <cstdlib>
#include "TraCITestClient.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace testclient;


// ===========================================================================
// method definitions
// ===========================================================================
int main(int argc, char* argv[]) {
    std::string defFile = "";
    std::string outFileName = "testclient_out.txt";
    int port = -1;
    std::string host = "localhost";
    TraCITestClient* client;

    if ((argc == 1) || (argc % 2 == 0)) {
        std::cout << "Usage: TraciTestClient -def <definition_file>  -p <remote port>"
                  << "[-h <remote host>] [-o <outputfile name>]" << std::endl;
        return 0;
    }

    for (int i=1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.compare("-def") == 0) {
            defFile = argv[i+1];
            i++;
        } else if (arg.compare("-o") == 0) {
            outFileName = argv[i+1];
            i++;
        } else if (arg.compare("-p") == 0) {
            port = atoi(argv[i+1]);
            i++;
        } else if (arg.compare("-h") == 0) {
            host = argv[i+1];
            i++;
        } else {
            std::cout << "unknown parameter: " << argv[i] << std::endl;
            return 1;
        }
    }

    if (port == -1) {
        std::cout << "Missing port" << std::endl;
        return 1;
    }
    if (defFile.compare("") == 0) {
        std::cout << "Missing definition file" << std::endl;
        return 1;
    }

    client = new TraCITestClient(outFileName);
    bool success = client->run(defFile, port, host);
    delete client;

    return !success;
}

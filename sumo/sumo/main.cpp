/***************************************************************************
                          main.cpp
			  The main procedure for the conversion /
			  building of networks
                             -------------------
    project              : SUMO
    subproject           : simulation
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.9  2002/07/31 17:42:10  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.12  2002/07/22 12:52:23  dkrajzew
// Source handling added
//
// Revision 1.11  2002/07/11 07:30:43  dkrajzew
// Option_FileName invented to allow relative path names within the configuration files; two not yet implemented parameter introduced
//
// Revision 1.10  2002/07/02 12:48:10  dkrajzew
// --help now does not require -c
//
// Revision 1.9  2002/07/02 08:16:19  dkrajzew
// Program flow changed to allow better options removal; return values corrected
//
// Revision 1.8  2002/06/17 15:57:43  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.7  2002/05/14 07:43:51  dkrajzew
// _SPEEDCHECK-methods moved completely to MSNet
//
// Revision 1.6  2002/04/29 13:52:07  dkrajzew
// the program flow, especially the handling of errors improved
//
// Revision 1.5  2002/04/18 06:04:53  dkrajzew
// Forgotten test switch removed
//
// Revision 1.4  2002/04/17 11:20:40  dkrajzew
// Windows-carriage returns removed
//
// Revision 1.3  2002/04/16 12:21:13  dkrajzew
// Usage of SUMO_DATA removed
//
// Revision 1.2  2002/04/15 06:55:47  dkrajzew
// new loading paradigm implemented
//
// Revision 2.6  2002/03/20 08:13:54  dkrajzew
// help-output added
//
// Revision 2.5  2002/03/15 12:45:49  dkrajzew
// Warning is set to true forever due to bugs in value testing when no warnings are used (will be fixed later)
//
// Revision 2.4  2002/03/14 08:09:13  traffic
// Option for no raw output added
//
// Revision 2.3  2002/03/07 07:54:43  traffic
// implemented the usage of stdout as the default raw output
//
// Revision 2.2  2002/03/06 10:12:17  traffic
// Enviroment variable changef from SUMO to SUMO_DATA
//
// Revision 2.1  2002/03/05 14:51:25  traffic
// SegViolation on unset path debugged
//
// Revision 2.0  2002/02/14 14:43:11  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:35:33  croessel
// Merging sourceForge with tesseraCVS.
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "microsim/MSNet.h"
#include "microsim/MSEmitControl.h"
#include "netload/NLNetBuilder.h"
#include <iostream>
#include <fstream>
#include "netload/SErrorHandler.h"
//#include <iomanip>
#include "utils/Option.h"
#include "utils/OptionsCont.h"
#include "utils/OptionsIO.h"
#include "utils/UtilExceptions.h"
#include "utils/FileHelpers.h"
#include "utils/HelpPrinter.h"
#include "utils/XMLSubSys.h"
#include "help.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * functions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */
/**
 * checkSettings
 * Checks the build settings. The following constraints must be valid:
 * - the network-file was specified (otherwise no simulation is existing)
 * - a junction folder must be given
 *   (otherwise no junctions can be loaded)
 * - the begin and the end of the simulation must be given
 * Returns true when all constraints are valid
 */
bool
checkSettings(OptionsCont *oc) {
    bool ok = true;
    try {
      if(oc!=0) oc->resetDefaults();
      // check the existance of a name for simulation file
      if(!oc->isSet("n")) {
        cout << "Error: No simulation file (-n) specified." << endl;
        ok = false;
      }
      // check if the begin and the end of the simulation are supplied
      if(!oc->isSet("b")) {
        cout << "Error: The begin of the simulation (-b) is not specified." << endl;
        ok = false;
      }
      if(!oc->isSet("e")) {
        cout << "Error: The end of the simulation (-e) is not specified." << endl;
        ok = false;
      }
    } catch (InvalidArgument &e) {
      cout << e.msg() << endl;
      return false;
    }
    return ok;
}

/**
 * getSettings
 * Builds the container of options and parses the configuration file
 * and the command line arguments using it. After this, the settings are
 * validated using "checkSettings".
 * Returns 0 when something failed, otherwise the build OptionsCont.
 */
OptionsCont *
getSettings(int argc, char **argv)
{
    OptionsCont *oc;
	oc = new OptionsCont();
    // register the file i/o options
    oc->doRegister("net-files", 'n', new Option_FileName());
    oc->doRegister("route-files", 'r', new Option_FileName());
    oc->doRegister("junction-files", 'j', new Option_FileName());
    oc->doRegister("detector-files", 'd', new Option_FileName());
    oc->doRegister("output-file", 'o', new Option_FileName(""));
    oc->doRegister("configuration-file", 'c', new Option_FileName("sumo.cfg"));
    oc->doRegister("source-files", 's', new Option_FileName());
    oc->addSynonyme("net-files", "net");
    oc->addSynonyme("route-files", "routes");
    oc->addSynonyme("junction-files", "junctions");
    oc->addSynonyme("detector-files", "detectors");
    oc->addSynonyme("output-file", "output");
    oc->addSynonyme("configuration-file", "configuration");
    oc->addSynonyme("source-files", "sources");
    // register the simulation settings
    oc->doRegister("begin", 'b', new Option_Long());
    oc->doRegister("end", 'e', new Option_Long());
    // register the report options
    oc->doRegister("verbose", 'v', new Option_Bool(false));
    oc->doRegister("warn", 'w', new Option_Bool(true));
    oc->doRegister("print-options", 'p', new Option_Bool(false));
    oc->doRegister("help", new Option_Bool(false));
//    oc->doRegister("validate-nodes", new Option_Bool(false));
    // register some research options
    oc->doRegister("initial-density", new Option_Float());
    oc->doRegister("initial-speed", new Option_Float());
    // register the data processing options
    oc->doRegister("no-config", 'C', new Option_Bool(false));
    oc->addSynonyme("no-config", "no-configuration");
    oc->doRegister("no-raw", 'R', new Option_Bool(false));
    oc->addSynonyme("no-raw", "no-raw-output");
    // parse the command line arguments and configuration the file
    if(OptionsIO::getOptions(oc, argc, argv)) {
        if(oc->getBool("help"))
            return oc;
        if(oc->getBool("p"))
            cout << *oc;
        if(!checkSettings(oc)) {
            delete oc;
            oc = 0;
        }
    } else {
        delete oc;
        oc = 0;
    }
    return oc;
}

/**
 * Builds the ostream used for raw data output
 */
ostream *buildRawOutputStream(OptionsCont *oc) {
    if(oc->getBool("R"))
	    return 0;

    filebuf *fb = new filebuf; // !!! possible memory leak
    ostream *craw = new ostream( (oc->getString("o")=="") ?
	    cout.rdbuf() :
	    fb->open(oc->getString("o").c_str(), ios::out|ios::trunc));
    if(craw->rdbuf()!=fb)
	    delete fb;
    if(!craw->good()) {
        cout << "The output file '" << oc->getString("o") << "' could not be built." << endl;
        cout << "Simulation failed." << endl;
        throw ProcessError();
    }
    return craw;
}

/**
 * loads the net, additional routes and the detectors
 */
MSNet *load(OptionsCont *oc) {
    NLNetBuilder builder(*oc);
    MSNet *ret = builder.build();
    if(ret==0)
        throw ProcessError();
    return ret;
}

/**
 * method for post-load - net initialisation
 */
void
postbuild(OptionsCont &oc, MSNet &net) {
    // set the initial density and vehicle speed when wished
/*    if(oc.isSet("initial-density")) {
        double initialSpeed = oc.isSet("initial-speed") ? oc.getFloat("initial-speed") : 5;
        net.setInitialState(oc.getFloat("initial-density"), initialSpeed);
    }*/
}

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
    int ret = 0;
    try {
        // try to initialise the XML-subsystem
        if(!XMLSubSys::init()) {
            return 1;
        }
        // parse the settings
        OptionsCont *oc = getSettings(argc, argv);
        if(oc==0) {
            cout << "Quitting." << endl;
            return 1;
        }
        // check whether only the help shall be printed
        if(oc->getBool("help")) {
            HelpPrinter::print(help);
            delete oc;
            return 0;
        }
        // load the net
        MSNet *net = load(oc);
        postbuild(*oc, *net);
        // simulate when everything's ok
        ostream *craw = buildRawOutputStream(oc);
        // report the begin when wished
        if(oc->getBool("v"))
            cout << "Simulation started with time: " << oc->getLong("b") << endl;
        // simulate
        net->simulate(craw, oc->getLong("b"), oc->getLong("e"));
        // report the end when wished
        if(oc->getBool("v"))
            cout << "Simulation ended at time: " << oc->getLong("e") << endl;
        delete net;
        delete craw;
    } catch (ProcessError) {
        ret = 1;
    }
    XMLSubSys::close();
    return ret;
}










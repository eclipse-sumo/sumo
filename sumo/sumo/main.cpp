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
// Revision 1.1  2002/04/08 07:21:21  traffic
// Initial revision
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
#include <ctime>
//#include <iomanip>
#include "utils/Option.h"
#include "utils/OptionsCont.h"
#include "utils/OptionsIO.h"
#include "utils/UtilExceptions.h"
#include "utils/FileHelpers.h"
#include "utils/HelpPrinter.h"
#include "help.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;
 
/* =========================================================================
 * static variables
 * ======================================================================= */
#ifdef _SPEEDCHECK
long novehicles;
time_t	begin;
time_t	end;
#endif

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
      // check the folder to load the junction logics from
      if(!oc->isSet("j")||!FileHelpers::exists(oc->getString("j"))) {
        cout << "Error: The named junctions folder '" << oc->getString("j") << "' does not exist." << endl;
        ok = false;
      }
      if(ok) {
        string dir = oc->getString("j");
        if(dir.at(dir.length()-1)!='/'&&dir.at(dir.length()-1)!='\\') {
          dir = dir + '/';
          oc->set("j", dir);
        }
      }
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
      // check if the output file is given
      if(!oc->isSet("o")) {
        cout << "Error: No output file (-o) specified." << endl;
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
    try {
	oc = new OptionsCont("SUMO_DATA");
    } catch (InvalidArgument &e) {
       cout << "SUMO requires a data folder." << endl;
       cout << "Generate a folder anywhere on your file system and copy the contents of " << endl << "the 'data'-folder that came with the SUMO-distribution into it." << endl;
       return 0;
    }
    // register the file i/o options
    oc->doRegister("net-file", 'n', new Option_String());
    oc->doRegisterSystemPath("junction-folder", 'j', "junctions");
    oc->doRegister("route-file", 'r', new Option_String());
    oc->doRegister("output-file", 'o', new Option_String(""));
    oc->doRegisterSystemPath("configuration-file", 'c', "config/sumo.cfg");
    oc->addSynonyme("net-file", "net");
    oc->addSynonyme("simulation-file", "net");
    oc->addSynonyme("simulation", "net");
    oc->addSynonyme("s", "net");
    oc->addSynonyme("route-file", "routes");
    oc->addSynonyme("output-file", "output");
    oc->addSynonyme("configuration-file", "configuration");
    // register the simulation settings
    oc->doRegister("begin", 'b', new Option_Long());
    oc->doRegister("end", 'e', new Option_Long());
    // register the report options
    oc->doRegister("verbose", 'v', new Option_Bool(false));
    oc->doRegister("warn", 'w', new Option_Bool(true));
    oc->doRegister("print-options", 'p', new Option_Bool(false));
    oc->doRegister("help", new Option_Bool(false));
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
    
    filebuf *fb = new filebuf;
    ostream *craw = new ostream( (oc->getString("o")=="") ?
	cout.rdbuf() :
	fb->open(oc->getString("o").c_str(), ios::out|ios::trunc));
    if(craw->rdbuf()!=fb)
	delete fb;
    return craw;
/*
    if(oc->getString("o")=="") {
	ofstream *craw = new ofstream();
	craw->copyfmt(cout);
	craw->clear(cout.rdstate());
	craw->rdbuf(cout.rdbuf());
	return craw;
    }
    else 
	return new ofstream(oc->getString("o").c_str());
*/}

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
  int ret = 1;
  try {
    // parse the settings
    OptionsCont *oc = getSettings(argc, argv);
    if(oc==0) {
      cout << "Quitting." << endl;
      return 1;
    }
    // check whether only the help shall be printed
    if(oc->getBool("help")) {
        HelpPrinter::print(help);
        return 0;
    }
    // load the net
    MSNet *net = 0;
    NLNetBuilder builder(true, oc->getBool("v"));
    net = builder.loadNet(oc->getString("n").c_str(), oc->getString("j").c_str());
    // load additional routes when wished
    if(oc->isSet("r")&&net!=0) {
       MSEmitControl *routes = builder.loadVehicles(oc->getString("r").c_str());
       if(routes!=0) {
          net->addVehicles(routes);
          delete routes;
       } else {
          cout << "Quittinig." << endl;
          return -1;
       }
    }

    // simulate when everything's ok
    ostream *craw = 0;
    if(net!=0) {

#ifdef _SPEEDCHECK
  time(&begin);
  novehicles = 0;
#endif
       craw = buildRawOutputStream(oc);
       if(oc->getBool("v"))
	   cout << "Simulation started with time: " << oc->getLong("b") << endl;
       if(!net->simulate(craw, oc->getLong("b"), oc->getLong("e")))
	   ret = 0;

#ifdef _SPEEDCHECK
  time(&end);
#endif
       if(oc->getBool("v"))
	   cout << "Simulation ended at time: " << oc->getLong("e") << endl;
    }
    delete net;
    delete craw;
  } catch (UnsupportedFeature &e) {
    cout << e.message() << endl;
    return -1;
  }
  return ret;
}










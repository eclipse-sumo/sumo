/****************************************************************************/
/// @file    sumo_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// The main procedure for the simulation module
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <ctime>
#include <string>
#include <iostream>
#include <microsim/MSNet.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEmitControl.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/trigger/MSTriggerControl.h>
#include <netload/NLBuilder.h>
#include <netload/NLHandler.h>
#include <netload/NLTriggerBuilder.h>
#include <netload/NLEdgeControlBuilder.h>
#include <netload/NLJunctionControlBuilder.h>
#include <netload/NLDetectorBuilder.h>
#include <netload/NLGeomShapeBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/options/OptionsSubSys.h>
#include <sumo_only/SUMOFrame.h>
#include <microsim/output/MSDetectorControl.h>
#include <utils/iodevices/SharedOutputDevices.h>

#include <itm-remoteserver/remoteserver.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// conditionally add library search record to object file for DataReel
#ifdef USE_SOCKETS
#ifdef _WIN32
#ifdef _DEBUG
#pragma comment (lib, "gxcoded.lib")
#else // _DEBUG
#pragma comment (lib, "gxcode.lib")
#endif // _DEBUG
#endif // _WIN32
#endif // USE_SOCKETS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// functions
// ===========================================================================
/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */
/**
 * loads the net, additional routes and the detectors
 */
MSNet *
load(OptionsCont &oc)
{
    SharedOutputDevices::setInstance(new SharedOutputDevices());
    SUMOFrame::setMSGlobals(oc);
    MSNet *net =
        new MSNet(oc.getInt("begin"), oc.getInt("end"),
                  oc.getFloat("too-slow-rtf"), !oc.getBool("no-duration-log"));
    NLEdgeControlBuilder eb;
    NLJunctionControlBuilder jb(*net, oc);
    NLDetectorBuilder db(*net);
    NLTriggerBuilder tb;
    NLGeomShapeBuilder sb(*net);
    NLHandler handler("", *net, db, tb, eb, jb, sb, false, oc.getInt("incremental-dua-base"), oc.getInt("incremental-dua-step"));
    NLBuilder builder(oc, *net, eb, jb, db, tb, sb, handler);
    if (!builder.build()) {
        delete net;
        net = 0;
    }
    return net;
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
    size_t rand_init = 10551;
    int ret = 0;
    try {
        int init_ret = SystemFrame::init(false, argc, argv, SUMOFrame::fillOptions);
        if (init_ret<0) {
            cout << "SUMO sumo" << endl;
            cout << " (c) DLR/ZAIK 2000-2007; http://sumo.sourceforge.net" << endl;
            cout << " Version " << VERSION << endl;
            switch (init_ret) {
            case -2:
                OptionsSubSys::getOptions().printHelp(cout);
                break;
            case -4:
                break;
            default:
                cout << " Use --help to get the list of options." << endl;
            }
            SystemFrame::close();
            return 0;
        } else if (init_ret!=0||!SUMOFrame::checkOptions(OptionsSubSys::getOptions())) {
            throw ProcessError();
        }
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
        // load the net
        MSNet *net = load(oc);
        if (net!=0) {
#ifdef ITM
            if (oc.getInt("remote-port") != 0) {
                WRITE_MESSAGE("waiting for request on port " + toString<int>(oc.getInt("remote-port")));
                itm::RemoteServer rs(oc.getInt("remote-port"),oc.getInt("end"),oc.getFloat("penetration"));
                WRITE_MESSAGE("Simulation started with time: " + toString<int>(oc.getInt("begin")));
                rs.run();
                WRITE_MESSAGE("Simulation ended at time: " + toString<int>(net->getCurrentTimeStep()));
            } else {
#endif
                // report the begin when wished
                WRITE_MESSAGE("Simulation started with time: " + toString<int>(oc.getInt("begin")));
                // simulate
                net->simulate(oc.getInt("begin"), oc.getInt("end"));
                // report the end when wished
                WRITE_MESSAGE("Simulation ended at time: " + toString<int>(net->getCurrentTimeStep()));
#ifdef ITM
            }
#endif
            delete net;
            delete SharedOutputDevices::getInstance();
        }
    } catch (ProcessError &e) {
        if(string(e.what())!=string("Process Error") && string(e.what())!=string("")) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (...) {
        MSNet::clearAll();
        delete SharedOutputDevices::getInstance();
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    SystemFrame::close();
    return ret;
}



/****************************************************************************/


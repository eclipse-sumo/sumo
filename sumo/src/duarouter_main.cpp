/****************************************************************************/
/// @file    duarouter_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 06 Jun 2002
/// @version $Id$
///
// }
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <utils/common/TplConvert.h>
#include <iostream>
#include <string>
#include <limits.h>
#include <ctime>
#include <router/ROLoader.h>
#include <router/ROEdgeVector.h>
#include <router/RONet.h>
#include <router/ROVehicleType_Krauss.h>
#include <utils/helpers/SUMODijkstraRouter.h>
#include <routing_dua/RODUAEdgeBuilder.h>
#include <router/ROFrame.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <routing_dua/RODUAFrame.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
 * loads the net
 * The net is in this meaning made up by the net itself and the dynamic
 * weights which may be supplied in a separate file
 */
RONet *
loadNet(ROLoader &loader, OptionsCont &oc)
{
    // load the net
    RODUAEdgeBuilder builder(oc.getBool("expand-weights"));
    RONet *net = loader.loadNet(builder);
    if (net==0) {
        throw ProcessError();
    }
    // load the weights when wished/available
    if (oc.isSet("weights")) {
        loader.loadWeights(*net, oc.getString("weights"), false);
    }
    if (oc.isSet("lane-weights")) {
        loader.loadWeights(*net, oc.getString("lane-weights"), true);
    }
    if (oc.isSet("S")) {
        loader.loadSupplementaryWeights(*net);
    }
    return net;
}



/**
 * Computes the routes saving them
 */
void
startComputation(RONet &net, ROLoader &loader, OptionsCont &oc)
{
    SUMOAbstractRouter<ROEdge, ROVehicle> *router;
    if (net.hasRestrictions()) {
        router = new SUMODijkstraRouter<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle>, ROEdge>(
                     net.getEdgeNo(), oc.getBool("continue-on-unbuild"), &ROEdge::getEffort);
    } else {
        router = new SUMODijkstraRouter<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle>, ROEdge>(
                     net.getEdgeNo(), oc.getBool("continue-on-unbuild"), &ROEdge::getEffort);
    }
    // build the router
    // initialise the loader
    size_t noLoaders =
        loader.openRoutes(net, oc.getFloat("gBeta"), oc.getFloat("gA"));
    if (noLoaders==0) {
        throw ProcessError("No route input specified.");
    }
    // prepare the output
    net.openOutput(oc.getString("output"), true);
    // the routes are sorted - process stepwise
    if (!oc.getBool("unsorted")) {
        loader.processRoutesStepWise(oc.getInt("begin"), oc.getInt("end"), net, *router);
    }
    // the routes are not sorted: load all and process
    else {
        loader.processAllRoutes(oc.getInt("begin"), oc.getInt("end"), net, *router);
    }
    // end the processing
    loader.closeReading();
    net.closeOutput();
    delete router;
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
    int ret = 0;
    RONet *net = 0;
    ROLoader *loader = 0;
    try {
        // initialise the application system (messaging, xml, options)
        int init_ret =
            SystemFrame::init(false, argc, argv, RODUAFrame::fillOptions);
        if (init_ret<0) {
            OptionsSubSys::getOptions().printHelp(cout, init_ret == -2, init_ret == -4);
            SystemFrame::close();
            return 0;
        } else if (init_ret!=0||!RODUAFrame::checkOptions(OptionsSubSys::getOptions())) {
            throw ProcessError();
        }
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
        // load data
        ROVehicleBuilder vb;
        loader = new ROLoader(oc, vb, false);
        net = loadNet(*loader, oc);
        if (net!=0) {
            // build routes
            try {
                startComputation(*net, *loader, oc);
            } catch (SAXParseException &e) {
                MsgHandler::getErrorInstance()->inform(
                    toString<int>(e.getLineNumber()));
                ret = 1;
            } catch (SAXException &e) {
                MsgHandler::getErrorInstance()->inform(
                    TplConvert<XMLCh>::_2str(e.getMessage()));
                ret = 1;
            }
        } else {
            ret = 1;
        }
    } catch (ProcessError &e) {
        if (string(e.what())!=string("Process Error") && string(e.what())!=string("")) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    delete net;
    delete loader;
    SystemFrame::close();
    if (ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}



/****************************************************************************/


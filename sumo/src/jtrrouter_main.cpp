//---------------------------------------------------------------------------//
//                          jp_router_main.cpp
//      The main procedure and some initialisation functions for the jp-router
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Tue, 20 Jan 2004
//  subproject           : junction percentage router
//  copyright            : (C) 2004 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
        "$Id$";
}
// $Log$
// Revision 1.7  2005/09/23 06:13:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/07/12 12:55:27  dkrajzew
// build number output added
//
// Revision 1.4  2005/05/04 09:33:43  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2005/02/17 10:33:29  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.2  2004/11/23 10:43:28  dkrajzew
// debugging
//
// Revision 1.1  2004/08/02 13:03:19  dkrajzew
// applied better names
//
// Revision 1.3  2004/07/02 09:50:22  dkrajzew
// generalised for easier online-router implementation; debugging
//
// Revision 1.2  2004/04/02 11:32:45  dkrajzew
// moving the vehicle forward if it shall start at a too short edge added; output of the number of loaded, build, and discarded
//
// Revision 1.1  2004/02/06 08:56:45  dkrajzew
// _INC_MALLOC definition removed (does not work on MSVC7.0)
//
// Revision 1.6  2004/02/02 16:22:50  dkrajzew
// exception handling improved
//
// Revision 1.5  2004/01/28 14:20:45  dkrajzew
// allowed to specify the maximum edge number in a route by a factor
//
// Revision 1.4  2004/01/27 08:51:00  dkrajzew
// cell processing options added
//
// Revision 1.3  2004/01/27 08:46:15  dkrajzew
// shared input possibilities between both routers
//
// Revision 1.2  2004/01/26 09:58:46  dkrajzew
// sinks are now simply marked as these instead of the usage of a further
//  container
//
// Revision 1.1  2004/01/26 07:12:12  dkrajzew
// now two routers are available - the dua- and the jp-router
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <sax/SAXException.hpp>
#include <sax/SAXParseException.hpp>
#include <utils/common/TplConvert.h>
#include <iostream>
#include <string>
#include <fstream>
#include <limits.h>
#include <ctime>
#include <set>
#include <router/ROFrame.h>
#include <router/ROLoader.h>
#include <router/RONet.h>
#include <router/ROVehicleType_Krauss.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/HelpPrinter.h>
#include <utils/common/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/xml/XMLSubSys.h>
#include <routing_jtr/ROJPEdgeBuilder.h>
#include <routing_jtr/ROJPRouter.h>
#include <routing_jtr/ROJPEdge.h>
#include <routing_jtr/ROJPTurnDefLoader.h>
#include <routing_jtr/ROJPHelpers.h>
#include <routing_jtr/ROJTRFrame.h>
#include "jtrrouter_help.h"
#include "jtrrouter_build.h"
#include "sumo_version.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
/*
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
//   #define _INC_MALLOC         // exclude standard memory alloc procedures
#ifdef WIN32
   #include <utils/dev/MemDiff.h>
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG
   */
#endif


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
 * loads the net
 * The net is in this meaning made up by the net itself and the dynamic
 * weights which may be supplied in a separate file
 */
RONet *
loadNet(ROLoader &loader, OptionsCont &oc,
        const std::vector<SUMOReal> &turnDefs)
{
    // load the net
    ROJPEdgeBuilder builder;
    RONet *net = loader.loadNet(builder);
    if(net==0) {
        return 0;
    }
    builder.setTurningDefinitions(*net, turnDefs);
    // load the weights when wished/available
    if(oc.isSet("w")) {
        loader.loadWeights(*net, oc.getString("w"), false);
    }
    if(oc.isSet("lane-weights")) {
        loader.loadWeights(*net, oc.getString("lane-weights"), true);
    }
    // initialise the network
//    net->postloadInit();
    return net;
}

SUMOReal
parseFloat_ReportError(const std::string &toParse, const std::string &output)
{
    try {
        return TplConvert<char>::_2SUMOReal(toParse.c_str());
    } catch (EmptyData &) {
    } catch (NumberFormatException&) {
    }
    //
    MsgHandler::getErrorInstance()->inform(output);
    throw NumberFormatException();
}


std::vector<SUMOReal>
getTurningDefaults(OptionsCont &oc)
{
    std::vector<SUMOReal> ret;
    if(oc.isSet("turn-defaults")) {
        string def = oc.getString("turn-defaults");
        StringTokenizer st(def, ";");
        switch(st.size()) {
        case 3:
            try {
                ret.push_back(parseFloat_ReportError(st.next(),
                    "The first number in turn defaults is not numeric."));
                ret.push_back(parseFloat_ReportError(st.next(),
                    "The second number in turn defaults is not numeric."));
                ret.push_back(parseFloat_ReportError(st.next(),
                    "The second number in turn defaults is not numeric."));
            } catch(NumberFormatException&) {
                throw ProcessError();
            }
            break;
        default:
            MsgHandler::getErrorInstance()->inform(
                "The defaults for turnings must be a tuple of two or three numbers divided by ';'");
            throw ProcessError();
        }
    }
    return ret;
}


void
loadJPDefinitions(RONet &net, OptionsCont &oc)
{
    std::set<ROJPEdge*> ret;
    // load the turning definitions (and possible sink definition)
    if(oc.isSet("turn-definition")) {
        ROJPTurnDefLoader loader(net);
        ret = loader.load(oc.getString("turn-definition"));
    }
    // add edges specified at the input/within the configuration
    if(oc.isSet("sinks")) {
        ROJPHelpers::parseROJPEdges(net, ret, oc.getString("sinks"));
    }
    // set the sink information into the edges
    for(std::set<ROJPEdge*>::iterator i=ret.begin(); i!=ret.end(); i++) {
        (*i)->setType(ROEdge::ET_SINK);
    }
}


/**
 * Computes the routes saving them
 */
void
startComputation(RONet &net, ROLoader &loader, OptionsCont &oc)
{
    // prepare the output
    net.openOutput(
        oc.getString("output"), false);
    // build the router
    ROJPRouter router(net);
    // initialise the loader
    loader.openRoutes(net, 1, 1);
    // the routes are sorted - process stepwise
    if(!oc.getBool("unsorted")) {
        loader.processRoutesStepWise(
            oc.getInt("b"), oc.getInt("e"), net, router);
    }
    // the routes are not sorted: load all and process
    else {
        loader.processAllRoutes(
            oc.getInt("b"), oc.getInt("e"), net, router);
    }
    // end the processing
    loader.closeReading();
    net.closeOutput();
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
#ifdef _DEBUG
#ifdef WIN32
    /*
    CMemDiff state1;
    // uncomment next line and insert the context of an undeleted
    //  allocation to break within it (MSVC++ only)
    // _CrtSetBreakAlloc(434490);
    */
#endif
#endif
    int ret = 0;
    RONet *net = 0;
    try {
        // initialise the application system (messaging, xml, options)
        int init_ret = SystemFrame::init(false, argc, argv,
			ROJTRFrame::fillOptions, ROJTRFrame::checkOptions, help);
        if(init_ret==-1) {
            cout << "SUMO jtrrouter" << endl;
            cout << " Version " << version << endl;
            cout << " Build #" << NEXT_BUILD_NUMBER << endl;
            SystemFrame::close();
            return 0;
        } else if(init_ret!=0) {
            throw ProcessError();
        }
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
        ROFrame::setDefaults(oc);
        std::vector<SUMOReal> defs = getTurningDefaults(oc);
        // load data
        ROVehicleBuilder vb;
        ROLoader loader(oc, vb, true);
        net = loadNet(loader, oc, defs);
        if(net!=0) {
            // parse and set the turn defaults first

            // build routes
            try {
                loadJPDefinitions(*net, oc);
                startComputation(*net, loader, oc);
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
            throw ProcessError();
        }
        if(MsgHandler::getErrorInstance()->wasInformed()) {
            throw ProcessError();
        }
    } catch (...) {
        WRITE_MESSAGE("Quitting (on error).");
        ret = 1;
    }
    delete net;
    SystemFrame::close();
    if(ret==0) {
        WRITE_MESSAGE("Success.");
    }
    return ret;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


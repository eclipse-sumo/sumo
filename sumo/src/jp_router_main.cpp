//---------------------------------------------------------------------------//
//                          jp_router_main.cpp
//		The main procedure and some initialisation functions for the jp-router
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
// Revision 1.2  2004/01/26 09:58:46  dkrajzew
// sinks are now simply marked as these instead of the usage of a further container
//
// Revision 1.1  2004/01/26 07:12:12  dkrajzew
// now two routers are available - the dua- and the jp-router
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <sax/SAXException.hpp>
#include <sax/SAXParseException.hpp>
#include <utils/convert/TplConvert.h>
#include <iostream>
#include <string>
#include <fstream>
#include <limits.h>
#include <ctime>
#include <set>
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
#include <utils/convert/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/xml/XMLSubSys.h>
#include <routing_jp/ROJPEdgeBuilder.h>
#include <routing_jp/ROJPRouter.h>
#include <routing_jp/ROJPEdge.h>
#include <routing_jp/ROJPTurnDefLoader.h>
#include <routing_jp/ROJPHelpers.h>
#include "jp_router_help.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#ifdef WIN32
   #include <utils/dev/MemDiff.h>
#endif
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
/** validate options (settings) */
bool
checkOptions(OptionsCont &oc)
{
    // check whether the output is valid and can be build
    if(!oc.isSet("output")) {
        MsgHandler::getErrorInstance()->inform("No output specified.");
        return false;
    }
    std::ofstream tst(oc.getString("output").c_str());
    if(!tst.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The output file '") + oc.getString("output")
            + string("' can not be build."));
        return false;
    }
    //
    if(oc.getBool("abs-rand")&&!oc.isSet("srand")) {
        oc.set("srand", toString<int>(time(0)));
    }
    return true;
}


/** build and retrieve the options (settings) */
void
fillOptions(OptionsCont &oc)
{
    // register the file i/o options
    oc.doRegister("output-file", 'o', new Option_FileName());
    oc.doRegister("net-file", 'n', new Option_FileName());
    oc.doRegister("weights", 'w', new Option_FileName());
    oc.doRegister("flow-definition", 'f', new Option_FileName());
    oc.doRegister("turn-definition", 't', new Option_FileName());
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.doRegister("random-per-second", 'R', new Option_Float());
    oc.addSynonyme("net-file", "net");
    oc.addSynonyme("output-file", "output");
    oc.addSynonyme("configuration-file", "configuration");
    oc.addSynonyme("weights", "weight-file");
    oc.addSynonyme("flow-definition", "flows");
    oc.addSynonyme("turn-definition", "turns");
    oc.doRegister("turn-defaults", 'T', new Option_String("30;50;20"));
    oc.doRegister("sinks", 's', new Option_String());
    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.doRegister("end", 'e', new Option_Integer(864000));
    oc.doRegister("unsorted", new Option_Bool(false));
    // register vehicle type defaults
    oc.doRegister("krauss-vmax", 'V', new Option_Float(float(70)));
    oc.doRegister("krauss-a", 'A', new Option_Float(float(2.6)));
    oc.doRegister("krauss-b", 'B', new Option_Float(float(4.5)));
    oc.doRegister("krauss-length", 'L', new Option_Float(float(5)));
    oc.doRegister("krauss-eps", 'E', new Option_Float(float(0.5)));
    // register the report options
    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.doRegister("continue-on-unbuild", new Option_Bool(false));
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("help", new Option_Bool(false));
    oc.doRegister("log-file", 'l', new Option_FileName());
    //
    oc.doRegister("srand", new Option_Integer(23423));
    oc.doRegister("abs-rand", new Option_Bool(false));
}


/**
 * loads the net
 * The net is in this meaning made up by the net itself and the dynamic
 * weights which may be supplied in a separate file
 */
RONet *
loadNet(ROLoader &loader, OptionsCont &oc,
        const std::vector<float> &turnDefs)
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
        loader.loadWeights(*net);
    }
    // initialise the network
    net->postloadInit();
    return net;
}

float
parseFloat_ReportError(const std::string &toParse, const std::string &output)
{
    try {
        return TplConvert<char>::_2float(toParse.c_str());
    } catch (EmptyData &) {
    } catch (NumberFormatException&) {
    }
    //
    MsgHandler::getErrorInstance()->inform(output);
    throw NumberFormatException();
}


std::vector<float>
getTurningDefaults(OptionsCont &oc)
{
    std::vector<float> ret;
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
	if(oc.isSet("end-streets")) {
		ROJPHelpers::parseROJPEdges(net, ret, oc.getString("end-streets"));
	}
    // set the sink information into the edges
    for(std::set<ROJPEdge*>::iterator i=ret.begin(); i!=ret.end(); i++) {
        (*i)->setType(ROEdge::ET_SINK);
    }
}



/**
 * Inserts the default from options into the vehicle
 *  type descriptions
 */
void
setDefaults(OptionsCont &oc)
{
    // insert the krauss-values
    ROVehicleType_Krauss::myDefault_A =
        oc.getFloat("krauss-a");
    ROVehicleType_Krauss::myDefault_B =
        oc.getFloat("krauss-b");
    ROVehicleType_Krauss::myDefault_EPS =
        oc.getFloat("krauss-eps");
    ROVehicleType_Krauss::myDefault_LENGTH =
        oc.getFloat("krauss-length");
    ROVehicleType_Krauss::myDefault_MAXSPEED =
        oc.getFloat("krauss-vmax");
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
    CMemDiff state1;
    // uncomment next line and insert the context of an undeleted
    //  allocation to break within it (MSVC++ only)
    // _CrtSetBreakAlloc(434490);
#endif
#endif
    int ret = 0;
    RONet *net = 0;
    try {
        // initialise the application system (messaging, xml, options)
        if(!SystemFrame::init(false, argc, argv,
            fillOptions, checkOptions, help)) {
            throw ProcessError();
        }
        // retrieve the options
        OptionsCont &oc = OptionsSubSys::getOptions();
        setDefaults(oc);
        std::vector<float> defs = getTurningDefaults(oc);
        // load data
        ROLoader loader(oc, true);
        net = loadNet(loader, oc, defs);
        if(net!=0) {
			// parse and set the turn defaults first

            // build routes
            try {
                loadJPDefinitions(*net, oc);
                startComputation(*net, loader, oc);
            } catch (SAXParseException e) {
                MsgHandler::getErrorInstance()->inform(
                    toString<int>(e.getLineNumber()));
                ret = 1;
            } catch (SAXException e) {
                MsgHandler::getErrorInstance()->inform(
                    TplConvert<XMLCh>::_2str(e.getMessage()));
                ret = 1;
            }
        } else {
            ret = 1;
        }
    } catch (std::string) {
        MsgHandler::getErrorInstance()->inform(
            "Quitting (building failed).");
        ret = 1;
    }
    delete net;
    SystemFrame::close();
    if(ret==0) {
        MsgHandler::getMessageInstance()->inform("Success.");
    }
    return ret;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


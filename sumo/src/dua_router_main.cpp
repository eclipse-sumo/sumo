/***************************************************************************
                          main.cpp
			  The main procedure for the build of person/vehicle routes
                             -------------------
    project              : SUMO
    subproject           : router
    begin                : Thu, 06 Jun 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
namespace
{
    const char rcsid[] =
        "$Id$";
}
// $Log$
// Revision 1.4  2004/04/02 11:30:35  dkrajzew
// moving the vehicle forward if it shall start at a too short edge added; output of the number of loaded, build, and discarded
//
// Revision 1.3  2004/02/06 08:54:28  dkrajzew
// _INC_MALLOC definition removed (does not work on MSVC7.0)
//
// Revision 1.2  2004/01/27 08:46:15  dkrajzew
// shared input possibilities between both routers
//
// Revision 1.1  2004/01/26 07:12:12  dkrajzew
// now two routers are available - the dua- and the jp-router
//
// Revision 1.27  2003/10/28 08:35:01  dkrajzew
// random number specification options added
//
// Revision 1.26  2003/10/17 07:16:44  dkrajzew
// errors patched
//
// Revision 1.25  2003/10/17 06:53:08  dkrajzew
// added the possibility to change the standard-krauss parameter via the
//  command line
//
// Revision 1.24  2003/08/21 13:01:39  dkrajzew
// some bugs patched
//
// Revision 1.23  2003/07/22 15:17:15  dkrajzew
// documentation
//
// Revision 1.22  2003/07/07 08:42:26  dkrajzew
// no configuration is loaded as default any more
//
// Revision 1.21  2003/06/24 14:38:46  dkrajzew
// false instantiation of option "log-file" as Option_String patched into
//  Option_FileName patched
//
// Revision 1.20  2003/06/24 08:50:00  dkrajzew
// some more sophisticated default values for Gawrons dua inserted
//
// Revision 1.19  2003/06/24 08:06:36  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.18  2003/06/19 11:03:57  dkrajzew
// debugging
//
// Revision 1.17  2003/06/19 07:07:52  dkrajzew
// false order of calling XML- and Options-subsystems patched
//
// Revision 1.16  2003/06/18 11:26:15  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
// this handler puts it further to output instances.
// changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.15  2003/05/20 09:54:45  dkrajzew
// configuration files are no longer set as default
//
// Revision 1.14  2003/04/10 16:13:52  dkrajzew
// recent changes
//
// Revision 1.13  2003/04/09 15:45:30  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.11  2003/04/02 11:48:43  dkrajzew
// debug statements removed
//
// Revision 1.10  2003/03/31 06:18:17  dkrajzew
// help screen corrected
//
// Revision 1.9  2003/03/18 13:06:19  dkrajzew
// windows eol removed
//
// Revision 1.8  2003/03/12 16:34:34  dkrajzew
// some style guides applied
//
// Revision 1.7  2003/03/03 15:08:20  dkrajzew
// debugging
//
// Revision 1.6  2003/02/07 10:37:30  dkrajzew
// files updated
//
// Revision 1.4  2002/10/22 10:02:47  dkrajzew
// minor warnings removed
//
// Revision 1.3  2002/10/21 10:01:03  dkrajzew
// routedefs renamed to tripdefs in options
//
// Revision 1.2  2002/10/18 11:35:53  dkrajzew
// postinitialisation of edges for computation of lane-independent value added
//
// Revision 1.1  2002/10/16 14:51:08  dkrajzew
// Moved from ROOT/sumo to ROOT/src; added further help and main files for
//  netconvert, router, od2trips and gui version
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
#include <router/ROLoader.h>
#include <router/RONet.h>
#include <router/ROVehicleType_Krauss.h>
#include <routing_dua/RODijkstraRouter.h>
#include <routing_dua/RODUAEdgeBuilder.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/HelpPrinter.h>
#include <utils/convert/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include "dua_router_help.h"


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
//   #define _INC_MALLOC	     // exclude standard memory alloc procedures
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
    if(!oc.isSet("o")) {
        MsgHandler::getErrorInstance()->inform("No output specified.");
        return false;
    }
    std::ofstream tst(oc.getString("o").c_str());
    if(!tst.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("The output file '") + oc.getString("o")
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
    oc.doRegister("cell-input", new Option_FileName());
    oc.doRegister("artemis-input", new Option_FileName());
    oc.doRegister("flow-definition", 'f', new Option_FileName());
    oc.doRegister("output", 'o', new Option_FileName());
    oc.doRegister("net-files", 'n', new Option_FileName());
    oc.doRegister("weights", 'w', new Option_FileName());
    oc.doRegister("sumo-input", 's', new Option_FileName());
    oc.doRegister("trip-defs", 't', new Option_FileName());
    oc.doRegister("alternatives", 'a', new Option_FileName());
    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.doRegister("save-cell-rindex", new Option_Bool(false));
    oc.doRegister("random-per-second", 'R', new Option_Float());
    oc.addSynonyme("flow-definition", "flows");
    oc.addSynonyme("net-files", "net");
    oc.addSynonyme("output-file", "output");
    oc.addSynonyme("configuration-file", "configuration");
    oc.addSynonyme("weights", "weight-files");
    oc.addSynonyme("artemis", "artemis-input");
    oc.addSynonyme("cell", "cell-input");
    oc.addSynonyme("sumo", "sumo-input");
    oc.addSynonyme("trips", "trip-defs");
    // register the simulation settings
    oc.doRegister("begin", 'b', new Option_Integer(0));
    oc.doRegister("end", 'e', new Option_Integer(864000));
    // register Gawron's DUE-settings
    oc.doRegister("gBeta", new Option_Float(float(0.3)));
    oc.doRegister("gA", new Option_Float(0.05));
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
    // register the data processing options
    oc.doRegister("unsorted", new Option_Bool(false));
    oc.doRegister("move-on-short", new Option_Bool(false));
    oc.doRegister("stats-period", new Option_Integer(-1));
    oc.doRegister("intel-cell", new Option_Bool(false));
    oc.doRegister("no-last-cell", new Option_Bool(false));
//    oc.doRegister("use-lanes", 'L', new Option_Bool(false));
    oc.doRegister("scheme", 'x', new Option_String("traveltime"));
//    oc.doRegister("no-sort", 'S', new Option_Bool(false));
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
loadNet(ROLoader &loader, OptionsCont &oc)
{
    // load the net
    RODUAEdgeBuilder builder;
    RONet *net = loader.loadNet(builder);
    if(net==0) {
        return 0;
    }
    // load the weights when wished/available
    if(oc.isSet("w")) {
        loader.loadWeights(*net);
    }
    // initialise the network
    net->postloadInit();
    return net;
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
        oc.getString("output"), true);
    // build the router
    RODijkstraRouter router(net);
    // initialise the loader
    loader.openRoutes(net, oc.getFloat("gBeta"), oc.getFloat("gA"));
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
        // load data
        ROLoader loader(oc, false);
        net = loadNet(loader, oc);
        if(net!=0) {
            // build routes
            try {
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
    } catch (...) {
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


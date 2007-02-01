/****************************************************************************/
/// @file    RODUAFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// Some helping methods for usage within sumo and sumo-gui
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

#include <iostream>
#include <fstream>
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include "RODUAFrame.h"
#include <router/ROFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SystemFrame.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
RODUAFrame::fillOptions(OptionsCont &oc)
{
    // give some application descriptions
    oc.setApplicationDescription("Shortest path router and DUE computer for the microscopic road traffic simulation SUMO.");
#ifdef WIN32
    oc.setApplicationName("duarouter.exe");
#else
    oc.setApplicationName("sumo-duarouter");
#endif
    oc.addCallExample("-c <CONFIGURATION>");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Generated Vehicles");
    oc.addOptionSubTopic("Time");
    oc.addOptionSubTopic("Report");


    // insert options
    ROFrame::fillOptions(oc);
    addImportOptions(oc);
    addDUAOptions(oc);
    // add rand options
    RandHelper::insertRandOptions(oc);
}


void
RODUAFrame::addImportOptions(OptionsCont &oc)
{
    // register import options
    oc.doRegister("trip-defs", 't', new Option_FileName());
    oc.addSynonyme("trips", "trip-defs");
    oc.addDescription("trip-defs", "Input", "Read trip-definitions from FILE");

    oc.doRegister("flow-definition", 'f', new Option_FileName());
    oc.addSynonyme("flow-definition", "flows");
    oc.addSynonyme("flow-definition", "flow-defs");
    oc.addDescription("flow-definition", "Input", "Read flow-definitions from FILE");

    oc.doRegister("sumo-input", 's', new Option_FileName());
    oc.addSynonyme("sumo", "sumo-input");
    oc.addDescription("sumo-input", "Input", "Read sumo-routes from FILE");

    oc.doRegister("cell-input", new Option_FileName());
    oc.addSynonyme("cell", "cell-input");
    oc.addDescription("cell-input", "Input", "Read cell-routes from FILE");

    oc.doRegister("artemis-input", new Option_FileName());
    oc.addSynonyme("artemis", "artemis-input");
    oc.addDescription("artemis-input", "Input", "Read artemis-routes from FILE");

    oc.doRegister("save-cell-rindex", new Option_Bool(false));
    oc.addDescription("save-cell-rindex", "Input", "Save Cell route indices");

    oc.doRegister("intel-cell", new Option_Bool(false));
    oc.addDescription("intel-cell", "Input", "Flip byte order on reading Cell-routes");

    oc.doRegister("no-last-cell", new Option_Bool(false));
    oc.addDescription("no-last-cell", "Input", "Use best, not the last cell-route");


    // register further processing options
    // ! The subtopic "Processing" must be initialised earlier !
    oc.doRegister("expand-weights", new Option_Bool(false));
    oc.addDescription("expand-weights", "Processing", "Expand weights behind the simulation's end");
}


void
RODUAFrame::addDUAOptions(OptionsCont &oc)
{
    // register additional options
    oc.doRegister("supplementary-weights", 'S', new Option_FileName());
    oc.addSynonyme("supplementary-weights", "add");
    oc.addDescription("supplementary-weights", "Input", "Read additional weights from FILE");

    oc.doRegister("scheme", 'x', new Option_String("traveltime"));
    oc.addDescription("scheme", "Processing", "");

    // register Gawron's DUE-settings
    oc.doRegister("gBeta", new Option_Float(SUMOReal(0.3)));
    oc.addDescription("gBeta", "Processing", "Use FLOAT as Gawron's beta");

    oc.doRegister("gA", new Option_Float((SUMOReal) 0.05));
    oc.addDescription("gA", "Processing", "Use FLOAT as Gawron's alpha");
}


bool
RODUAFrame::checkOptions(OptionsCont &oc)
{
    return ROFrame::checkOptions(oc);
}



/****************************************************************************/


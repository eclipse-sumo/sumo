/****************************************************************************/
/// @file    ROJTRFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Sets and checks options for jtr-routing
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

#include <iostream>
#include <fstream>
#include <ctime>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include "ROJTRFrame.h"
#include <router/ROFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SystemFrame.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
ROJTRFrame::fillOptions()
{
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc);
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Time");
    oc.addOptionSubTopic("Report");

    ROFrame::fillOptions(oc);
    addJTROptions();
    // add rand options
    RandHelper::insertRandOptions();
}


void
ROJTRFrame::addJTROptions()
{
    OptionsCont &oc = OptionsCont::getOptions();
    oc.doRegister("flow-definition", 'f', new Option_FileName());
    oc.addSynonyme("flow-definition", "flows");
    oc.addDescription("flow-definition", "Input", "Read flow-definitions from FILE");

    oc.doRegister("turn-definition", 't', new Option_FileName());
    oc.addSynonyme("turn-definition", "turns");
    oc.addDescription("turn-definition", "Input", "Read turn-definitions from FILE");


    oc.doRegister("exit-times", new Option_Bool(false));
    oc.addDescription("exit-times", "Output", "Write exit times (weights) for each edge");


    oc.doRegister("max-edges-factor", new Option_Float(2.0));
    oc.addDescription("max-edges-factor", "Processing", "");

    oc.doRegister("turn-defaults", 'T', new Option_String("30,50,20"));
    oc.addDescription("turn-defaults", "Processing", "Use STR as default turn definition");

    oc.doRegister("sinks", 's', new Option_String());
    oc.addDescription("sinks", "Processing", "Use STR as sink list");

    oc.doRegister("accept-all-destinations", 'A', new Option_Bool(false));
    oc.addDescription("accept-all-destinations", "Processing", "");

    oc.doRegister("ignore-classes", 'i', new Option_Bool(false));
    oc.addDescription("ignore-classes", "Processing", "");

    oc.doRegister("dismiss-loading-errors", new Option_Bool(false));
    oc.addDescription("dismiss-loading-errors", "Processing", "Continue on broken input");
}


bool
ROJTRFrame::checkOptions()
{
    return ROFrame::checkOptions(OptionsCont::getOptions());
}



/****************************************************************************/


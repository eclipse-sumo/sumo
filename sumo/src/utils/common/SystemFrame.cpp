/****************************************************************************/
/// @file    SystemFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 23.06.2003
/// @version $Id$
///
// A set of actions common to all applications
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include "SystemFrame.h"
#include <string>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include "RandHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
SystemFrame::addConfigurationOptions(OptionsCont &oc) {
    oc.addOptionSubTopic("Configuration");

    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("configuration-file", "configuration");
    oc.addDescription("configuration-file", "Configuration", "Loads the named config on startup");

    oc.doRegister("save-configuration", new Option_FileName());
    oc.addSynonyme("save-config", "save-configuration");
    oc.addDescription("save-configuration", "Configuration", "Saves current configuration into FILE");

    oc.doRegister("save-template", new Option_FileName());
    oc.addDescription("save-template", "Configuration", "Saves a configuration template (empty) into FILE");

    /*
    oc.doRegister("save-template.mandatory-only", new Option_Bool(false));
    oc.addDescription("save-template.mandatory-only", "Configuration", "Saves only mandatory values into template");
    */

    oc.doRegister("save-template.commented", new Option_Bool(false));
    oc.addDescription("save-template.commented", "Configuration", "Adds comments to saved template");
}


void
SystemFrame::addReportOptions(OptionsCont &oc) {
    oc.addOptionSubTopic("Report");

    oc.doRegister("verbose", 'v', new Option_Bool(false));
    oc.addDescription("verbose", "Report", "Switches to verbose output");

    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.addDescription("print-options", "Report", "Prints option values before processing");

    oc.doRegister("help", '?', new Option_Bool(false));
    oc.addDescription("help", "Report", "Prints this screen");

    oc.doRegister("suppress-warnings", 'W', new Option_Bool(false));
    oc.addDescription("suppress-warnings", "Report", "Disables output of warnings");

    oc.doRegister("log-file", 'l', new Option_FileName());
    oc.addDescription("log-file", "Report", "Writes all messages to FILE");
}


void
SystemFrame::close() {
    // close the xml-subsystem
    XMLSubSys::close();
    // delete build program options
    OptionsCont::getOptions().clear();
    // delete messages
    MsgHandler::cleanupOnEnd();
}


/****************************************************************************/


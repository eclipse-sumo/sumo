/****************************************************************************/
/// @file    SystemFrame.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 23.06.2003
/// @version $Id: $
///
// A set of actions common to all applications
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

#include "SystemFrame.h"
#include <string>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/LogFile.h>
#include "RandHelper.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
LogFile *SystemFrame::myLogFile = 0;


// ===========================================================================
// method definitions
// ===========================================================================
int
SystemFrame::init(bool gui, int argc, char **argv,
                  fill_options *fill_f,
                  check_options *check_f)
{
    // initialise the output for option processing
    MsgHandler::getErrorInstance()->report2cerr(true);
    MsgHandler::getErrorInstance()->report2cout(false);
    MsgHandler::getWarningInstance()->report2cerr(false);
    MsgHandler::getWarningInstance()->report2cout(true);
    MsgHandler::getMessageInstance()->report2cerr(false);
    MsgHandler::getMessageInstance()->report2cout(true);
    // initialise the xml-subsystem
    if (!XMLSubSys::init()) {
        return 2;
    }
    // initialise the options-subsystem
    if (argc<2&&!gui) {
        // no options are given
        return -3;
    }
    bool iret = OptionsSubSys::init(!gui, argc, argv, fill_f, check_f);
    // check whether the help shall be printed
    if (OptionsSubSys::getOptions().getBool("help")) {
        //HelpPrinter::print(help);
        return -2;
    }
    // check whether the settings shall be printed
    if (OptionsSubSys::getOptions().getBool("print-options")) {
        cout << OptionsSubSys::getOptions();
    }
    // check whether something has to be done with options
    // whether the current options shall be saved
    if (OptionsSubSys::getOptions().isSet("save-configuration")) {
        ofstream out(OptionsSubSys::getOptions().getString("save-configuration").c_str());
        if (!out.good()) {
            MsgHandler::getErrorInstance()->inform("Could not save configuration to '" + OptionsSubSys::getOptions().getString("save-configuration") + "'");
            if (OptionsSubSys::getOptions().getBool("verbose")) {
                MsgHandler::getMessageInstance()->inform("Written configuration to '" + OptionsSubSys::getOptions().getString("save-configuration") + "'");
            }
            return 1;
        } else {
            OptionsSubSys::getOptions().writeConfiguration(out, true, false, false);
        }
    }
    // whether the template shall be saved
    if (OptionsSubSys::getOptions().isSet("save-template")) {
        ofstream out(OptionsSubSys::getOptions().getString("save-template").c_str());
        if (!out.good()) {
            MsgHandler::getErrorInstance()->inform("Could not save template to '" + OptionsSubSys::getOptions().getString("save-template") + "'");
            return 1;
        } else {
            OptionsSubSys::getOptions().writeConfiguration(out, false, true, OptionsSubSys::getOptions().getBool("save-template.commented"));
            if (OptionsSubSys::getOptions().getBool("verbose")) {
                MsgHandler::getMessageInstance()->inform("Written template to '" + OptionsSubSys::getOptions().getString("save-template") + "'");
            }
            return -4;
        }
    }

    // were the options ok?
    if (!iret) {
        return -3;
    }

    // initialise the output
    // check whether it is a gui-version or not, first
    if (gui) {
        // within gui-based applications, nothing is reported to the console
        MsgHandler::getErrorInstance()->report2cout(false);
        MsgHandler::getErrorInstance()->report2cerr(false);
        MsgHandler::getWarningInstance()->report2cout(false);
        MsgHandler::getWarningInstance()->report2cerr(false);
        MsgHandler::getMessageInstance()->report2cout(false);
        MsgHandler::getMessageInstance()->report2cerr(false);
    } else {
        // within console-based applications, report everything to the console
        MsgHandler::getErrorInstance()->report2cout(false);
        MsgHandler::getErrorInstance()->report2cerr(true);
        MsgHandler::getWarningInstance()->report2cout(false);
        MsgHandler::getWarningInstance()->report2cerr(true);
        MsgHandler::getMessageInstance()->report2cout(true);
        MsgHandler::getMessageInstance()->report2cerr(false);
    }
    // then, check whether be verbose
    if (!gui&&!OptionsSubSys::getOptions().getBool("verbose")) {
        MsgHandler::getMessageInstance()->report2cout(false);
    }
    // check whether to suppress warnings
    if (!gui&&OptionsSubSys::getOptions().getBool("suppress-warnings")) {
        MsgHandler::getWarningInstance()->report2cout(false);
    }
    // build the logger if possible
    if (!gui&&OptionsSubSys::getOptions().isSet("log-file")) {
        myLogFile =
            new LogFile(OptionsSubSys::getOptions().getString("log-file"));
        if (!myLogFile->good()) {
            delete myLogFile;
            myLogFile = 0;
            MsgHandler::getErrorInstance()->inform("Could not build logging file '" + OptionsSubSys::getOptions().getString("log-file") + "'");
            return 1;
        } else {
            MsgHandler::getErrorInstance()->addRetriever(myLogFile);
            MsgHandler::getWarningInstance()->addRetriever(myLogFile);
            MsgHandler::getMessageInstance()->addRetriever(myLogFile);
        }
    }
    // return the state
    if (!gui) {
        RandHelper::initRandGlobal(OptionsSubSys::getOptions());
    }
    return 0;
}


void
SystemFrame::close()
{
    delete myLogFile;
    // close the xml-subsystem
    XMLSubSys::close();
    // delete build program options
    OptionsSubSys::close();
    // delete messages
    MsgHandler::cleanupOnEnd();
}


void
SystemFrame::addConfigurationOptions(OptionsCont &oc)
{
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



/****************************************************************************/


/****************************************************************************/
/// @file    guisim_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Main for GUISIM
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <ctime>
#include <signal.h>
#include <iostream>
#include <fx.h>
#include <fx3d.h>
#include <microsim/MSNet.h>
#include <microsim/MSEmitControl.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/xml/XMLSubSys.h>
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <guisim/GUIEdge.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/gui/images/GUIImageGlobals.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <gui/GUIViewTraffic.h>
#include <guisim/GUIVehicle.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// methods
// ===========================================================================
/* -------------------------------------------------------------------------
 * options initialisation
 * ----------------------------------------------------------------------- */
void
fillOptions() {
    OptionsCont &oc = OptionsCont::getOptions();
    oc.addCallExample("");
    oc.addCallExample("-c <CONFIGURATION>");

    // insert options sub-topics
    oc.addOptionSubTopic("Process");
    oc.addOptionSubTopic("Visualisation");

    oc.doRegister("configuration-file", 'c', new Option_FileName());
    oc.addSynonyme("configuration-file", "configuration");
    oc.addDescription("configuration-file", "Process", "Loads the named config on startup");

    oc.doRegister("quit-on-end", 'Q', new Option_Bool(false));
    oc.addDescription("quit-on-end", "Process", "Quits the gui when the simulation stops");

    oc.doRegister("game", 'G', new Option_Bool(false));
    oc.addDescription("game", "Process", "Start the GUI in gaming mode");

    oc.doRegister("no-start", 'N', new Option_Bool(false));
    oc.addDescription("no-start", "Process", "Does not start the simulation after loading");

    oc.doRegister("disable-textures", 'T', new Option_Bool(false)); // !!!
    oc.addDescription("disable-textures", "Visualisation", "");
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv) {
    // make the output aware of threading
    MFXMutex lock;
    MsgHandler::assignLock(&lock);
    // get the options
    OptionsCont &oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("GUI version of the simulation SUMO.");
#ifdef WIN32
    oc.setApplicationName("guisim.exe", "SUMO guisim Version " + (std::string)VERSION_STRING);
#else
    oc.setApplicationName("sumo-guisim", "SUMO guisim Version " + (std::string)VERSION_STRING);
#endif
    int ret = 0;
#ifndef _DEBUG
    try {
#else
    {
#endif
        // initialise subsystems
        XMLSubSys::init(false);
        fillOptions();
        OptionsIO::getOptions(false, argc, argv);
        // within gui-based applications, nothing is reported to the console
        MsgHandler::getErrorInstance()->report2cout(false);
        MsgHandler::getErrorInstance()->report2cerr(false);
        MsgHandler::getWarningInstance()->report2cout(false);
        MsgHandler::getWarningInstance()->report2cerr(false);
        MsgHandler::getMessageInstance()->report2cout(false);
        MsgHandler::getMessageInstance()->report2cerr(false);
        // Make application
        FXApp application("SUMO GUISimulation", "DLR");
        gFXApp = &application;
        // Open display
        application.init(argc,argv);
        int minor, major;
        if (!FXGLVisual::supported(&application, major, minor)) {
            throw ProcessError("This system has no OpenGL support. Exiting.");
        }
        // initialise global settings
        gQuitOnEnd = oc.getBool("quit-on-end");
        gAllowTextures = !oc.getBool("disable-textures");

        // build the main window
        GUIApplicationWindow * window =
            new GUIApplicationWindow(&application, "*.sumo.cfg");
        window->dependentBuild();
        gSchemeStorage.init(&application);
        // init simulation and visualization structures
        initGuiShapeNames();
        // Create app
        application.addSignal(SIGINT,window, MID_QUIT);
        application.create();
        // Load configuration given on command line
        if (oc.isSet("configuration-file")) {
            window->loadOnStartup(oc.getString("configuration-file"),
                                  !oc.getBool("no-start"), oc.getBool("game"));
        }
        // Run
        ret = application.run();
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    }
#else
    }
#endif
    SystemFrame::close();
    return ret;
}



/****************************************************************************/


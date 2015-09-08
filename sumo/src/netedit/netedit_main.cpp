/****************************************************************************/
/// @file    netedit_main.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// Main for NETEDIT (adapted from guisim_main)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/geom/GeoConvHelper.h>
#include <netimport/NIFrame.h>
#include <netbuild/NBFrame.h>
#include <netwrite/NWFrame.h>

#include "GNEApplicationWindow.h"
#include "GNELoadThread.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// methods
// ===========================================================================


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char** argv) {
    // make the output aware of threading
    MFXMutex lock;
    MsgHandler::assignLock(&lock);
    // get the options
    OptionsCont& oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("Graphical editor for SUMO networks.");
    oc.setApplicationName("netedit.exe", "Netedit Version " + getBuildName(VERSION_STRING));
    int ret = 0;
#ifndef _DEBUG
    try {
#else
    {
#endif
        // initialise subsystems
        XMLSubSys::init();
        OptionsCont& oc = OptionsCont::getOptions();
        GNELoadThread::fillOptions(oc);
        OptionsIO::setArgs(argc, argv);
        OptionsIO::getOptions();
        if (oc.processMetaOptions(false)) {
            SystemFrame::close();
            return 0;
        }
        XMLSubSys::setValidation(oc.getString("xml-validation"), oc.getString("xml-validation.net"));
        // within gui-based applications, nothing is reported to the console
        /*
        MsgHandler::getErrorInstance()->report2cout(false);
        MsgHandler::getErrorInstance()->report2cerr(false);
        MsgHandler::getWarningInstance()->report2cout(false);
        MsgHandler::getWarningInstance()->report2cerr(false);
        MsgHandler::getMessageInstance()->report2cout(false);
        MsgHandler::getMessageInstance()->report2cerr(false);
        */
        // Make application
        FXApp application("Netedit", "DLR");
        // Open display
        application.init(argc, argv);
        int minor, major;
        if (!FXGLVisual::supported(&application, major, minor)) {
            throw ProcessError("This system has no OpenGL support. Exiting.");
        }
        // initialise global settings
        GUITexturesHelper::allowTextures(!oc.getBool("disable-textures"));

        // build the main window
        GNEApplicationWindow* window =
        new GNEApplicationWindow(&application, "*.netc.cfg,*.netccfg");
        window->dependentBuild();
        gSchemeStorage.init(&application);
        // init simulation and visualization structures
        // initGuiShapeNames();
        // Create app
        application.addSignal(SIGINT, window, MID_QUIT);
        application.create();
        // Load configuration given on command line
        if (oc.isSet("configuration-file") || oc.isSet("sumo-net-file")) {
            window->loadOnStartup();
        }
        // Run
        ret = application.run();
#ifndef _DEBUG
    } catch (const std::exception& e) {
        if (std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    SystemFrame::close();
    return ret;
}



/****************************************************************************/


/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    netedit_main.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// Main for NETEDIT (adapted from guisim_main)
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


// ===========================================================================
// main function
// ===========================================================================
int
main(int argc, char** argv) {
    // make the output aware of threading
    MFXMutex lock;
    MsgHandler::assignLock(&lock);
    // get the options
    OptionsCont& oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("Graphical editor for SUMO networks.");
    oc.setApplicationName("netedit", "Netedit Version " VERSION_STRING);
    int ret = 0;
#ifndef _DEBUG
    try {
#else
    {
#endif
        // initialise subsystems
        XMLSubSys::init();
        GNELoadThread::fillOptions(oc);
        OptionsIO::setArgs(argc, argv);
        OptionsIO::getOptions(true);
        if (oc.processMetaOptions(false)) {
            SystemFrame::close();
            return 0;
        }
        // Make application
        FXApp application("Netedit", "DLR");
        // Open display
        application.init(argc, argv);
        int minor, major;
        if (!FXGLVisual::supported(&application, major, minor)) {
            throw ProcessError("This system has no OpenGL support. Exiting.");
        }
        // build the main window
        GNEApplicationWindow* window =
        new GNEApplicationWindow(&application, "*.netc.cfg,*.netccfg");
        gSchemeStorage.init(&application, true);
        window->dependentBuild();
        // Create app
        application.addSignal(SIGINT, window, MID_QUIT);
        application.create();
        // Load configuration given on command line
        if (argc > 1) {
            // Set default options
            OptionsIO::setArgs(argc, argv);
            // load options
            window->loadOptionOnStartup();
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


/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    netedit_main.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// Main for netedit (adapted from guisim_main)
/****************************************************************************/
#include <config.h>

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <signal.h>
#include <utils/common/SystemFrame.h>
#include <utils/foxtools/MsgHandlerSynchronized.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/elements/GNEAttributeCarrier.h>

#include "GNEApplicationWindow.h"
#include "GNELoadThread.h"


// ===========================================================================
// main function
// ===========================================================================
int
main(int argc, char** argv) {
    // make the output aware of threading
    MsgHandler::setFactory(&MsgHandlerSynchronized::create);
    // get the options
    auto& neteditOptions = OptionsCont::getOptions();
    neteditOptions.setApplicationDescription(TL("Graphical editor for SUMO networks, demand and additional infrastructure."));
    neteditOptions.setApplicationName("netedit", "Eclipse SUMO netedit Version " VERSION_STRING);
    // preload registry from sumo to decide on language
    FXRegistry reg("SUMO GUI", "sumo-gui");
    reg.read();
    gLanguage = reg.readStringEntry("gui", "language", gLanguage.c_str());
    int ret = 0;
#ifndef _DEBUG
    try {
#else
    {
#endif
        // initialise subsystems
        XMLSubSys::init();
        // fill options
        GNELoadThread::fillOptions(neteditOptions);
        // set default options
        GNELoadThread::setDefaultOptions(neteditOptions);
        // set arguments called through console
        OptionsIO::setArgs(argc, argv);
        OptionsIO::getOptions(true);
        if (neteditOptions.processMetaOptions(false)) {
            SystemFrame::close();
            return 0;
        }
        if (neteditOptions.isSet("attribute-help-output")) {
            GNEAttributeCarrier::writeAttributeHelp();
            SystemFrame::close();
            return 0;
        }
        // Make application
        FXApp application("SUMO netedit", "netedit");
        // Open display
        application.init(argc, argv);
        int minor, major;
        if (!FXGLVisual::supported(&application, major, minor)) {
            throw ProcessError(TL("This system has no OpenGL support. Exiting."));
        }
        // build the main window
        GNEApplicationWindow* window = new GNEApplicationWindow(&application, "*.netc.cfg,*.netccfg");
        gLanguage = neteditOptions.getString("language");
        gSchemeStorage.init(&application, true);
        window->dependentBuild();
        // Create app
        application.addSignal(SIGINT, window, MID_HOTKEY_CTRL_Q_CLOSE);
        application.create();
        // Load configuration given on command line
        if (argc > 1) {
            // Set default options
            OptionsIO::setArgs(argc, argv);
            // load options
            window->loadOptionOnStartup();
        }
        // focus window at startup
        window->setFocus();
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

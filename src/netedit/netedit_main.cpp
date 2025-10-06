/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @author  Mirko Barthauer
/// @date    Feb 2011
///
// Main for netedit (adapted from guisim_main)
/****************************************************************************/

#include <signal.h>
#include <utils/common/SystemFrame.h>
#include <utils/foxtools/MsgHandlerSynchronized.h>
#include <utils/gui/settings/GUICompleteSchemeStorage.h>
#include <utils/options/OptionsIO.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/dialogs/GNECrashDialog.h>

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include "GNEApplicationWindow.h"
#include "GNEExternalRunner.h"
#include "GNELoadThread.h"
#include "GNETagPropertiesDatabase.h"

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
    neteditOptions.setApplicationName("netedit", "Eclipse SUMO netedit " VERSION_STRING);
    // preload registry from sumo to decide on language
    FXRegistry reg("SUMO GUI", "sumo-gui");
    reg.read();
    // set language
    gLanguage = reg.readStringEntry("gui", "language", gLanguage.c_str());
    // declare return value (0 means all ok, 1 means error)
    int ret = 0;
    // run netedit with try-catch if we're in release mode
#ifndef _DEBUG
    try {
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
        } else {
            // create tagPropertiesdatabase
            const GNETagPropertiesDatabase* tagPropertiesDatabase = new GNETagPropertiesDatabase();
            // check if we're only printing the attributes in console
            if (neteditOptions.isSet("attribute-help-output")) {
                // write attribute help in console
                tagPropertiesDatabase->writeAttributeHelp();
            } else {
                // create FX application
                FXApp application("SUMO netedit", "netedit");
                // Open display
                application.init(argc, argv);
                int minor, major;
                if (!FXGLVisual::supported(&application, major, minor)) {
                    throw ProcessError(TL("This system has no OpenGL support. Exiting."));
                } else {
                    // build the main window
                    GNEApplicationWindow* netedit = new GNEApplicationWindow(&application, tagPropertiesDatabase, "*.netc.cfg,*.netccfg");
#ifndef _DEBUG
                    try {
#endif
                        // build external runner
                        GNEExternalRunner* externalRunner = new GNEExternalRunner(netedit);
                        // set language
                        gLanguage = neteditOptions.getString("language");
                        // initialize GUICompleteSchemeStorage
                        gSchemeStorage.init(&application, true);
                        // build dependent elements
                        netedit->dependentBuild();
                        // add signal handler for CTRL+Q
                        application.addSignal(SIGINT, netedit, MID_HOTKEY_CTRL_Q_CLOSE);
                        // Create app
                        application.create();
                        // Load configuration given on command line
                        if (argc > 1) {
                            // Set default options
                            OptionsIO::setArgs(argc, argv);
                            // load options
                            netedit->loadOptionOnStartup();
                        }
                        // focus window at startup
                        netedit->setFocus();
                        // Run
                        ret = application.run();
                        // delete external runner
                        delete externalRunner;
                        // delete netedit
                        delete netedit;
#ifndef _DEBUG
                    } catch (const ProcessError& e) {
                        // write info
                        if (std::string(e.what()).length() > 0) {
                            WRITE_ERROR(e.what());
                        }
                        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
                        ret = 1;
                        // open crash dialog with the exception
                        GNECrashDialog(netedit, e);
                    }
#endif
                }
            }
            // delete tagPropertiesDatabase
            delete tagPropertiesDatabase;
        }
#ifndef _DEBUG
    } catch (const std::exception& e) {
        if (std::string(e.what()).length() > 0) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
    }
#endif
    // close system frame before return result
    SystemFrame::close();
    return ret;
}

/****************************************************************************/

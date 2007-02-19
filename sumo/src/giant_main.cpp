/****************************************************************************/
/// @file    giant_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Revision 1.16  2007/01/10 08:33:04  dkrajzew
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <ctime>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <fx.h>
#include <fx3d.h>
#include <microsim/MSNet.h>
#include <microsim/MSEmitControl.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/GNEApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <gui/GUIThreadFactory.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <gui/drawerimpl/GUIVehicleDrawer.h>
#include <utils/gui/div/GUIFrame.h>
#include <utils/gui/drawer/GUIGradients.h>
#include <utils/gui/drawer/GUIColorer_SingleColor.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/gui/images/GUIImageGlobals.h>
#include <utils/gui/drawer/GUICompleteSchemeStorage.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// conditionally add library search record to object file for DataReel
#ifdef USE_SOCKETS
#ifdef _WIN32
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _DEBUG
#pragma comment (lib, "gxcoded.lib")
#else // _DEBUG
#pragma comment (lib, "gxcode.lib")
#endif // _DEBUG
#endif // _WIN32
#endif // USE_SOCKETS

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// methods
// ===========================================================================
/* -------------------------------------------------------------------------
 * options initialisation
 * ----------------------------------------------------------------------- */
void
fillOptions(OptionsCont &oc)
{
    // give some application descriptions
    oc.setApplicationDescription("Graphical network editor and simulation.");
#ifdef WIN32
    oc.setApplicationName("guisim.exe");
#else
    oc.setApplicationName("sumo-guisim");
#endif
    oc.addCallExample("");
    oc.addCallExample("-c <CONFIGURATION>");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // fill this subtopic, too
    oc.addOptionSubTopic("Process");
    oc.addOptionSubTopic("Visualisation");
    oc.addOptionSubTopic("Open GL");
    oc.addOptionSubTopic("Report");


    // insert options
    GUIFrame::fillInitOptions(oc);
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
    int ret = 0;
#ifndef _DEBUG
    try {
#endif
        int init_ret = SystemFrame::init(true, argc, argv, fillOptions);
        if (init_ret<0) {
            cout << "SUMO giant" << endl;
            cout << " (c) DLR/ZAIK 2000-2007; http://sumo.sourceforge.net" << endl;
            cout << " Version " << VERSION << endl;
            switch (init_ret) {
            case -2:
                OptionsSubSys::getOptions().printHelp(cout);
                break;
            case -4:
                break;
            default:
                cout << " Use --help to get the list of options." << endl;
            }
            SystemFrame::close();
            return 0;
        } else if (init_ret!=0||!GUIFrame::checkInitOptions(OptionsSubSys::getOptions())) {
            throw ProcessError();
        }
        // Make application
        FXApp application("SUMO GUINetEdit","DLR+ZAIK");
        gFXApp = &application;
        // Open display
        application.init(argc,argv);
        OptionsCont &oc = OptionsSubSys::getOptions();
        int minor, major;
        if (!FXGLVisual::supported(&application, major, minor)) {
            MsgHandler::getErrorInstance()->inform("This system has no OpenGL support. Exiting.");
            throw ProcessError();
        }
        // initialise global settings
        gQuitOnEnd = oc.getBool("quit-on-end");
        gAllowAggregatedFloating = oc.getBool("allow-floating-aggregated-views");
        gAllowAggregated = !oc.getBool("disable-aggregated-views");
        gAllowTextures = !oc.getBool("disable-textures");
        gSuppressEndInfo = oc.getBool("surpress-end-info");

        // build the main window
        GUIThreadFactory tf;
        GNEApplicationWindow * window =
            new GNEApplicationWindow(&application, tf,
                                     oc.getInt("w"), oc.getInt("h"),
                                     oc.getString("configuration-file"));
        gGradients = new GUIGradientStorage(window);
        // delete startup-options
        OptionsSubSys::close();
        // Create app
        application.addSignal(SIGINT,window, MID_QUIT);
        application.create();
        // Run
        ret = application.run();
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    }
#endif
    SystemFrame::close();
    return ret;
}



/****************************************************************************/


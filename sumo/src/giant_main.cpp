/***************************************************************************
                          main.cpp
              The main procedure for the conversion /
              building of networks
                             -------------------
    project              : SUMO
    subproject           : simulation
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.9  2005/11/30 08:56:49  dkrajzew
// final try/catch is now only used in the release version
//
// Revision 1.8  2005/11/29 13:42:03  dkrajzew
// added a minimum simulation speed definition before the simulation ends (unfinished)
//
// Revision 1.7  2005/11/15 10:15:49  dkrajzew
// debugging and beautifying for the next release
//
// Revision 1.6  2005/10/17 09:27:46  dkrajzew
// got rid of the old MSVC memory leak checker
//
// Revision 1.5  2005/10/07 11:48:00  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/23 06:13:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.3  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/07/12 12:55:27  dkrajzew
// build number output added
//
// Revision 1.1  2004/11/23 10:45:07  dkrajzew
// netedit by A. Gaubatz added
//
// Revision 1.1  2004/11/22 13:02:17  dksumo
// 'netedit' and 'giant' added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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
#include <utils/common/HelpPrinter.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/xml/XMLSubSys.h>
#include <netedit/GNEApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <gui/GUIThreadFactory.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <gui/drawerimpl/GUIBaseVehicleDrawer.h>
#include "giant_help.h"
#include "giant_build.h"
#include "sumo_version.h"
#include <utils/gui/div/GUIFrame.h>
#include <utils/gui/drawer/GUIGradients.h>
#include <utils/gui/drawer/GUIColorer_SingleColor.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/gui/images/GUIImageGlobals.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


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
        int init_ret = SystemFrame::init(true, argc, argv,
			GUIFrame::fillInitOptions, GUIFrame::checkInitOptions, help);
        if(init_ret==-1) {
            cout << "SUMO giant" << endl;
            cout << " Version " << version << endl;
            cout << " Build #" << NEXT_BUILD_NUMBER << endl;
            SystemFrame::close();
            return 0;
        } else if(init_ret!=0) {
            throw ProcessError();
        }
        // Make application
        FXApp application("SUMO GUINetEdit","DLR+ZAIK");
        gFXApp = &application;
        // Open display
        application.init(argc,argv);
        OptionsCont &oc = OptionsSubSys::getOptions();
        int minor, major;
        if(!FXGLVisual::supported(&application, major, minor)) {
            MsgHandler::getErrorInstance()->inform(
                "This system has no OpenGL support. Exiting." );
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
                oc.getString("c"));
		gGradients = new GUIGradientStorage(window);
        // delete startup-options
        OptionsSubSys::close();
        // Create app
        application.addSignal(SIGINT,window, MID_QUIT);
        application.create();
        // Run
        ret = application.run();
#ifndef _DEBUG
    } catch(...) {
        WRITE_MESSAGE("Quitting (on error).");
        ret = 1;
    }
#endif
    SystemFrame::close();
    return ret;
}




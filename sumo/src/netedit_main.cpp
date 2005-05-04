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
// Revision 1.2  2005/05/04 09:33:43  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
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
/*
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // Microsoft memory leak detection procedures
//   #define _INC_MALLOC         // exclude standard memory alloc procedures
#ifdef WIN32
   #include <utils/dev/MemDiff.h>
   #include <crtdbg.h>
#endif
#endif
*/
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
#include "netedit_help.h"
#include <utils/gui/div/GUIFrame.h>
#include <utils/gui/drawer/GUIGradients.h>
#include <utils/gui/drawer/GUIColorer_SingleColor.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/gui/images/GUIImageGlobals.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif


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
/*
#ifdef _DEBUG
#ifdef WIN32
    CMemDiff state1;
    // uncomment next line and insert the context of an undeleted
    //  allocation to break within it (MSVC++ only)
    _CrtSetBreakAlloc(376348);
#endif
#endif
*/
    int ret = 0;
    try {
        if(!SystemFrame::init(true, argc, argv,
            GUIFrame::fillInitOptions, GUIFrame::checkInitOptions, help)) {

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
    } catch(GUIThreadFactory) {
        WRITE_MESSAGE("Quitting (on error).");
        ret = 1;
    }
    SystemFrame::close();
    return ret;
}




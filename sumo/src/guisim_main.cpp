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
// Revision 1.1  2004/08/02 13:03:19  dkrajzew
// applied better names
//
// Revision 1.16  2004/07/02 09:49:59  dkrajzew
// generalised for easier online-server implementation
//
// Revision 1.15  2004/04/02 11:32:01  dkrajzew
// first try to optionally disable textures
//
// Revision 1.14  2004/03/19 13:05:30  dkrajzew
// porting to fox
//
// Revision 1.13  2004/02/06 08:54:28  dkrajzew
// _INC_MALLOC definition removed (does not work on MSVC7.0)
//
// Revision 1.12  2003/12/11 06:19:35  dkrajzew
// network loading and initialisation improved
//
// Revision 1.11  2003/10/28 08:35:01  dkrajzew
// random number specification options added
//
// Revision 1.10  2003/10/27 10:47:49  dkrajzew
// added to possibility to close the application after a simulations end
//  without user interaction
//
// Revision 1.9  2003/09/05 14:41:48  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.8  2003/08/14 14:07:55  dkrajzew
// memory allocation control added
//
// Revision 1.7  2003/06/24 08:06:36  dkrajzew
// implemented SystemFrame and applied the changes to all applications
//
// Revision 1.6  2003/06/19 11:03:57  dkrajzew
// debugging
//
// Revision 1.5  2003/06/19 07:07:52  dkrajzew
// false order of calling XML- and Options-subsystems patched
//
// Revision 1.4  2003/06/18 11:26:15  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.3  2003/04/16 09:57:05  dkrajzew
// additional parameter of maximum display size added
//
// Revision 1.2  2003/02/07 10:37:30  dkrajzew
// files updated
//
// Revision 1.1  2002/10/16 14:51:08  dkrajzew
// Moved from ROOT/sumo to ROOT/src; added further help and main files for
//  netconvert, router, od2trips and gui version
//
// Revision 1.9  2002/07/31 17:42:10  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.12  2002/07/22 12:52:23  dkrajzew
// Source handling added
//
// Revision 1.11  2002/07/11 07:30:43  dkrajzew
// Option_FileName invented to allow relative path names within the
//  configuration files; two not yet implemented parameter introduced
//
// Revision 1.10  2002/07/02 12:48:10  dkrajzew
// --help now does not require -c
//
// Revision 1.9  2002/07/02 08:16:19  dkrajzew
// Program flow changed to allow better options removal; return values
//  corrected
//
// Revision 1.8  2002/06/17 15:57:43  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.7  2002/05/14 07:43:51  dkrajzew
// _SPEEDCHECK-methods moved completely to MSNet
//
// Revision 1.6  2002/04/29 13:52:07  dkrajzew
// the program flow, especially the handling of errors improved
//
// Revision 1.5  2002/04/18 06:04:53  dkrajzew
// Forgotten test switch removed
//
// Revision 1.4  2002/04/17 11:20:40  dkrajzew
// Windows-carriage returns removed
//
// Revision 1.3  2002/04/16 12:21:13  dkrajzew
// Usage of SUMO_DATA removed
//
// Revision 1.2  2002/04/15 06:55:47  dkrajzew
// new loading paradigm implemented
//
// Revision 2.6  2002/03/20 08:13:54  dkrajzew
// help-output added
//
// Revision 2.5  2002/03/15 12:45:49  dkrajzew
// Warning is set to true forever due to bugs in value testing when no
//  warnings are used (will be fixed later)
//
// Revision 2.4  2002/03/14 08:09:13  traffic
// Option for no raw output added
//
// Revision 2.3  2002/03/07 07:54:43  traffic
// implemented the usage of stdout as the default raw output
//
// Revision 2.2  2002/03/06 10:12:17  traffic
// Enviroment variable changef from SUMO to SUMO_DATA
//
// Revision 2.1  2002/03/05 14:51:25  traffic
// SegViolation on unset path debugged
//
// Revision 2.0  2002/02/14 14:43:11  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.4  2002/02/13 15:35:33  croessel
// Merging sourceForge with tesseraCVS.
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // Microsoft memory leak detection procedures
//   #define _INC_MALLOC         // exclude standard memory alloc procedures
#ifdef WIN32
   #include <utils/dev/MemDiff.h>
   #include <crtdbg.h>
#endif
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
#include <utils/options/OptionsParser.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/HelpPrinter.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/xml/XMLSubSys.h>
#include <gui/GUIApplicationWindow.h>
#include <gui/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <gui/GUIThreadFactory.h>
#include <gui/GUISUMOAbstractView.h>
#include <gui/drawerimpl/GUIColoringSchemesMap.h>
#include <gui/drawerimpl/GUIBaseVehicleDrawer.h>
#include "guisim_help.h"

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* -------------------------------------------------------------------------
 * build options
 * ----------------------------------------------------------------------- */
void
fillInitOptions(OptionsCont &oc)
{
    oc.doRegister("max-gl-width", 'w', new Option_Integer(1280));
    oc.doRegister("max-gl-height", 'h', new Option_Integer(1024));
    oc.doRegister("quit-on-end", 'Q', new Option_Bool(false));
    oc.doRegister("surpress-end-info", 'S', new Option_Bool(false));
    oc.doRegister("help", '?', new Option_Bool(false));
    oc.doRegister("configuration", 'c', new Option_FileName());
    oc.doRegister("print-options", 'p', new Option_Bool(false));
    oc.doRegister("allow-floating-aggregated-views", 'F', new Option_Bool(false));
    oc.doRegister("disable-aggregated-views", 'A', new Option_Bool(false));
    oc.doRegister("disable-textures", 'T', new Option_Bool(false));
    oc.doRegister("verbose", 'v', new Option_Bool(false)); // !!!
}


bool
checkInitOptions(OptionsCont &oc)
{
    // check whether the parameter are ok
    if(oc.getInt("w")<0||oc.getInt("h")<0) {
        MsgHandler::getErrorInstance()->inform(
            "Both the screen's width and the screen's height must be larger than zero.");
        return false;
    }
    return true;
}


void
initColoringSchemes()
{
    GUIColoringSchemesMap<GUISUMOAbstractView::VehicleColoringScheme> &sm =
        GUIBaseVehicleDrawer::getSchemesMap();
    sm.add("by speed", GUISUMOAbstractView::VCS_BY_SPEED);
    sm.add("specified", GUISUMOAbstractView::VCS_SPECIFIED);
    sm.add("type", GUISUMOAbstractView::VCS_TYPE);
    sm.add("route", GUISUMOAbstractView::VCS_ROUTE);
    sm.add("random#1", GUISUMOAbstractView::VCS_RANDOM1);
    sm.add("random#2", GUISUMOAbstractView::VCS_RANDOM2);
    sm.add("lanechange#1", GUISUMOAbstractView::VCS_LANECHANGE1);
    sm.add("lanechange#2", GUISUMOAbstractView::VCS_LANECHANGE2);
    sm.add("lanechange#3", GUISUMOAbstractView::VCS_LANECHANGE3);
    sm.add("waiting#1", GUISUMOAbstractView::VCS_WAITING1);
//    sm.add("reroute off", GUISUMOAbstractView::VCS_ROUTECHANGEOFFSET);
//    sm.add("reroute #", GUISUMOAbstractView::VCS_ROUTECHANGENUMBER);
    sm.add("lanechange#4", GUISUMOAbstractView::VCS_LANECHANGE4);
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
#ifdef _DEBUG
#ifdef WIN32
    CMemDiff state1;
    // uncomment next line and insert the context of an undeleted
    //  allocation to break within it (MSVC++ only)
    //_CrtSetBreakAlloc(376348);
#endif
#endif
    int ret = 0;
    try {
        if(!SystemFrame::init(true, argc, argv,
            fillInitOptions, checkInitOptions, help)) {
            throw ProcessError();
        }
        // Make application
        FXApp application("SUMO 0.8","DLR+ZAIK");
        gFXApp = &application;
        initColoringSchemes();
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
        GUIApplicationWindow * window =
            new GUIApplicationWindow(&application, tf,
                oc.getInt("w"), oc.getInt("h"),
                oc.getString("c"));
        // delete startup-options
        OptionsSubSys::close();
        // Create app
        application.addSignal(SIGINT,window, MID_QUIT);
        application.create();
        // Run
        ret = application.run();
    } catch(GUIThreadFactory) {
        MsgHandler::getErrorInstance()->inform("Quitting (on error).");
        ret = 1;
    }
    SystemFrame::close();
    return ret;
}




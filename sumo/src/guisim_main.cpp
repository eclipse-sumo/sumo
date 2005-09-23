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
// Revision 1.7  2005/09/23 06:13:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 12:27:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/07/12 12:55:27  dkrajzew
// build number output added
//
// Revision 1.4  2005/05/04 09:33:43  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2005/02/17 10:33:28  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.2  2004/11/23 10:43:28  dkrajzew
// debugging
//
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
/*
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // Microsoft memory leak detection procedures
//   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#ifdef WIN32
   #ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

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
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <gui/GUIThreadFactory.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <gui/drawerimpl/GUIBaseVehicleDrawer.h>
#include "guisim_help.h"
#include "guisim_build.h"
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
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* -------------------------------------------------------------------------
 * build options
 * ----------------------------------------------------------------------- */
void
initColoringSchemes()
{
    GUIColoringSchemesMap<GUISUMOAbstractView::VehicleColoringScheme, GUIVehicle> &sm =
        GUIBaseVehicleDrawer::getSchemesMap();
    sm.add("by speed",
		GUISUMOAbstractView::VCS_BY_SPEED,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
    sm.add("specified",
		GUISUMOAbstractView::VCS_SPECIFIED,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
    sm.add("type",
		GUISUMOAbstractView::VCS_TYPE,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
    sm.add("route",
		GUISUMOAbstractView::VCS_ROUTE,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
    sm.add("random#1",
		GUISUMOAbstractView::VCS_RANDOM1,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
    sm.add("random#2",
		GUISUMOAbstractView::VCS_RANDOM2,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
    sm.add("lanechange#1",
		GUISUMOAbstractView::VCS_LANECHANGE1,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
    sm.add("lanechange#2",
		GUISUMOAbstractView::VCS_LANECHANGE2,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
    sm.add("lanechange#3",
		GUISUMOAbstractView::VCS_LANECHANGE3,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
    sm.add("waiting#1",
		GUISUMOAbstractView::VCS_WAITING1,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
    sm.add("device #",
		GUISUMOAbstractView::VCS_DEVICENO,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
    sm.add("device state",
		GUISUMOAbstractView::VCS_DEVICE_STATE,
			new GUIColorer_SingleColor<GUIVehicle>(RGBColor()));
//    sm.add("reroute off", GUISUMOAbstractView::VCS_ROUTECHANGEOFFSET);
//    sm.add("reroute #", GUISUMOAbstractView::VCS_ROUTECHANGENUMBER);
//    sm.add("lanechange#4", GUISUMOAbstractView::VCS_LANECHANGE4);
	myDensityGradient =
		gGradients->getRGBColors(
			GUIGradientStorage::GRADIENT_GREEN_YELLOW_RED, 101);
	//
}


void
deleteColoringSchemes()
{
    delete &GUIBaseVehicleDrawer::getSchemesMap();
}



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
        int init_ret = SystemFrame::init(true, argc, argv,
			GUIFrame::fillInitOptions, GUIFrame::checkInitOptions, help);
        if(init_ret==-1) {
            cout << "SUMO guisim" << endl;
            cout << " Version " << version << endl;
            cout << " Build #" << NEXT_BUILD_NUMBER << endl;
            SystemFrame::close();
            return 0;
        } else if(init_ret!=0) {
            throw ProcessError();
        }
        // Make application
        FXApp application("SUMO GUISimulation","DLR+ZAIK");
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
        gAllowAggregatedFloating = oc.getBool("allow-SUMORealing-aggregated-views");
        gAllowAggregated = !oc.getBool("disable-aggregated-views");
        gAllowTextures = !oc.getBool("disable-textures");
        gSuppressEndInfo = oc.getBool("surpress-end-info");
        bool useConfig = oc.isSet("c");
        string configFile =
            useConfig ? oc.getString("c") : "";
        bool runAfterLoad = !oc.isSet("no-start");

        // build the main window
        GUIThreadFactory tf;
        GUIApplicationWindow * window =
            new GUIApplicationWindow(&application,
                oc.getInt("w"), oc.getInt("h"), "*.sumo.cfg");
        window->dependentBuild(tf);
		gGradients = new GUIGradientStorage(window);
        initColoringSchemes();
        // delete startup-options
        OptionsSubSys::close();
        // Create app
        application.addSignal(SIGINT,window, MID_QUIT);
        application.create();
        // Load configuration given oncommand line
        if(useConfig) {
            window->loadOnStartup(configFile, runAfterLoad);
        }
        // Run
        ret = application.run();
    } catch(MSNet&) {
        WRITE_MESSAGE("Quitting (on error).");
        ret = 1;
    }
    SystemFrame::close();
    return ret;
}




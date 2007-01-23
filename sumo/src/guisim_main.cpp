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
// $Log: guisim_main.cpp,v $
// Revision 1.23  2007/01/10 08:33:04  dkrajzew
// expanded the some option names when asking for them
//
// Revision 1.22  2006/12/18 14:41:41  dkrajzew
// warnings removed
//
// Revision 1.21  2006/12/18 08:25:24  dkrajzew
// consolidation of setting colors
//
// Revision 1.20  2006/12/12 12:15:24  dkrajzew
// removed simple/full geometry options; everything is now drawn using full geometry
//
// Revision 1.19  2006/11/20 11:11:33  dkrajzew
// bug [ 1598346 ] (Versioning information in many places) patched - Version number is now read from windows_config.h/config.h
//
// Revision 1.18  2006/11/16 10:50:42  dkrajzew
// warnings removed
//
// Revision 1.17  2006/11/13 16:18:49  fxrb
// support for TCP/IP iodevices using DataReel library
//
// Revision 1.16  2006/08/01 07:19:56  dkrajzew
// removed build number information
//
// Revision 1.15  2006/07/06 09:22:08  dkrajzew
// debugging
//
// Revision 1.14  2006/04/18 08:21:48  dkrajzew
// beautifying: output consolidation
//
// Revision 1.13  2006/01/09 13:33:30  dkrajzew
// debugging error handling
//
// Revision 1.12  2005/11/30 08:56:49  dkrajzew
// final try/catch is now only used in the release version
//
// Revision 1.11  2005/11/29 13:42:03  dkrajzew
// added a minimum simulation speed definition before the simulation ends (unfinished)
//
// Revision 1.10  2005/11/15 10:15:49  dkrajzew
// debugging and beautifying for the next release
//
// Revision 1.9  2005/10/17 09:27:46  dkrajzew
// got rid of the old MSVC memory leak checker
//
// Revision 1.8  2005/10/07 11:48:00  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
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
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <gui/GUIGlobals.h>
#include <gui/GUIThreadFactory.h>
#include <guisim/GUIEdge.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <gui/drawerimpl/GUIVehicleDrawer.h>
#include <utils/gui/div/GUIFrame.h>
#include <utils/gui/drawer/GUIGradients.h>
#include <utils/gui/drawer/GUIColorer_SingleColor.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/gui/images/GUIImageGlobals.h>
#include <utils/gui/drawer/GUICompleteSchemeStorage.h>
#include <gui/GUIViewTraffic.h>

#include <gui/GUIColorer_LaneByPurpose.h>
#include <utils/gui/drawer/GUIColorer_LaneBySelection.h>
#include <gui/GUIColorer_LaneByVehKnowledge.h>
#include <gui/GUIColorer_LaneNeighEdges.h>
#include <utils/gui/drawer/GUIColorer_SingleColor.h>
#include <utils/gui/drawer/GUIColorer_ShadeByFunctionValue.h>
#include <utils/gui/drawer/GUIColorer_ShadeByCastedFunctionValue.h>
#include <utils/gui/drawer/GUIColorer_ColorSettingFunction.h>
#include <utils/gui/drawer/GUIColorer_ByDeviceNumber.h>
#include <utils/gui/drawer/GUIColorer_ByOptCORNValue.h>
#include <guisim/GUIVehicle.h>


#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif


// conditionally add library search record to object file for DataReel
#ifdef USE_SOCKETS
#ifdef _WIN32
#ifdef _DEBUG
#pragma comment (lib, "gxcoded.lib")
#else // _DEBUG
#pragma comment (lib, "gxcode.lib")
#endif // _DEBUG
#endif // _WIN32
#endif // USE_SOCKETS

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * methods
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * coloring schemes initialisation
 * ----------------------------------------------------------------------- */
map<int, vector<RGBColor> >
initVehicleColoringSchemes()
{
    map<int, vector<RGBColor> > vehColMap;
    // insert possible vehicle coloring schemes
    GUIColoringSchemesMap<GUIVehicle> &sm = GUIViewTraffic::getVehiclesSchemesMap();
        // from read/assigned colors
    sm.add("given/assigned vehicle color",
        new GUIColorer_ColorSettingFunction<GUIVehicle>(
            (void (GUIVehicle::*)() const) &GUIVehicle::setOwnDefinedColor));
    sm.add("given/assigned type color",
        new GUIColorer_ColorSettingFunction<GUIVehicle>(
            (void (GUIVehicle::*)() const) &GUIVehicle::setOwnTypeColor));
    sm.add("given/assigned route color",
        new GUIColorer_ColorSettingFunction<GUIVehicle>(
            (void (GUIVehicle::*)() const) &GUIVehicle::setOwnRouteColor));
        // from a vehicle's standard values
    sm.add("by speed",
        new GUIColorer_ShadeByFunctionValue<GUIVehicle>(
            0, (SUMOReal) (150.0/3.6), RGBColor(1, 0, 0), RGBColor(0, 0, 1),
            (SUMOReal (GUIVehicle::*)() const) &GUIVehicle::getSpeed));
    sm.add("by waiting time",
        new GUIColorer_ShadeByCastedFunctionValue<GUIVehicle, size_t>(
            0, (SUMOReal) (5*60), RGBColor(0, 0, 1), RGBColor(1, 0, 0),
            (size_t (GUIVehicle::*)() const) &GUIVehicle::getWaitingTime));
    sm.add("by time since last lanechange",
        new GUIColorer_ShadeByCastedFunctionValue<GUIVehicle, size_t>(
            0, (SUMOReal) (5*60), RGBColor(1, 1, 1), RGBColor((SUMOReal) .5, (SUMOReal) .5, (SUMOReal) .5),
            (size_t (GUIVehicle::*)() const) &GUIVehicle::getLastLaneChangeOffset));
    sm.add("by max speed",
        new GUIColorer_ShadeByFunctionValue<GUIVehicle>(
            0, (SUMOReal) (150.0/3.6), RGBColor(1, 0, 0), RGBColor(0, 0, 1),
            (SUMOReal (GUIVehicle::*)() const) &GUIVehicle::getMaxSpeed));
        // ... and some not always used values
    sm.add("by reroute number",
        new GUIColorer_ByOptCORNValue<GUIVehicle, MSCORN::Function>(
            (bool (GUIVehicle::*)(MSCORN::Function) const) &GUIVehicle::hasCORNDoubleValue,
            (SUMOReal (GUIVehicle::*)(MSCORN::Function) const) &GUIVehicle::getCORNDoubleValue,
            true, 1, 10,
            RGBColor(1,0,0), RGBColor(1,1,0), RGBColor(1,1,1),
            MSCORN::CORN_VEH_NUMBERROUTE));
    /*
    sm.add("by time since last reroute",
        new GUIColorer_ByDeviceNumber<GUIVehicle, MSCORN::Function>(
            (bool (GUIVehicle::*)(MSCORN::Function) const) &GUIVehicle::hasCORNDoubleValue,
            (SUMOReal (GUIVehicle::*)(MSCORN::Function) const) &GUIVehicle::getCORNDoubleValue,
            true, 1, 10,
            RGBColor(1,0,0), RGBColor(1,1,0), RGBColor(1,1,1),
            MSCORN::CORN_VEH_NUMBERROUTE));
            */
        // using TOL-extensions
    sm.add("TOL: by device number",
        new GUIColorer_ByOptCORNValue<GUIVehicle, MSCORN::Function>(
            (bool (GUIVehicle::*)(MSCORN::Function) const) &GUIVehicle::hasCORNDoubleValue,
            (SUMOReal (GUIVehicle::*)(MSCORN::Function) const) &GUIVehicle::getCORNDoubleValue,
            true, 1, 10,
            RGBColor(1,0,0), RGBColor(1,1,0), RGBColor(1,1,1),
            MSCORN::CORN_VEH_DEV_NO_CPHONE));
    /*
    sm.add("by device state",
        new GUIColorer_ByOptCORNValue<GUIVehicle, MSCORN::Function>(
            (bool (GUIVehicle::*)(MSCORN::Function) const) &GUIVehicle::hasCORNDoubleValue,
            (SUMOReal (GUIVehicle::*)(MSCORN::Function) const) &GUIVehicle::getCORNDoubleValue,
            true, 1, 10,
            RGBColor(1,0,0), RGBColor(1,1,0), RGBColor(1,1,1),
            MSCORN::CORN_VEH_DEV_NO_CPHONE));
            */
        // using C2C extensions
    sm.add("C2C: by having a device",
        new GUIColorer_ShadeByCastedFunctionValue<GUIVehicle, bool>(
            0, 1, RGBColor((SUMOReal) .5, (SUMOReal) .5, (SUMOReal) .5), RGBColor(1, 1, 1),
            (bool (GUIVehicle::*)() const) &GUIVehicle::isEquipped));
    sm.add("C2C: by number of all information",
        new GUIColorer_ShadeByCastedFunctionValue<GUIVehicle, int>(
            0, (SUMOReal) (10), RGBColor(1, 0, 0), RGBColor(0, 0, 1),
            (int (GUIVehicle::*)() const) &GUIVehicle::getTotalInformationNumber)); // !!!
    sm.add("C2C: by information number",
        new GUIColorer_ShadeByCastedFunctionValue<GUIVehicle, size_t>(
            0, (SUMOReal) (200), RGBColor(1, 0, 0), RGBColor(0, 0, 1),
            (size_t (GUIVehicle::*)() const) &GUIVehicle::getInformationNumber));
    sm.add("C2C: by connections number",
        new GUIColorer_ShadeByCastedFunctionValue<GUIVehicle, size_t>(
            0, (SUMOReal) (3), RGBColor(1, 0, 0), RGBColor(0, 0, 1),
            (size_t (GUIVehicle::*)() const) &GUIVehicle::getConnectionsNumber));
    sm.add("C2C: by having a route information",
        new GUIColorer_ShadeByCastedFunctionValue<GUIVehicle, bool>(
            0, 1, RGBColor((SUMOReal) .5, (SUMOReal) .5, (SUMOReal) .5), RGBColor(1, 1, 1),
            (bool (GUIVehicle::*)() const) &GUIVehicle::hasRouteInformation));
    /*
    sm.add("C2C: by last information time",
        new GUIColorer_ShadeByCastedFunctionValue<GUIVehicle, bool>(
            0, 1, RGBColor((SUMOReal) .5, (SUMOReal) .5, (SUMOReal) .5), RGBColor(1, 1, 1),
            (bool (GUIVehicle::*)() const) &GUIVehicle::getLastInfoTime));
            */
    // build the colors map
    {
        for(size_t i=0; i<sm.size(); i++) {
            vehColMap[i] = vector<RGBColor>();
            switch(sm.getColorSetType(i)) {
            case CST_SINGLE:
                vehColMap[i].push_back(sm.getColorerInterface(i)->getSingleColor());
                break;
            case CST_MINMAX:
                vehColMap[i].push_back(sm.getColorerInterface(i)->getMinColor());
                vehColMap[i].push_back(sm.getColorerInterface(i)->getMaxColor());
                break;
            case CST_MINMAX_OPT:
                vehColMap[i].push_back(sm.getColorerInterface(i)->getMinColor());
                vehColMap[i].push_back(sm.getColorerInterface(i)->getMaxColor());
                vehColMap[i].push_back(sm.getColorerInterface(i)->getFallbackColor());
                break;
            default:
                break;
            }
        }
    }
    return vehColMap;
}


map<int, vector<RGBColor> >
initLaneColoringSchemes()
{
    map<int, vector<RGBColor> > laneColMap;
    // insert possible lane coloring schemes
    GUIColoringSchemesMap<GUILaneWrapper> &sm = GUIViewTraffic::getLaneSchemesMap();
    // insert possible lane coloring schemes
        //
	sm.add("uniform",
        new GUIColorer_SingleColor<GUILaneWrapper>(RGBColor(0, 0, 0)));
	sm.add("by selection (lanewise)",
		new GUIColorer_LaneBySelection<GUILaneWrapper>());
	sm.add("by purpose (lanewise)",
		new GUIColorer_LaneByPurpose<GUILaneWrapper>());
        // from a lane's standard values
	sm.add("by allowed speed (lanewise)",
		new GUIColorer_ShadeByFunctionValue<GUILaneWrapper>(
            0, (SUMOReal) (150.0/3.6),
            RGBColor(1, 0, 0), RGBColor(0, 0, 1),
            (SUMOReal (GUILaneWrapper::*)() const) &GUILaneWrapper::maxSpeed));
	sm.add("by current density (lanewise)",
		new GUIColorer_ShadeByFunctionValue<GUILaneWrapper>(
            0, (SUMOReal) .95,
            RGBColor(0, 1, 0), RGBColor(1, 0, 0),
            (SUMOReal (GUILaneWrapper::*)() const) &GUILaneWrapper::getDensity));
	sm.add("by first vehicle waiting time (lanewise)",
		new GUIColorer_ShadeByFunctionValue<GUILaneWrapper>(
            0, 200,
            RGBColor(0, 1, 0), RGBColor(1, 0, 0),
            (SUMOReal (GUILaneWrapper::*)() const) &GUILaneWrapper::firstWaitingTime));
	sm.add("by lane number (streetwise)",
		new GUIColorer_ShadeByFunctionValue<GUILaneWrapper>(
            0, (SUMOReal) 5,
            RGBColor(1, 0, 0), RGBColor(0, 0, 1),
            (SUMOReal (GUILaneWrapper::*)() const) &GUILaneWrapper::getEdgeLaneNumber));
        // using C2C extensions
    /*
	sm.add("C2C: by vehicle knowledge",
		new GUIColorer_LaneByVehKnowledge<GUILaneWrapper>(this));
	sm.add("C2C: by edge neighborhood",
		new GUIColorer_LaneNeighEdges<GUILaneWrapper>(this));
        */
    // build the colors map
    {
        for(size_t i=0; i<sm.size(); i++) {
            laneColMap[i] = vector<RGBColor>();
            switch(sm.getColorSetType(i)) {
            case CST_SINGLE:
                laneColMap[i].push_back(sm.getColorerInterface(i)->getSingleColor());
                break;
            case CST_MINMAX:
                laneColMap[i].push_back(sm.getColorerInterface(i)->getMinColor());
                laneColMap[i].push_back(sm.getColorerInterface(i)->getMaxColor());
                break;
            case CST_MINMAX_OPT:
                laneColMap[i].push_back(sm.getColorerInterface(i)->getMinColor());
                laneColMap[i].push_back(sm.getColorerInterface(i)->getMaxColor());
                laneColMap[i].push_back(sm.getColorerInterface(i)->getFallbackColor());
                break;
            default:
                break;
            }
        }
    }
    return laneColMap;
}


void
initColoringSchemes()
{
    map<int, vector<RGBColor> > vehColMap = initVehicleColoringSchemes();
    map<int, vector<RGBColor> > laneColMap = initLaneColoringSchemes();
    // initialise gradients
	myDensityGradient = gGradients->getRGBColors(GUIGradientStorage::GRADIENT_GREEN_YELLOW_RED, 101);
	// initialise available coloring schemes
    gSchemeStorage.init(vehColMap, laneColMap);
}


void
deleteColoringSchemes()
{
    delete &GUIViewTraffic::getVehiclesSchemesMap();
    delete &GUIViewTraffic::getLaneSchemesMap();
}


/* -------------------------------------------------------------------------
 * options initialisation
 * ----------------------------------------------------------------------- */
void
fillOptions(OptionsCont &oc)
{
    // give some application descriptions
    oc.setApplicationDescription("GUI version of the simulation SUMO.");
#ifdef WIN32
    oc.setApplicationName("guisim.exe");
#else
    oc.setApplicationName("sumo-guisim");
#endif
    oc.addCallExample("");
    oc.addCallExample("-c <CONFIGURATION>");

    // insert options sub-topics
    oc.addOptionSubTopic("Configuration");
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
#else
    {
#endif
        int init_ret = SystemFrame::init(true, argc, argv, fillOptions);
        if(init_ret<0) {
            cout << "SUMO guisim" << endl;
            cout << " (c) DLR/ZAIK 2000-2007; http://sumo.sourceforge.net" << endl;
            cout << " Version " << VERSION << endl;
            switch(init_ret) {
            case -2:
                OptionsSubSys::getOptions().printHelp(cout);
                break;
            default:
                cout << " Use --help to get the list of options." << endl;
            }
            SystemFrame::close();
            return 0;
        } else if(init_ret!=0||!GUIFrame::checkInitOptions(OptionsSubSys::getOptions())) {
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
            MsgHandler::getErrorInstance()->inform("This system has no OpenGL support. Exiting." );
            throw ProcessError();
        }
        // initialise global settings
        gQuitOnEnd = oc.getBool("quit-on-end");
        gAllowAggregatedFloating = oc.getBool("allow-floating-aggregated-views");
        gAllowAggregated = !oc.getBool("disable-aggregated-views");
        gAllowTextures = !oc.getBool("disable-textures");
        gSuppressEndInfo = oc.getBool("surpress-end-info");
        bool useConfig = oc.isSet("configuration-file");
        string configFile = useConfig ? oc.getString("configuration-file") : "";
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
#ifndef _DEBUG
    } catch(...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    }
#else
    }
#endif
    SystemFrame::close();
    return ret;
}




//---------------------------------------------------------------------------//
//                        GUIVehicle.cpp -
//  A MSVehicle extended by some values for usage within the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.27  2004/12/16 12:20:09  dkrajzew
// debugging
//
// Revision 1.26  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.25  2004/07/02 08:54:11  dkrajzew
// some design issues
//
// Revision 1.24  2004/04/02 11:20:35  dkrajzew
// changes needed to visualise the selection status
//
// Revision 1.23  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.22  2004/01/26 15:53:21  dkrajzew
// added some yet unset display variables
//
// Revision 1.21  2004/01/26 07:00:50  dkrajzew
// reinserted the building of repeating vehicles
//
// Revision 1.20  2003/11/20 13:06:30  dkrajzew
// loading and using of predefined vehicle colors added
//
// Revision 1.19  2003/11/12 13:59:04  dkrajzew
// redesigned some classes by changing them to templates
//
// Revision 1.18  2003/11/11 08:11:05  dkrajzew
// logging (value passing) moved from utils to microsim
//
// Revision 1.17  2003/10/31 08:02:31  dkrajzew
// hope to have patched false usage of RAND_MAX when using gcc
//
// Revision 1.16  2003/10/22 07:07:06  dkrajzew
// patching of lane states on force vehicle removal added
//
// Revision 1.15  2003/08/14 13:47:44  dkrajzew
// false usage of function-pointers patched; false inclusion of .moc-files removed
//
// Revision 1.14  2003/08/04 11:35:52  dkrajzew
// only GUIVehicles need a color definition; process of building cars changed
//
// Revision 1.13  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
// Revision 1.12  2003/07/22 14:59:27  dkrajzew
// changes due to new detector handling
//
// Revision 1.11  2003/06/18 12:54:19  dkrajzew
// has to reapply a changed setting of table parameter
//
// Revision 1.10  2003/06/18 11:30:26  dkrajzew
// debug outputs now use a DEBUG_OUT macro instead of cout; this shall ease the search for further couts which must be redirected to the messaaging subsystem
//
// Revision 1.9  2003/06/06 10:29:24  dkrajzew
// new subfolder holding popup-menus was added due to link-dependencies under linux; QGLObjectPopupMenu*-classes were moved to "popup"
//
// Revision 1.8  2003/06/05 06:29:50  dkrajzew
// first tries to build under linux: warnings removed; moc-files included Makefiles added
//
// Revision 1.7  2003/05/20 09:26:57  dkrajzew
// data retrieval for new views added
//
// Revision 1.6  2003/04/14 08:27:17  dkrajzew
// new globject concept implemented
//
// Revision 1.5  2003/04/09 15:32:28  dkrajzew
// periodical vehicles must have a period over zero now to be reasserted
//
// Revision 1.4  2003/03/20 17:31:41  dkrajzew
// StringUtils moved from utils/importio to utils/common
//
// Revision 1.3  2003/03/20 16:19:28  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.2  2003/02/07 10:39:17  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cmath>
#include <vector>
#include <string>
#include <utils/common/StringUtils.h>
#include <microsim/MSVehicle.h>
#include "GUINet.h"
#include "GUIVehicle.h"
#include <gui/GUIApplicationWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <gui/GUIGlobals.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSVehicleControl.h>
#include <utils/foxtools/MFXMenuHeader.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <gui/GUIViewTraffic.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member variables
 * ======================================================================= */
RGBColor GUIVehicle::_laneChangeColor1;
RGBColor GUIVehicle::_laneChangeColor2;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIVehicle::GUIVehiclePopupMenu) GUIVehiclePopupMenuMap[]=
{
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute),
};

// Object implementation
FXIMPLEMENT(GUIVehicle::GUIVehiclePopupMenu, GUIGLObjectPopupMenu, GUIVehiclePopupMenuMap, ARRAYNUMBER(GUIVehiclePopupMenuMap))


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * GUIVehicle::GUIVehiclePopupMenu - methods
 * ----------------------------------------------------------------------- */
GUIVehicle::GUIVehiclePopupMenu::GUIVehiclePopupMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent,
        GUIGlObject &o)
    : GUIGLObjectPopupMenu(app, parent, o)
{
}


GUIVehicle::GUIVehiclePopupMenu::~GUIVehiclePopupMenu()
{
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->showRoute(
        static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->hideRoute(
        static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->showRoute(
        static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->hideRoute(
        static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}

#ifdef NETWORKING_BLA
ofstream networking_endOut("end.txt");
ofstream networking_stepOut("step.txt");
ofstream networking_knownOut("known.txt");
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#endif

/* =========================================================================
 * method definitions
 * ======================================================================= */

GUIVehicle::GUIVehicle( GUIGlObjectStorage &idStorage,
                       std::string id, MSRoute* route,
                       MSNet::Time departTime,
                       const MSVehicleType* type,
                       size_t noMeanData,
                       int repNo, int repOffset,
                       const RGBColor &color)
    : MSVehicle(id, route, departTime, type, noMeanData, repNo, repOffset),
    GUIGlObject(idStorage, string("vehicle:")+id), myDefinedColor(color)
#ifdef NETWROKING_BLA
    ,networking_globalConns(0)
#endif
{
    // compute both random colors
    //  color1
    long prod = 1;
    for(size_t i=0; i<id.length(); i++) {
        prod *= (int) id.at(i);
        if(prod>(1<<24)) {
            prod /= 128;
        }
    }
    _randomColor1 = RGBColor(
        (double) (256-(prod & 255)) / (double) 255,
        (double) (256-((prod>>8) & 255)) / (double) 255,
        (double) (256-((prod>>16) & 255)) / (double) 255);
    // color2
    _randomColor2 = RGBColor(
        (double)rand() / ( static_cast<double>(RAND_MAX) + 1),
        (double)rand() / ( static_cast<double>(RAND_MAX) + 1),
        (double)rand() / ( static_cast<double>(RAND_MAX) + 1));
    // lane change color (static!!!)
    _laneChangeColor1 = RGBColor(1, 1, 1);
    _laneChangeColor2 = RGBColor(0.7, 0.7, 0.7);

#ifdef NETWORKING_BLA
    float prob = OptionsSubSys::getOptions().getFloat("device");
    if(prob>(double) rand() / (double) RAND_MAX) {
        networking_HaveDevice = true;
    } else {
        networking_HaveDevice = false;
    }
    if(id==OptionsSubSys::getOptions().getString("knownveh")) {
        networking_HaveDevice = true;
    }
#endif
}


GUIVehicle::~GUIVehicle()
{
#ifdef NETWORKING_BLA
    if(networking_HaveDevice) {
        networking_endOut
            << id() << ";" << MSNet::globaltime << ";"
            << networking_myKnownEdges.size() << ";"
			<< networking_globalConns << endl;
    }
#endif
}


std::vector<std::string>
GUIVehicle::getNames()
{
    std::vector<std::string> ret;
    ret.reserve(MSVehicle::myDict.size());
    for(MSVehicle::DictType::iterator i=MSVehicle::myDict.begin();
        i!=MSVehicle::myDict.end(); i++) {
        MSVehicle *veh = (*i).second;
        if(veh->running()) {
            ret.push_back((*i).first);
        }
    }
    return ret;
}


std::vector<size_t>
GUIVehicle::getIDs()
{
    std::vector<size_t> ret;
    ret.reserve(MSVehicle::myDict.size());
    for(MSVehicle::DictType::iterator i=MSVehicle::myDict.begin();
        i!=MSVehicle::myDict.end(); i++) {
        MSVehicle *veh = (*i).second;
        if(veh->running()) {
            ret.push_back(static_cast<GUIVehicle*>((*i).second)->getGlID());
        }
    }
    return ret;
}


const RGBColor &
GUIVehicle::getDefinedColor() const
{
    return myDefinedColor;
}


const RGBColor &
GUIVehicle::getRandomColor1() const
{
    return _randomColor1;
}


const RGBColor &
GUIVehicle::getRandomColor2() const
{
    return _randomColor2;
}


int
GUIVehicle::getPassedColor() const
{
    int passed = 255 - myLastLaneChangeOffset;
    if(passed<128) {
        passed = 128;
    }
    return passed;
}


const RGBColor &
GUIVehicle::getLaneChangeColor2() const
{
    if(myLastLaneChangeOffset==0) {
        return _laneChangeColor1;
    } else {
        return _laneChangeColor2;
    }
}


MSVehicle *
GUIVehicle::getNextPeriodical() const
{
    // check whether another one shall be repated
    if(myRepetitionNumber<=0) {
        return 0;
    }
    return MSNet::getInstance()->getVehicleControl().buildVehicle(
        StringUtils::version1(myID), myRoute, myDesiredDepart+myPeriod,
        myType, myRepetitionNumber-1, myPeriod, myDefinedColor);
}


GUIGLObjectPopupMenu *
GUIVehicle::getPopUpMenu(GUIMainWindow &app,
                         GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIVehiclePopupMenu(app, parent, *this);
    new MFXMenuHeader(ret, app.getBoldFont(), getFullName().c_str(), 0, 0, 0);
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Center",
        GUIIconSubSys::getIcon(ICON_RECENTERVIEW), ret, MID_CENTER);
    new FXMenuSeparator(ret);
    //
    if(gSelected.isSelected(GLO_VEHICLE, getGlID())) {
        new FXMenuCommand(ret, "Remove From Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_MINUS), ret, MID_REMOVESELECT);
    } else {
        new FXMenuCommand(ret, "Add To Selected",
            GUIIconSubSys::getIcon(ICON_FLAG_PLUS), ret, MID_ADDSELECT);
    }
    new FXMenuSeparator(ret);
    //
    new FXMenuCommand(ret, "Show Parameter", 0, ret, MID_SHOWPARS);
    if(static_cast<GUIViewTraffic&>(parent).amShowingRouteFor(this, 0)) {
        new FXMenuCommand(ret, "Hide Current Route", 0, ret, MID_HIDE_CURRENTROUTE);
    } else {
        new FXMenuCommand(ret, "Show Current Route", 0, ret, MID_SHOW_CURRENTROUTE);
    }
    if(static_cast<GUIViewTraffic&>(parent).amShowingRouteFor(this, -1)) {
        new FXMenuCommand(ret, "Hide All Routes", 0, ret, MID_HIDE_ALLROUTES);
    } else {
        new FXMenuCommand(ret, "Show All Routes", 0, ret, MID_SHOW_ALLROUTES);
    }
    return ret;
}


GUIParameterTableWindow *
GUIVehicle::getParameterWindow(GUIMainWindow &app,
                               GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 9);
    // add items
    ret->mkItem("type [NAME]", false, myType->id());
    ret->mkItem("left same route [#]", false, getRepetitionNo());
    ret->mkItem("emission period [s]", false, getPeriod());
    ret->mkItem("waiting time [s]", true,
        new CastingFunctionBinding<MSVehicle, double, size_t>(
            this, &MSVehicle::getWaitingTime));
    ret->mkItem("last lane change [s]", true,
        new CastingFunctionBinding<GUIVehicle, double, size_t>(
        this, &GUIVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, getDesiredDepart());
    ret->mkItem("position [m]", true,
        new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::pos));
    ret->mkItem("speed [m/s]", true,
        new FunctionBinding<GUIVehicle, double>(this, &GUIVehicle::speed));
    // close building
    ret->closeBuilding();
    return ret;
}



GUIGlObjectType
GUIVehicle::getType() const
{
    return GLO_VEHICLE;
}


std::string
GUIVehicle::microsimID() const
{
    return id();
}


bool
GUIVehicle::active() const
{
    return running();
}


void
GUIVehicle::setRemoved()
{
    myLane = 0;
}


int
GUIVehicle::getRepetitionNo() const
{
    return myRepetitionNumber;
}


int
GUIVehicle::getPeriod() const
{
    return myPeriod;
}


size_t
GUIVehicle::getLastLaneChangeOffset() const
{
    return myLastLaneChangeOffset;
}


size_t
GUIVehicle::getDesiredDepart() const
{
    return myDesiredDepart;
}


Boundary
GUIVehicle::getCenteringBoundary() const
{
	throw 1;
}

#ifdef NETWORKING_BLA


#endif


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



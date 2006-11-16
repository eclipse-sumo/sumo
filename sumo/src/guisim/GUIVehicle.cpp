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
// Revision 1.47  2006/11/16 10:50:44  dkrajzew
// warnings removed
//
// Revision 1.46  2006/11/14 13:01:53  dkrajzew
// warnings removed
//
// Revision 1.45  2006/10/12 07:57:14  dkrajzew
// added the possibility to copy an artefact's (gl-object's) name to clipboard (windows)
//
// Revision 1.44  2006/10/04 13:18:14  dkrajzew
// debugging internal lanes, multiple vehicle emission and net building
//
// Revision 1.43  2006/09/18 10:02:18  dkrajzew
// removed deprecated c2c functions, added new made by Danilot Boyom
//
// Revision 1.42  2006/07/06 06:40:38  dkrajzew
// applied current microsim-APIs
//
// Revision 1.41  2006/05/15 05:51:04  dkrajzew
// debugged the id retrieval usage
//
// Revision 1.41  2006/05/08 11:01:17  dkrajzew
// debugging: all structures now return their id via getID()
//
// Revision 1.40  2006/04/18 08:12:04  dkrajzew
// consolidation of interaction with gl-objects
//
// Revision 1.39  2006/04/11 10:56:32  dkrajzew
// microsimID() now returns a const reference
//
// Revision 1.38  2006/04/05 05:22:36  dkrajzew
// retrieval of microsim ids is now also done using getID() instead of id()
//
// Revision 1.37  2006/03/28 06:12:54  dkrajzew
// unneeded string wrapping removed
//
// Revision 1.36  2006/02/27 12:14:36  dkrajzew
// further work on the dfrouter
//
// Revision 1.35  2006/01/09 11:50:21  dkrajzew
// new visualization settings implemented
//
// Revision 1.34  2005/12/01 07:33:44  dkrajzew
// introducing bus stops: eased building vehicles; vehicles may now have nested elements
//
// Revision 1.33  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.32  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.31  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.30  2005/05/04 08:05:24  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.29  2005/02/01 10:10:39  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.28  2005/01/27 14:20:56  dkrajzew
// code beautifying
//
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
// false usage of function-pointers patched; false inclusion of
//  .moc-files removed
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
// debug outputs now use a DEBUG_OUT macro instead of cout; this shall ease
//  the search for further couts which must be redirected to the messaaging
//  subsystem
//
// Revision 1.9  2003/06/06 10:29:24  dkrajzew
// new subfolder holding popup-menus was added due to link-dependencies under
//  linux; QGLObjectPopupMenu*-classes were moved to "popup"
//
// Revision 1.8  2003/06/05 06:29:50  dkrajzew
// first tries to build under linux: warnings removed; moc-files included
//  Makefiles added
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
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/MSVehicleControl.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <gui/GUIViewTraffic.h>
#include <guisim/GUIVehicleType.h>
#include <guisim/GUIRoute.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * FOX callback mapping
 * ======================================================================= */
FXDEFMAP(GUIVehicle::GUIVehiclePopupMenu) GUIVehiclePopupMenuMap[]=
{
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdShowAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_ALLROUTES, GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes),
    FXMAPFUNC(SEL_COMMAND, MID_SHOW_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_HIDE_CURRENTROUTE, GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute),
    FXMAPFUNC(SEL_COMMAND, MID_START_TRACK, GUIVehicle::GUIVehiclePopupMenu::onCmdStartTrack),
    FXMAPFUNC(SEL_COMMAND, MID_STOP_TRACK, GUIVehicle::GUIVehiclePopupMenu::onCmdStopTrack),
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
    static_cast<GUIViewTraffic*>(myParent)->showRoute(static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideAllRoutes(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->hideRoute(static_cast<GUIVehicle*>(myObject), -1);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdShowCurrentRoute(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->showRoute(static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}


long
GUIVehicle::GUIVehiclePopupMenu::onCmdHideCurrentRoute(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->hideRoute(static_cast<GUIVehicle*>(myObject), 0);
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdStartTrack(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->startTrack(static_cast<GUIVehicle*>(myObject)->getGlID());
    return 1;
}

long
GUIVehicle::GUIVehiclePopupMenu::onCmdStopTrack(FXObject*,FXSelector,void*)
{
    assert(myObject->getType()==GLO_VEHICLE);
    static_cast<GUIViewTraffic*>(myParent)->stopTrack();
    return 1;
}


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIVehicle::GUIVehicle( GUIGlObjectStorage &idStorage,
                       std::string id, MSRoute* route,
                       SUMOTime departTime,
                       const MSVehicleType* type,
                       int repNo, int repOffset)
    : MSVehicle(id, route, departTime, type, repNo, repOffset),
    GUIGlObject(idStorage, "vehicle:"+id)
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
        (SUMOReal) (256-(prod & 255)) / (SUMOReal) 255,
        (SUMOReal) (256-((prod>>8) & 255)) / (SUMOReal) 255,
        (SUMOReal) (256-((prod>>16) & 255)) / (SUMOReal) 255);
    // color2
    _randomColor2 = RGBColor(
        (SUMOReal)rand() / ( static_cast<SUMOReal>(RAND_MAX) + 1),
        (SUMOReal)rand() / ( static_cast<SUMOReal>(RAND_MAX) + 1),
        (SUMOReal)rand() / ( static_cast<SUMOReal>(RAND_MAX) + 1));
}


GUIVehicle::~GUIVehicle()
{
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


SUMOReal
GUIVehicle::getTimeSinceLastLaneChangeAsReal() const
{
    return (SUMOReal) myLastLaneChangeOffset;
}


MSVehicle *
GUIVehicle::getNextPeriodical() const
{
    // check whether another one shall be repated
    if(myRepetitionNumber<=0) {
        return 0;
    }
    MSVehicle *ret = MSNet::getInstance()->getVehicleControl().buildVehicle(
        StringUtils::version1(myID), myRoute, myDesiredDepart+myPeriod,
        myType, myRepetitionNumber-1, myPeriod);
    for(std::list<Stop>::const_iterator i=myStops.begin(); i!=myStops.end(); ++i) {
        ret->myStops.push_back(*i);
    }
    if(hasCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_RED)) {
        ret->setCORNColor(
            (SUMOReal) getCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_RED),
            (SUMOReal) getCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_GREEN),
            (SUMOReal) getCORNDoubleValue(MSCORN::CORN_VEH_OWNCOL_BLUE));
    }
    return ret;
}


GUIGLObjectPopupMenu *
GUIVehicle::getPopUpMenu(GUIMainWindow &app,
                         GUISUMOAbstractView &parent)
{
    GUIGLObjectPopupMenu *ret = new GUIVehiclePopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    //
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
    new FXMenuSeparator(ret);
    GUIViewTraffic *view = dynamic_cast<GUIViewTraffic*>(&parent);
    if(view!=0) {
        int trackedID = view->getTrackedID();
        if(trackedID<0||(size_t)trackedID!=getGlID()) {
            new FXMenuCommand(ret, "Start Tracking", 0, ret, MID_START_TRACK);
        } else {
            new FXMenuCommand(ret, "Stop Tracking", 0, ret, MID_STOP_TRACK);
        }
        new FXMenuSeparator(ret);
    }
    //
    buildShowParamsPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUIVehicle::getParameterWindow(GUIMainWindow &app,
                               GUISUMOAbstractView &)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 9);
    // add items
    ret->mkItem("type [NAME]", false, myType->getID());
    ret->mkItem("left same route [#]", false, (SUMOReal) getRepetitionNo());
    ret->mkItem("emission period [s]", false, (SUMOReal) getPeriod());
    ret->mkItem("waiting time [s]", true,
        new CastingFunctionBinding<MSVehicle, SUMOReal, size_t>(this, &MSVehicle::getWaitingTime));
    ret->mkItem("last lane change [s]", true,
        new CastingFunctionBinding<GUIVehicle, SUMOReal, size_t>(this, &GUIVehicle::getLastLaneChangeOffset));
    ret->mkItem("desired depart [s]", false, (SUMOReal) getDesiredDepart());
    ret->mkItem("position [m]", true,
        new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getPositionOnLane));
    ret->mkItem("speed [m/s]", true,
        new FunctionBinding<GUIVehicle, SUMOReal>(this, &GUIVehicle::getSpeed));
    // close building
    ret->closeBuilding();
    return ret;
}



GUIGlObjectType
GUIVehicle::getType() const
{
    return GLO_VEHICLE;
}


const std::string &
GUIVehicle::microsimID() const
{
    return getID();
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



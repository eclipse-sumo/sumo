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
#include <qwidget.h>
#include <gui/GUISUMOAbstractView.h>
#include <gui/popup/QGLObjectPopupMenu.h>
#include <gui/TableTypes.h>
#include <gui/popup/QGLObjectPopupMenuItem.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member variables
 * ======================================================================= */
float GUIVehicle::_laneChangeColor1[3];
float GUIVehicle::_laneChangeColor2[3];

const char * const
GUIVehicle::myTableItems[] =
{
    "speed", "position",
    "desired depart", "real depart",
    "last lane change", "waiting time",
    "emission period", "following vehicles",
    "type",
    0
};

const TableType
GUIVehicle::myTableItemTypes[] =
{
    TT_DOUBLE, TT_DOUBLE,
    TT_DOUBLE, TT_DOUBLE,
    TT_DOUBLE, TT_DOUBLE,
    TT_DOUBLE, TT_DOUBLE,
    TT_MENU_END
};

size_t GUIVehicle::myParamCounterHelp = 0;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUIVehicle::GUIVehicle( GUIGlObjectStorage &idStorage,
                       std::string id, MSRoute* route,
                       MSNet::Time departTime,
                       const MSVehicleType* type,
                       size_t noMeanData,
                       int repNo, int repOffset,
                       float *defColor)
    : MSVehicle(id, route, departTime, type, noMeanData, repNo, repOffset),
    GUIGlObject(idStorage, string("vehicle:")+id)
{
    // copy the defined color
    if(defColor!=0) {
        _definedColor[0] = defColor[0];
        _definedColor[1] = defColor[1];
        _definedColor[2] = defColor[2];
    }
    // compute both random colors
    //  color1
    long prod = 1;
    for(size_t i=0; i<id.length(); i++) {
        prod *= (int) id.at(i);
        if(prod>(1<<24)) {
            prod /= 128;
        }
    }
    _randomColor1[0] = (double) (256-(prod & 255)) / (double) 255;
    _randomColor1[1] = (double) (256-((prod>>8) & 255)) / (double) 255;
    _randomColor1[2] = (double) (256-((prod>>16) & 255)) / (double) 255;
    // color2
    _randomColor2[0] = (double)rand() / (double)(RAND_MAX);
    _randomColor2[1] = (double)rand() / (double)(RAND_MAX);
    _randomColor2[2] = (double)rand() / (double)(RAND_MAX);
    // lane change color (static!!!)
    _laneChangeColor1[0] = 1;
    _laneChangeColor1[1] = 1;
    _laneChangeColor1[2] = 1;
    _laneChangeColor2[0] = float(0.7);
    _laneChangeColor2[1] = float(0.7);
    _laneChangeColor2[2] = float(0.7);
}


GUIVehicle::~GUIVehicle()
{
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


const float *
GUIVehicle::getDefinedColor() const
{
    return _definedColor;
}


const float *
GUIVehicle::getRandomColor1() const
{
    return _randomColor1;
}


const float *
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


const float *
GUIVehicle::getLaneChangeColor2() const
{
    if(myLastLaneChangeOffset==0) {
        return _laneChangeColor1;
    } else {
        return _laneChangeColor2;
    }
}


long
GUIVehicle::getWaitingTime() const
{
    return myWaitingTime;
}


MSVehicle *
GUIVehicle::getNextPeriodical() const
{
    // check whether another one shall be repated
    if(myRepetitionNumber<=0) {
        return 0;
    }
    return GUINet::getInstance()->buildNewVehicle(StringUtils::version1(myID),
        myRoute, myDesiredDepart+myPeriod, myType, myRepetitionNumber-1,
        myPeriod, 0);
}


QGLObjectPopupMenu *
GUIVehicle::getPopUpMenu(GUIApplicationWindow *app,
                         GUISUMOAbstractView *parent)
{
    int id;
    QGLObjectPopupMenu *ret =
        new QGLObjectPopupMenu(app, parent, this);
    // insert name
    id = ret->insertItem(
        new QGLObjectPopupMenuItem(ret, getFullName().c_str(), true));
    ret->insertSeparator();
    // add view options
    id = ret->insertItem("Center", ret, SLOT(center()));
    ret->setItemEnabled(id, TRUE);
    id = ret->insertItem("Track");
    ret->setItemEnabled(id, FALSE);
    id = ret->insertItem("Stop");
    ret->setItemEnabled(id, FALSE);
    id = ret->insertItem("Delete");
    ret->setItemEnabled(id, FALSE);
    ret->insertSeparator();
    // add views adding options
    id = ret->insertItem("Show Parameter", ret, SLOT(showPars()));
    ret->setItemEnabled(id, TRUE);
    ret->insertSeparator();
    id = ret->insertItem("Open ValueTracker");
    ret->setItemEnabled(id, FALSE);
    ret->insertSeparator();
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


const char * const
GUIVehicle::getTableItem(size_t pos) const
{
    if(myParamCounterHelp!=0&&pos==getTableParameterNo()-1) {
        return "type";
    }
    if(myParamCounterHelp!=0&&pos>=myParamCounterHelp) {
        return myType->getTableItem(pos-myParamCounterHelp);
    }
    return myTableItems[pos];
}


TableType
GUIVehicle::getTableType(size_t pos) const
{
    if(myParamCounterHelp!=0&&pos==getTableParameterNo()-1) {
        return TT_MENU_BEGIN;
    }
    if(myParamCounterHelp!=0&&pos>=myParamCounterHelp) {
        return myType->getTableType(pos-myParamCounterHelp);
    }
    return myTableItemTypes[pos];
}


void
GUIVehicle::fillTableParameter(double *parameter) const
{
    parameter[0] = myState.speed();
    parameter[1] = myState.pos();
    parameter[2] = myDesiredDepart;
    parameter[3] = -1;
    parameter[4] = myLastLaneChangeOffset;
    parameter[5] = myWaitingTime;
    parameter[6] = myPeriod;
    parameter[7] = myRepetitionNumber;
	myType->fillTableParameter(parameter+9);
}

double
GUIVehicle::getTableParameter(size_t pos) const
{
    if(myParamCounterHelp!=0&&pos>=myParamCounterHelp) {
        return myType->getTableParameter(pos-myParamCounterHelp);
    }
    switch(pos) {
    case 0:
        return myState.speed();
    case 1:
        return myState.pos();
    case 2:
        return myDesiredDepart;
    case 3:
        return -1;
    case 4:
        return myLastLaneChangeOffset;
    case 5:
        return myWaitingTime;
    case 6:
        return myPeriod;
    case 7:
        return myRepetitionNumber;
    default:
        throw 1;
    }
}


size_t
GUIVehicle::getTableParameterNo() const
{
    if(myParamCounterHelp==0) {
        myParamCounterHelp = GUIGlObject::getTableParameterNo();
    }
    return myParamCounterHelp
        + myType->getNoParameter()
        + 1;
}


const char *
GUIVehicle::getTableBeginValue(size_t pos) const
{
    return myType->id().c_str();
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUIVehicle.icc"
//#endif

// Local Variables:
// mode:C++
// End:



//---------------------------------------------------------------------------//
//                        GUINetWrapper.cpp -
//  No geometrical information is hold, here. Still, some methods for
//      displaying network-information are stored in here
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                :
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
// Revision 1.5  2003/12/11 06:24:55  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.4  2003/11/12 13:59:04  dkrajzew
// redesigned some classes by changing them to templates
//
// Revision 1.3  2003/11/11 08:11:05  dkrajzew
// logging (value passing) moved from utils to microsim
//
// Revision 1.2  2003/08/14 13:47:44  dkrajzew
// false usage of function-pointers patched; false inclusion of .moc-files removed
//
// Revision 1.1  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <iostream> // !!!
#include <utility>
#include <utils/qutils/NewQMutex.h>
#include <guisim/GUINet.h>
#include <gui/GUISUMOAbstractView.h>
#include "GUINetWrapper.h"
#include <gui/popup/QGLObjectPopupMenu.h>
#include <gui/partable/GUIParameterTableWindow.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <gui/popup/QGLObjectPopupMenuItem.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
GUINetWrapper::GUINetWrapper( GUIGlObjectStorage &idStorage, GUINet &net)
    : GUIGlObject(idStorage, string("network")),
    myNet(net)
{
}


GUINetWrapper::~GUINetWrapper()
{
}


QGLObjectPopupMenu *
GUINetWrapper::getPopUpMenu(GUIApplicationWindow &app,
                                 GUISUMOAbstractView &parent)
{
    QGLObjectPopupMenu *ret = new QGLObjectPopupMenu(app, parent, *this);
    int id;
    // insert name
    id = ret->insertItem(
        new QGLObjectPopupMenuItem(ret, getFullName().c_str(), true));
    ret->insertSeparator();
    // add view option
    id = ret->insertItem("Center", ret, SLOT(center()));
    ret->setItemEnabled(id, TRUE);
    ret->insertSeparator();
    // add parameter option
    id = ret->insertItem("Show Parameter", ret, SLOT(showPars()));
    ret->setItemEnabled(id, TRUE);
    return ret;
}


GUIParameterTableWindow *
GUINetWrapper::getParameterWindow(GUIApplicationWindow &app,
                                       GUISUMOAbstractView &parent)
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this);
    // add items
    ret->mkItem("vehicles running [#]", true,
        new CastingFunctionBinding<MSVehicleControl, double, size_t>(
            &(getNet().getVehicleControl()),
            &MSVehicleControl::getRunningVehicleNo));
    ret->mkItem("vehicles ended [#]", true,
        new CastingFunctionBinding<MSVehicleControl, double, size_t>(
            &(getNet().getVehicleControl()),
            &MSVehicleControl::getEndedVehicleNo));
    ret->mkItem("vehicles emitted [#]", true,
        new CastingFunctionBinding<MSVehicleControl, double, size_t>(
            &(getNet().getVehicleControl()),
            &MSVehicleControl::getEmittedVehicleNo));
    ret->mkItem("vehicles loaded [#]", true,
        new CastingFunctionBinding<MSVehicleControl, double, size_t>(
            &(getNet().getVehicleControl()),
            &MSVehicleControl::getLoadedVehicleNo));
    ret->mkItem("end time [s]", false,
        OptionsSubSys::getOptions().getInt("e"));
    ret->mkItem("begin time [s]", false,
        OptionsSubSys::getOptions().getInt("b"));
    ret->mkItem("time step [s]", true,
        new CastingFunctionBinding<GUINet, double, size_t>(
            &(getNet()), &GUINet::getCurrentTimeStep));
    // close building
    ret->closeBuilding();
    return ret;
}



GUIGlObjectType
GUINetWrapper::getType() const
{
    return GLO_NETWORK;
}


std::string
GUINetWrapper::microsimID() const
{
    return "";
}


Boundery
GUINetWrapper::getBoundery() const
{
    return myNet.getBoundery();
}


GUINet &
GUINetWrapper::getNet() const
{
    return myNet;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "GUINetWrapper.icc"
//#endif

// Local Variables:
// mode:C++
// End:



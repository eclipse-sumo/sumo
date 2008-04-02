/****************************************************************************/
/// @file    GUINetWrapper.cpp
/// @author  Daniel Krajzewicz
/// @date
/// @version $Id$
///
// No geometrical information is hold, here. Still, some methods for
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream> // !!!
#include <utility>
#include <guisim/GUINet.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include "GUINetWrapper.h"
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/common/StringUtils.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/logging/CastingFunctionBinding.h>
#include <microsim/logging/FunctionBinding.h>
#include <microsim/logging/FuncBinding_IntParam.h>
#include <utils/options/OptionsCont.h>
#include <gui/GUIApplicationWindow.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
GUINetWrapper::GUINetWrapper(GUIGlObjectStorage &idStorage, GUINet &net) throw()
        : GUIGlObject(idStorage, "network"),
        myNet(net)
{}


GUINetWrapper::~GUINetWrapper() throw()
{}


GUIGLObjectPopupMenu *
GUINetWrapper::getPopUpMenu(GUIMainWindow &app,
                            GUISUMOAbstractView &parent) throw()
{
    GUIGLObjectPopupMenu *ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    buildShowParamsPopupEntry(ret, false);
    return ret;
}


GUIParameterTableWindow *
GUINetWrapper::getParameterWindow(GUIMainWindow &app,
                                  GUISUMOAbstractView &) throw()
{
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *this, 13);
    // add items
    ret->mkItem("vehicles running [#]", true,
                new FunctionBinding<MSVehicleControl, unsigned int>(&(getNet().getVehicleControl()), &MSVehicleControl::getRunningVehicleNo));
    ret->mkItem("vehicles ended [#]", true,
                new FunctionBinding<MSVehicleControl, unsigned int>(&(getNet().getVehicleControl()), &MSVehicleControl::getEndedVehicleNo));
    ret->mkItem("vehicles emitted [#]", true,
                new FunctionBinding<MSVehicleControl, unsigned int>(&(getNet().getVehicleControl()), &MSVehicleControl::getEmittedVehicleNo));
    ret->mkItem("vehicles loaded [#]", true,
                new FunctionBinding<MSVehicleControl, unsigned int>(&(getNet().getVehicleControl()), &MSVehicleControl::getLoadedVehicleNo));
    ret->mkItem("vehicles waiting [#]", true,
                new FunctionBinding<GUINet, unsigned int>(&(getNet()), &GUINet::getWaitingVehicleNo));
    ret->mkItem("end time [s]", false,
                OptionsCont::getOptions().getInt("end"));
    ret->mkItem("begin time [s]", false,
                OptionsCont::getOptions().getInt("begin"));
    ret->mkItem("time step [s]", true,
                new FunctionBinding<GUINet, SUMOTime>(&(getNet()), &GUINet::getCurrentTimeStep));
    if (getNet().logSimulationDuration()) {
        ret->mkItem("step duration [ms]", true,
                    new FunctionBinding<GUINet, int>(&(getNet()), &GUINet::getWholeDuration));
        ret->mkItem("simulation duration [ms]", true,
                    new FunctionBinding<GUINet, int>(&(getNet()), &GUINet::getSimDuration));
        /*
        ret->mkItem("visualisation duration [ms]", true,
            new CastingFunctionBinding<GUINet, SUMOReal, int>(
                &(getNet()), &GUINet::getVisDuration));
        */
        ret->mkItem("idle duration [ms]", true,
                    new FunctionBinding<GUINet, int>(&(getNet()), &GUINet::getIdleDuration));
        ret->mkItem("duration factor []", true,
                    new FunctionBinding<GUINet, SUMOReal>(&(getNet()), &GUINet::getRTFactor));
        /*
        ret->mkItem("mean duration factor []", true,
            new FuncBinding_IntParam<GUINet, SUMOReal>(
                &(getNet()), &GUINet::getMeanRTFactor), 1);
                */
        ret->mkItem("ups [#]", true,
                    new FunctionBinding<GUINet, SUMOReal>(&(getNet()), &GUINet::getUPS));
        ret->mkItem("mean ups [#]", true,
                    new FunctionBinding<GUINet, SUMOReal>(&(getNet()), &GUINet::getMeanUPS));
    }
    // close building
    ret->closeBuilding();
    return ret;
}



GUIGlObjectType
GUINetWrapper::getType() const throw()
{
    return GLO_NETWORK;
}


const std::string &
GUINetWrapper::microsimID() const throw()
{
    return StringUtils::emptyString;
}


Boundary
GUINetWrapper::getBoundary() const
{
    return myNet.getBoundary();
}


GUINet &
GUINetWrapper::getNet() const
{
    return myNet;
}

Boundary
GUINetWrapper::getCenteringBoundary() const throw()
{
    return getBoundary();
}



/****************************************************************************/


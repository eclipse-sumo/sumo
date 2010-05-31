/****************************************************************************/
/// @file    GUIMeanData_Net.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 28.05.2010
/// @version $Id: GUIMeanData_Net.cpp 8740 2010-05-07 12:23:10Z behrisch $
///
// Network state mean data collector for edges/lanes (gui version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <utils/gui/div/GUIParameterTableWindow.h>
#include <microsim/logging/FunctionBinding.h>
#include "GUINet.h"
#include "GUIMeanData_Net.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIMeanData_Net::GUIMeanData_Net(const std::string &id,
                                 const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                                 const bool useLanes, const bool withEmpty,
                                 const bool trackVehicles,
                                 const SUMOReal maxTravelTime, const SUMOReal minSamples,
                                 const SUMOReal haltSpeed, const std::set<std::string> vTypes) throw()
        : MSMeanData_Net(id, dumpBegin, dumpEnd, useLanes, withEmpty, trackVehicles,
                         maxTravelTime, minSamples, haltSpeed, vTypes) {
}


GUIMeanData_Net::~GUIMeanData_Net() throw() {}


GUIParameterTableWindow *
GUIMeanData_Net::getParameterWindow(GUIMainWindow &app,
                                    GUISUMOAbstractView &) throw() {
    GUIParameterTableWindow *ret =
        new GUIParameterTableWindow(app, *(GUINet*)MSNet::getInstance(), 13);
    // add items
    ret->mkItem("vehicles entered [#]", true,
                new FunctionBinding<GUIMeanData_Net, unsigned int>(this, &GUIMeanData_Net::getTotalEntered));
    ret->closeBuilding();
    return ret;
}


unsigned int
GUIMeanData_Net::getTotalEntered() const throw() {
    return 0;
}

/****************************************************************************/

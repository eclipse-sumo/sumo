/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    VClassIcons.cpp
/// @author  Pablo Alvarez Lopez
/// @author  Angelo Banse
/// @date    Jan 2023
///
// Function that returns the icon for a certain vClass
/****************************************************************************/
#include <config.h>

#include <utils/common/SUMOVehicleClass.h>
#include <utils/foxtools/fxheader.h>
#include <utils/gui/images/GUIIconSubSys.h>

#include "VClassIcons.h"


// ===========================================================================
// member method definitions
// ===========================================================================
FXIcon*
VClassIcons::getVClassIcon(const SUMOVehicleClass vc) {
    switch (vc) {
        case SVC_IGNORING:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_IGNORING);
        case SVC_PRIVATE:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_PRIVATE);
        case SVC_EMERGENCY:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_EMERGENCY);
        case SVC_AUTHORITY:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_AUTHORITY);
        case SVC_ARMY:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_ARMY);
        case SVC_VIP:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_VIP);
        case SVC_PEDESTRIAN:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_PEDESTRIAN);
        case SVC_PASSENGER:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_PASSENGER);
        case SVC_HOV:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_HOV);
        case SVC_TAXI:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_TAXI);
        case SVC_BUS:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_BUS);
        case SVC_COACH:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_COACH);
        case SVC_DELIVERY:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_DELIVERY);
        case SVC_TRUCK:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_TRUCK);
        case SVC_TRAILER:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_TRAILER);
        case SVC_MOTORCYCLE:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_MOTORCYCLE);
        case SVC_MOPED:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_MOPED);
        case SVC_BICYCLE:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_BICYCLE);
        case SVC_E_VEHICLE:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_EVEHICLE);
        case SVC_TRAM:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_TRAM);
        case SVC_RAIL_URBAN:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_RAIL_URBAN);
        case SVC_RAIL:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_RAIL);
        case SVC_RAIL_ELECTRIC:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_RAIL_ELECTRIC);
        case SVC_RAIL_FAST:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_RAIL_FAST);
        case SVC_SHIP:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_SHIP);
        case SVC_CUSTOM1:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_CUSTOM1);
        case SVC_CUSTOM2:
            return GUIIconSubSys::getIcon(GUIIcon::VCLASS_SMALL_CUSTOM2);
        default:
            throw ProcessError(TL("Invalid vClass"));
    }
}


/****************************************************************************/

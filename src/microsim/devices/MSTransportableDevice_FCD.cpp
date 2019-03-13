/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSTransportableDevice_FCD.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    11.06.2013
/// @version $Id$
///
// A device which stands as an implementation FCD and which outputs movereminder calls
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSTransportable.h>
#include "MSTransportableDevice_FCD.h"

// ===========================================================================
// static members
// ===========================================================================

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSTransportableDevice_FCD::insertOptions(OptionsCont& oc) {
    insertDefaultAssignmentOptions("fcd", "FCD Device", oc, true);

    oc.doRegister("person-device.fcd.period", new Option_String("0"));
    oc.addDescription("person-device.fcd.period", "FCD Device", "Recording period for FCD-data");
}


void
MSTransportableDevice_FCD::buildDevices(MSTransportable& t, std::vector<MSTransportableDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "fcd", t, oc.isSet("fcd-output"), true)) {
        MSTransportableDevice_FCD* device = new MSTransportableDevice_FCD(t, "fcd_" + t.getID());
        into.push_back(device);
    }
}


// ---------------------------------------------------------------------------
// MSTransportableDevice_FCD-methods
// ---------------------------------------------------------------------------
MSTransportableDevice_FCD::MSTransportableDevice_FCD(MSTransportable& holder, const std::string& id) :
    MSTransportableDevice(holder, id) {
}


MSTransportableDevice_FCD::~MSTransportableDevice_FCD() {
}


void
MSTransportableDevice_FCD::cleanup() {
}

/****************************************************************************/


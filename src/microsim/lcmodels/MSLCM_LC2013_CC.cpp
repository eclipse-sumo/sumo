/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSLCM_LC2013_CC.cpp
/// @author  Michele Segata
/// @date    Do, 02.05.2024
///
// A lane change model developed modification for the CC carFollowModel
/****************************************************************************/
#include "MSLCM_LC2013_CC.h"
#include <microsim/cfmodels/MSCFModel_CC.h>

// ===========================================================================
// member method definitions
// ===========================================================================
MSLCM_LC2013_CC::MSLCM_LC2013_CC(MSVehicle& v) : MSLCM_LC2013(v) {}

MSLCM_LC2013_CC::~MSLCM_LC2013_CC() {}

int MSLCM_LC2013_CC::checkChangeBeforeCommitting(const MSVehicle *veh, int state) const {

    if (state & LCA_WANTS_LANECHANGE) {
        auto *cfm = dynamic_cast<const MSCFModel_CC *>(&veh->getCarFollowModel());

        if (cfm) {
            bool left = (state & LCA_LEFT) != 0;
            return cfm->commitToLaneChange(veh, left);
        }
    }
    return 0;
}

/****************************************************************************/

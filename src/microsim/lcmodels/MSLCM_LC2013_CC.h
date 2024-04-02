/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
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
/// @file    MSLCM_LC2013.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Fri, 08.10.2013
///
// A lane change model developed by D. Krajzewicz, J. Erdmann et al. between 2004 and 2013
/****************************************************************************/
#pragma once

#include "MSLCM_LC2013.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLCM_LC2013_CC
 * @brief A lane change model developed by D. Krajzewicz, J. Erdmann
 * et al. between 2004 and 2013, extended for atomic lane change for platoons
 */
class MSLCM_LC2013_CC : public MSLCM_LC2013 {
public:

    MSLCM_LC2013_CC(MSVehicle& v);

    virtual ~MSLCM_LC2013_CC();

    /// @brief Returns the model's id
    LaneChangeModel getModelID() const override {
        return LaneChangeModel::LC2013_CC;
    }

    int checkChangeBeforeCommitting(const MSVehicle* veh, int state) const override;

};

/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEWalkingPathCalculator.h
/// @author  Josh
/// @date    2026-11-26
///
// Computes oriented walking geometry using an editable network snapshot.
/****************************************************************************/
#pragma once
#include <config.h>

#include <memory>
#include <string>
#include <vector>
#include <utils/geom/PositionVector.h>

class NBNetBuilder;
class OptionsCont;

/// @brief Computes oriented walking geometry using an editable network snapshot.
class GNEWalkingPathCalculator {
public:
    struct Segment {
        std::string laneID;
        PositionVector shape;
    };

    GNEWalkingPathCalculator(NBNetBuilder& builder, const OptionsCont& options);
    ~GNEWalkingPathCalculator();

    bool isAvailable() const;

    std::vector<Segment> compute(const std::string& fromID, const std::string& toID,
                                 double departPos, double arrivalPos) const;

private:
    class Network;
    std::unique_ptr<Network> myNetwork;
};

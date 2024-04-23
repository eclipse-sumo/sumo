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
/// @file    MSRouteTest.cpp
/// @author  Michael Behrisch
/// @date    2024-04-23
///
// Tests selected MSRoute functions
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <gtest/gtest.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSRoute.h>


class MSRouteTest : public testing::Test {
protected :
    MSEdge* edge;
    ConstMSRoutePtr route;

    virtual void SetUp() {
        edge = new MSEdge("dummy", 0, SumoXMLEdgeFunc::NORMAL, "", "", -1, 0);
        MSLane* dummyLane = new MSLane("dummy_0", 50 / 3.6, 1., 100, edge, 0, PositionVector(), SUMO_const_laneWidth, SVCAll, SVCAll, SVCAll, 0, false, "", PositionVector());
        std::vector<MSLane*> lanes;
        lanes.push_back(dummyLane);
        edge->initialize(&lanes);
        edge->closeBuilding();
        ConstMSEdgeVector edges{edge};
        route = std::make_shared<MSRoute>("dummyRoute", edges, true, nullptr, std::vector<SUMOVehicleParameter::Stop>());
    }
};


TEST_F(MSRouteTest, test_method_getDistanceBetween) {
    EXPECT_DOUBLE_EQ(1, route->getDistanceBetween(0, 1, edge, edge));
}

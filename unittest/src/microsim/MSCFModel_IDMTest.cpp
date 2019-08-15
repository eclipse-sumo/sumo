/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSCFModel_IDMTest.cpp
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    2013-06-05
/// @version $Id$
///
// Tests the cfmodel functions
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <gtest/gtest.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSFrame.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include <microsim/MSRoute.h>
#include <microsim/cfmodels/MSCFModel_IDM.h>


class MSVehicleMock : public MSVehicle {
public:
    MSVehicleMock(SUMOVehicleParameter* pars, const MSRoute* route,
                  MSVehicleType* type, const double speedFactor):
        MSVehicle(pars, route, type, speedFactor) {}

};


class MSCFModel_IDMTest : public testing::Test {
protected :
    MSVehicleType* type;
    SUMOVehicleParameter* defs;
    MSVehicle* veh;
    MSRoute* route;
    MSEdge* edge;
    MSLane* lane;
    double accel;
    double decel;
    double dawdle;
    double tau; // headway time

    virtual void SetUp() {
        if (!OptionsCont::getOptions().exists("step-length")) {
            MSFrame::fillOptions();
        }
        MSLane::initRNGs(OptionsCont::getOptions());
        tau = 1;
        MSGlobals::gUnitTests = true;
        defs = new SUMOVehicleParameter();
        defs->departLaneProcedure = DEPART_LANE_GIVEN;
        SUMOVTypeParameter typeDefs("t0");
        typeDefs.cfModel = SUMO_TAG_CF_IDM;
        //typeDefs.cfParameter[SUMO_ATTR_CF_IDM_STEPPING] = "1";
        ConstMSEdgeVector edges;
        MSEdge* edge = new MSEdge("dummy", 0, EDGEFUNC_NORMAL, "", "", -1, 0);
        MSLane* lane = new MSLane("dummy_0", 50 / 3.6, 100, edge, 0, PositionVector(), SUMO_const_laneWidth, SVCAll, 0, false, "");
        std::vector<MSLane*> lanes;
        lanes.push_back(lane);
        edge->initialize(&lanes);
        edges.push_back(edge);
        route = new MSRoute("dummyRoute", edges, true, 0, defs->stops);
        type = MSVehicleType::build(typeDefs);
        veh = new MSVehicleMock(defs, route, type, 1);
        veh->setTentativeLaneAndPosition(lane, 0);
        MSGlobals::gSemiImplicitEulerUpdate = true;
    }

    virtual void TearDown() {
        delete veh;
        delete type;
        delete route;
    }
};

/* Test the method 'brakeGap'.*/

TEST_F(MSCFModel_IDMTest, test_method_brakeGap) {
    // discrete braking model. keep driving for 1 s
    MSCFModel& m = type->getCarFollowModel();
    const double v = 3;
    EXPECT_DOUBLE_EQ(tau * v, m.brakeGap(v));
}

TEST_F(MSCFModel_IDMTest, test_method_getSecureGap) {
    // the value of getSecureGap should be consistent with followSpeed so that
    // strong braking is avoided after lane changing (#4517)
    MSCFModel& m = type->getCarFollowModel();
    for (double v = 0; v < 15; v += 1) { // follower
        for (double u = 0; u < 25; u += 1) { // leader
            double sg = m.getSecureGap(v, u, m.getMaxDecel());
            double vFollow = m.followSpeed(veh, v, sg, u, m.getMaxDecel(), nullptr);
            double accel = SPEED2ACCEL(vFollow - v);
            //std::cout << v << " " << u << " " << sg << " " << vFollow << " " << accel << "\n";
            EXPECT_GT(accel, -2.2);
        }
    }
}




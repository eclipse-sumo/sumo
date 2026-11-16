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
/// @file    LogitCalculatorTest.cpp
/// @author  Mark Colley
/// @date    2026-11-05
///
/****************************************************************************/
#include <config.h>
#include <gtest/gtest.h>
#include <utils/options/Option.h>
#include <utils/router/RouteCostCalculator.h>

namespace {
struct TestVehicle {};

struct TestEdge {
    double getTravelTime(const TestVehicle*, double) const {
        return 10.;
    }
};

struct TestRoute {
    std::vector<const TestEdge*> edges;
    double cost = 3600.;
    double probability = 0.;

    const std::vector<const TestEdge*>& getEdgeVector() const {
        return edges;
    }
    double getCosts() const {
        return cost;
    }
    void setCosts(double value) {
        cost = value;
    }
    void setProbability(double value) {
        probability = value;
    }
};
}

class LogitCalculatorTest : public testing::Test {
protected:
    void SetUp() override {
        OptionsCont& options = OptionsCont::getOptions();
        options.clear();
        options.doRegister("max-alternatives", new Option_Integer(5));
        options.doRegister("keep-all-routes", new Option_Bool(false));
        options.doRegister("skip-new-routes", new Option_Bool(false));
        for (int i = 0; i < 3; ++i) {
            auto route = std::make_shared<TestRoute>();
            route->edges.push_back(i < 2 ? &first : &second);
            routes.push_back(route);
        }
    }

    void TearDown() override {
        OptionsCont::getOptions().clear();
    }

    TestEdge first;
    TestEdge second;
    std::vector<std::shared_ptr<TestRoute>> routes;
};

TEST_F(LogitCalculatorTest, ZeroBetaDoesNotReusePriorCommonality) {
    LogitCalculator<TestRoute, TestEdge, TestVehicle> calculator(-1., 1., 1.);
    calculator.calculateProbabilities(routes, nullptr, 0);
    EXPECT_NEAR(.25, routes[0]->probability, 1e-12);
    EXPECT_NEAR(.25, routes[1]->probability, 1e-12);
    EXPECT_NEAR(.5, routes[2]->probability, 1e-12);
    for (const auto& route : routes) {
        route->cost = 0.;
    }
    calculator.calculateProbabilities(routes, nullptr, 0);
    for (const auto& route : routes) {
        EXPECT_NEAR(1. / 3., route->probability, 1e-12);
    }
}

TEST_F(LogitCalculatorTest, RepeatedQueriesMatchFreshCalculators) {
    for (double beta : {-1., 0., 1.}) {
        LogitCalculator<TestRoute, TestEdge, TestVehicle> calculator(beta, 1., .01);
        for (double cost : {3600., 1800., 0., 7200.}) {
            for (const auto& route : routes) {
                route->cost = cost;
            }
            calculator.calculateProbabilities(routes, nullptr, 100000);
            std::vector<double> probabilities;
            for (const auto& route : routes) {
                probabilities.push_back(route->probability);
            }
            LogitCalculator<TestRoute, TestEdge, TestVehicle> fresh(beta, 1., .01);
            fresh.calculateProbabilities(routes, nullptr, 100000);
            double total = 0.;
            for (size_t i = 0; i < routes.size(); ++i) {
                EXPECT_DOUBLE_EQ(probabilities[i], routes[i]->probability);
                total += routes[i]->probability;
            }
            EXPECT_NEAR(1., total, 1e-12);
        }
    }
}

/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2023 German Aerospace Center (DLR) and others.
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
/// @file    AFROtest.h
/// @author  Ruediger Ebendt
/// @date    01.06.2024
///
// Declaration of class for the arc flag router test suite
/****************************************************************************/
#pragma once
#include <config.h>

#include <router/RONet.h>
#include <router/ROEdge.h>
#define AFRO_DEBUG_LEVEL_0
#include <utils/router/AFRouter.h>
#include <utils/router/Node2EdgeRouter.h>
#include <utils/router/CHRouter.h>
#include <utils/router/AStarRouter.h>

typedef typename KDTreePartition<ROEdge, RONode, ROVehicle>::Cell Cell;
typedef typename AFInfo<ROEdge>::FlagInfo FlagInfo;
typedef AbstractLookupTable<ROEdge, ROVehicle> LookupTable;
typedef AbstractLookupTable<FlippedEdge<ROEdge, RONode, ROVehicle>, ROVehicle> FlippedLookupTable;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class AFROTest
 * Test suite for the arc flag router
 */
class AFROTest {
public:
    /// @brief Constructor
    AFROTest();
    /// @brief Destructor
    ~AFROTest();

    /** @brief Invokes the test suite
     * @param[in] vehicle The vehicle
     * @param[in] unbuildIsWarning The flag indicating whether network unbuilds should issue warnings or errors
     * @param[in] operation The operation for a forward graph
     * @param[in] flippedOperation The operation for a backward graph with flipped edges
     * @param[in] weightPeriod The validity duration of one weight interval
     * @param[in] lookup The lookup table for a forward graph
     * @param[in] flippedLookup The lookup table for a backward graph  with flipped edges
     * @param[in] havePermissions The boolean flag indicating whether edge permissions need to be considered or not
     * @param[in] haveRestrictions The boolean flag indicating whether edge restrictions need to be considered or not
     */
    void test(const ROVehicle* const vehicle, bool unbuildIsWarning, typename SUMOAbstractRouter<ROEdge, ROVehicle>::Operation operation,
              typename SUMOAbstractRouter<FlippedEdge<ROEdge, RONode, ROVehicle>, ROVehicle>::Operation flippedOperation,
              SUMOTime weightPeriod,
              const std::shared_ptr<const LookupTable> lookup = nullptr,
              const std::shared_ptr<const FlippedLookupTable> flippedLookup = nullptr,
              const bool havePermissions = false, const bool haveRestrictions = false);

private:
    /** @brief Tests routes from edges inside the first cell to such of the second cell
     * @param[in] cell1 The first cell
     * @param[in] cell2 The second cell
     * @param[in] vehicle The vehicle
     * @param[in] arcFlagRouter The arc flag router
     * @param[in] cHRouter The contraction hierarchies router
     * @param[in] aStar The A* (A Star) router
     */
    void testRoutes(const Cell* cell1, const Cell* cell2, const ROVehicle* const vehicle,
                    AFRouter<ROEdge, RONode, ROVehicle>* arcFlagRouter,
                    CHRouter<ROEdge, ROVehicle>* cHRouter,
                    AStarRouter<ROEdge, ROVehicle>* aStar);

    /** @brief Tests a single route from the first edge to the second one
     * @param[in] edge1 The first edge
     * @param[in] edge2 The second edge
     * @param[in] vehicle The vehicle
     * @param[in] arcFlagRouter The arc flag router
     * @param[in] aStar The A* (A Star) router
     */
    void testQuery(const ROEdge* edge1, const ROEdge* edge2, const ROVehicle* const vehicle,
                   AFRouter<ROEdge, RONode, ROVehicle>* arcFlagRouter,
                   AStarRouter<ROEdge, ROVehicle>* aStar);
};

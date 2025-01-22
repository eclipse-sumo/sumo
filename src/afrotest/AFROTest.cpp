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
/// @file    AFROTest.cpp
/// @author  Ruediger Ebendt
/// @date    01.06.2024
///
// Class for the arc flag router test
/****************************************************************************/
#include <config.h>
#include "AFROTest.h"

/// @brief The number of levels in the k-d tree partition
constexpr auto NUMBER_OF_LEVELS = 5; //or 4 or 8
/// @brief The number of start edges of the route queries
constexpr auto NUMBER_OF_START_EDGES = 32; // 32 -> 32^2 = 1024 test routes
/// @brief The number of end edges of the route queries
constexpr auto NUMBER_OF_END_EDGES = NUMBER_OF_START_EDGES;

//#define ATST_DEBUG_LEVEL_0

// ===========================================================================
// member method definitions
// ===========================================================================

AFROTest::AFROTest() {};
AFROTest::~AFROTest() {};

void
AFROTest::test(const ROVehicle* const vehicle, bool unbuildIsWarning, typename SUMOAbstractRouter<ROEdge, ROVehicle>::Operation operation,
               typename SUMOAbstractRouter<FlippedEdge<ROEdge, RONode, ROVehicle>, ROVehicle>::Operation flippedOperation,
               SUMOTime weightPeriod,
               const std::shared_ptr<const LookupTable> lookup,
               const std::shared_ptr<const FlippedLookupTable> flippedLookup,
               const bool havePermissions, const bool haveRestrictions) {
    std::cout << "Creating forward k-d tree partition..." << std::endl;
    int actualNumberOfLevels = -1;
    const ROEdgeVector& edges = ROEdge::getAllEdges();
    long long int kDPartitionBuildStart = 0;
    long long int kDPartitionBuildTime = 0;
    kDPartitionBuildStart = SysUtils::getCurrentMillis();
    KDTreePartition<ROEdge, RONode, ROVehicle>* partition = new KDTreePartition<ROEdge, RONode, ROVehicle>(NUMBER_OF_LEVELS, edges, havePermissions, haveRestrictions);
    partition->init(vehicle);
    kDPartitionBuildTime = (SysUtils::getCurrentMillis() - kDPartitionBuildStart);
    std::cout << "Time spent for k-d partition build: " << elapsedMs2string(kDPartitionBuildTime) << std::endl;
    std::cout << "Forward k-d tree partition ready." << std::endl;
    AFRouter<ROEdge, RONode, ROVehicle>* arcFlagRouter = new AFRouter<ROEdge, RONode, ROVehicle>(edges,
            partition, unbuildIsWarning, operation, flippedOperation, weightPeriod, lookup, flippedLookup, havePermissions,
            haveRestrictions);
    CHRouter<ROEdge, ROVehicle>* cHRouter = new CHRouter<ROEdge, ROVehicle>(
        edges, unbuildIsWarning, operation, /*SVC_IGNORING*/vehicle->getVClass(), weightPeriod, havePermissions, haveRestrictions);
    AStarRouter<ROEdge, ROVehicle>* aStar = new AStarRouter<ROEdge, ROVehicle>(edges, unbuildIsWarning, operation, lookup,
            havePermissions, haveRestrictions);
    actualNumberOfLevels = partition->getNumberOfLevels();
    std::cout << "Actual number of levels after partitioning: " << actualNumberOfLevels << std::endl;
    if (actualNumberOfLevels == 3) {
        try {
            const Cell* cell2 = partition->cell(2);

            std::cout << "Test suite started: " << std::endl;
            testRoutes(cell2, cell2, vehicle, arcFlagRouter, cHRouter, aStar);
            std::cout << "Stats:" << std::endl;
            arcFlagRouter->reportStatistics();
            arcFlagRouter->resetStatistics();
            std::cout << "Tests are done." << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            exit(-1);
        }
    } else if (actualNumberOfLevels == 4) {
        try {
            const Cell* cell3 = partition->cell(3);
            const Cell* cell14 = partition->cell(14);

            std::cout << "Test suite started: " << std::endl;
            testRoutes(cell3, cell14, vehicle, arcFlagRouter, cHRouter, aStar);
            std::cout << "Stats:" << std::endl;
            arcFlagRouter->reportStatistics();
            arcFlagRouter->resetStatistics();
            std::cout << "Tests are done." << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            exit(-1);
        }
    } else if (actualNumberOfLevels == 5) {
        try {
            const Cell* cell4 = partition->cell(4);
            //const Cell* cell5 = partition->cell(5);
            //const Cell* cell7 = partition->cell(7);
            //const Cell* cell8 = partition->cell(8);
            //const Cell* cell11 = partition->cell(11);
            //const Cell* cell23 = partition->cell(23);
            const Cell* cell30 = partition->cell(30);

            std::cout << "Test suite started: " << std::endl;
            //testRoutes(cell4, cell5, vehicle, arcFlagRouter, cHRouter, aStar);
            //std::cout << "Stats:" << std::endl;
            //arcFlagRouter->reportStatistics();
            //arcFlagRouter->resetStatistics();
            //testRoutes(cell4, cell7, vehicle, arcFlagRouter, cHRouter, aStar);
            //std::cout << "Stats:" << std::endl;
            //arcFlagRouter->reportStatistics();
            //arcFlagRouter->resetStatistics();
            //testRoutes(cell4, cell8, vehicle, arcFlagRouter, cHRouter, aStar);
            //std::cout << "Stats:" << std::endl;
            //arcFlagRouter->reportStatistics();
            //arcFlagRouter->resetStatistics();
            //testRoutes(cell11, cell23, vehicle, arcFlagRouter, cHRouter, aStar);
            //std::cout << "Stats:" << std::endl;
            //arcFlagRouter->reportStatistics();
            //arcFlagRouter->resetStatistics();
            testRoutes(cell4, cell30, vehicle, arcFlagRouter, cHRouter, aStar);
            std::cout << "Stats:" << std::endl;
            arcFlagRouter->reportStatistics();
            arcFlagRouter->resetStatistics();
            std::cout << "Tests are done." << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            exit(-1);
        }
    } else if (actualNumberOfLevels == 8) {
        try {
            const Cell* cell7 = partition->cell(7);
            const Cell* cell254 = partition->cell(254);
            //const Cell* cell8 = partition->cell(8);
            //const Cell* cell253 = partition->cell(253);
            //const Cell* cell10 = partition->cell(10);
            //const Cell* cell251 = partition->cell(251);
            //const Cell* cell11 = partition->cell(11);
            //const Cell* cell250 = partition->cell(250);
            //const Cell* cell14 = partition->cell(14);
            //const Cell* cell247 = partition->cell(247);

            std::cout << "Test suite started: " << std::endl;
            //testRoutes(cell14, cell247, vehicle, arcFlagRouter, cHRouter, aStar);
            //std::cout << "Stats:" << std::endl;
            //arcFlagRouter->reportStatistics();
            //arcFlagRouter->resetStatistics();
            //testRoutes(cell11, cell250, vehicle, arcFlagRouter, cHRouter, aStar);
            //std::cout << "Stats:" << std::endl;
            //arcFlagRouter->reportStatistics();
            //arcFlagRouter->resetStatistics();
            //testRoutes(cell10, cell251, vehicle, arcFlagRouter, cHRouter, aStar);
            //std::cout << "Stats:" << std::endl;
            //arcFlagRouter->reportStatistics();
            //arcFlagRouter->resetStatistics();
            //testRoutes(cell8, cell253, vehicle, arcFlagRouter, cHRouter, aStar);
            //std::cout << "Stats:" << std::endl;
            //arcFlagRouter->reportStatistics();
            //arcFlagRouter->resetStatistics();
            testRoutes(cell7, cell254, vehicle, arcFlagRouter, cHRouter, aStar);
            std::cout << "Stats:" << std::endl;
            arcFlagRouter->reportStatistics();
            arcFlagRouter->resetStatistics();
            std::cout << "Tests are done." << std::endl;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            exit(-1);
        }
    } // end if
    delete arcFlagRouter;
    delete cHRouter;
    delete aStar;
    delete partition;
} // end of test method

void
AFROTest::testRoutes(const Cell* cell1, const Cell* cell2, const ROVehicle* const vehicle,
                     AFRouter<ROEdge, RONode, ROVehicle>* arcFlagRouter,
                     CHRouter<ROEdge, ROVehicle>* cHRouter,
                     AStarRouter<ROEdge, ROVehicle>* aStar) {
    std::unordered_set<const ROEdge*>* cell1InsideEdges = cell1->edgeSet(vehicle);
    for (auto iter = cell1InsideEdges->begin(); iter != cell1InsideEdges->end();) {
        if ((arcFlagRouter->edgeInfo(*iter))->prohibited
                || arcFlagRouter->isProhibited(*iter, vehicle)) {
            iter = cell1InsideEdges->erase(iter);
        } else {
            ++iter;
        }
    }
    std::unordered_set<const ROEdge*> someCell1InsideEdges;
    auto iter = cell1InsideEdges->begin();
    std::advance(iter, MIN2(cell1InsideEdges->size(), static_cast<size_t>(NUMBER_OF_START_EDGES)));
    someCell1InsideEdges.insert(cell1InsideEdges->begin(), iter);
    delete cell1InsideEdges;
    std::unordered_set<const ROEdge*>* cell2InsideEdges = cell2->edgeSet(vehicle);
    for (auto iter2 = cell2InsideEdges->begin(); iter2 != cell2InsideEdges->end();) {
        if ((arcFlagRouter->edgeInfo(*iter2))->prohibited
                || arcFlagRouter->isProhibited(*iter2, vehicle)) {
            iter2 = cell2InsideEdges->erase(iter2);
        } else {
            ++iter2;
        }
    }
    std::unordered_set<const ROEdge*> someCell2InsideEdges;
    auto iter2 = cell2InsideEdges->begin();
    std::advance(iter2, MIN2(cell2InsideEdges->size(), static_cast<size_t>(NUMBER_OF_END_EDGES)));
    someCell2InsideEdges.insert(cell2InsideEdges->begin(), iter2);
    delete cell2InsideEdges;

    SUMOTime msTime = 4611686018427387903; // dummy time
    std::vector<const ROEdge*> into, into2;
    std::cout << "Number of cell1 inside edges: " << someCell1InsideEdges.size() << std::endl;
    std::cout << "Number of cell2 inside edges: " << someCell2InsideEdges.size() << std::endl;
    std::cout << "Cell1 number: " << cell1->getNumber() << std::endl;
    std::cout << "Cell1 level: " << cell1->getLevel() << std::endl;
    std::cout << "Cell1 index: " << (cell1->isLeftOrLowerCell() ? 0 : 1) << std::endl;
    std::cout << "Supercell number: " << cell1->getSupercell()->getNumber() << std::endl;
    std::cout << "Supercell level: " << cell1->getSupercell()->getLevel() << std::endl;
    std::cout << "Supercell index: " << (cell1->getSupercell()->isLeftOrLowerCell() ? 0 : 1) << std::endl;
    std::cout << "Cell2 number: " << cell2->getNumber() << std::endl;
    std::cout << "Cell2 level: " << cell2->getLevel() << std::endl;
    std::cout << "Cell2 index : " << (cell2->isLeftOrLowerCell() ? 0 : 1) << std::endl;
    assert(cell1->getLevel() == cell2->getLevel());
    int routeCnt = 0;
    int errorCnt = 0;
    int failCnt = 0;
    double errorSum = 0.;
    double aStarEffortSum = 0.;
    bool firstCallOfCHRouter = true;
    double recomputedEffortAStar = -1.;
    bool alreadyTriedAStar = false;
    for (const ROEdge* edge1 : someCell2InsideEdges) {
        for (const ROEdge* edge2 : someCell1InsideEdges) {
            if (edge1 == edge2) {
                continue;
            }
            std::cout << "routeCnt: " << routeCnt++ << ", edge1: " << edge1->getID() << ", edge2 : " << edge2->getID() << std::endl;
            into.clear();
            into2.clear();
            recomputedEffortAStar = -1.;
            alreadyTriedAStar = false;
            // measure time for first call, though
            long long int firstCallStart = 0;
            long long int firstCallTime = 0;
            if (firstCallOfCHRouter) {
                firstCallStart = SysUtils::getCurrentMillis();
                std::cout << "Calling CHRouter for the first time (hierarchy build): " << std::endl;
            }
            if (cHRouter->compute(edge1, edge2, vehicle, msTime, into)) {
                std::cout << "CH router succeeded." << std::endl;
                if (!into.empty() && aStar->compute(edge1, edge2, vehicle, msTime, into2)) {
                    double recomputedEffortCHRouter = cHRouter->recomputeCosts(into, vehicle, msTime);
                    recomputedEffortAStar = aStar->recomputeCosts(into2, vehicle, msTime);
                    std::cout << "Recomputed CH router effort: " << recomputedEffortCHRouter << ", recomputed A* effort: "
                              << recomputedEffortAStar << std::endl;
                    if (recomputedEffortCHRouter > recomputedEffortAStar) {
                        std::cout << "CH FAIL: Recomputed CH router effort greater than recomputed A* effort." << std::endl;
                        failCnt++;
                        aStarEffortSum += recomputedEffortAStar;
                        errorSum += (recomputedEffortCHRouter - recomputedEffortAStar);
                    }
                } else {
                    std::cout << "Either into is empty or A* did not succeed." << std::endl;
                }
            } else {
                std::cout << "UNREACHABLE for CHRouter!" << std::endl;
                if (aStar->compute(edge1, edge2, vehicle, msTime, into2)) {
                    recomputedEffortAStar = aStar->recomputeCosts(into2, vehicle, msTime);
                    std::cout << "But A* does it, A* effort: " << recomputedEffortAStar << std::endl;
                    failCnt++;
                }
                alreadyTriedAStar = true;
            }
            into.clear();
            into2.clear();
            if (firstCallOfCHRouter) {
                firstCallTime = (SysUtils::getCurrentMillis() - firstCallStart);
                std::cout << "Time spent for hierarchy build: " << elapsedMs2string(firstCallTime) << std::endl;
                firstCallOfCHRouter = false;
            }
            if (arcFlagRouter->compute(edge1, edge2, vehicle, msTime, into)) {
                std::cout << "arc flag router succeeded." << std::endl;
                if (arcFlagRouter->flags(edge1).empty()) {
                    std::cout << "arc flags of edge 1 empty!" << std::endl;
                }
#ifdef ATST_DEBUG_LEVEL_0
                else {
                    std::cout << "flags of edge 1: " << std::endl;
                    for (bool flag : arcFlagRouter->flags(edge1)) {
                        std::cout << flag << ' ';
                    }
                    std::cout << std::endl;
                }
#endif
                if (recomputedEffortAStar != -1. ||
                        (!into.empty() && aStar->compute(edge1, edge2, vehicle, msTime, into2))) {
                    double recomputedEffortArcFlagRouter = arcFlagRouter->recomputeCosts(into, vehicle, msTime);
                    if (recomputedEffortAStar == -1.) {
                        recomputedEffortAStar = aStar->recomputeCosts(into2, vehicle, msTime);
                    }
                    std::cout << "Recomputed arc flag router effort : " << recomputedEffortArcFlagRouter << ", recomputed A* effort: "
                              << recomputedEffortAStar << std::endl;
                    if (recomputedEffortArcFlagRouter > recomputedEffortAStar) {
                        std::cout << "ERROR: Recomputed arc flag router effort greater than recomputed A* effort." << std::endl;
                        errorCnt++;
                    }
                } else {
                    std::cout << "Either into is empty or A* did not succeed." << std::endl;
                }
            } else {
                std::cout << "UNREACHABLE for arc flag router!" << std::endl;
                if (recomputedEffortAStar == -1. && !alreadyTriedAStar
                        && aStar->compute(edge1, edge2, vehicle, msTime, into2)) {
                    recomputedEffortAStar = aStar->recomputeCosts(into2, vehicle, msTime);
                    std::cout << "But A* does it, A* effort: " << recomputedEffortAStar << std::endl;
                    errorCnt++;
                } else if (recomputedEffortAStar != -1.) {
                    std::cout << "But A* does it, A* effort: " << recomputedEffortAStar << std::endl;
                    errorCnt++;
                }
            }
        } // end of loop over cell boundary edge
    } // end of loop over sibling boundary edge
    std::cout << "Arc flag router: " << errorCnt << " errors in " << routeCnt
              << " routes";
    if (routeCnt == 0) {
        std::cout << "." << std::endl;
    } else {
        std::cout << " (that is " << (static_cast<double>(errorCnt) / static_cast<double>(routeCnt)) * 100.
                  << " percent)." << std::endl;
    }
    std::cout << "CH router: " << failCnt << " errors in " << routeCnt
              << " routes";
    if (routeCnt == 0) {
        std::cout << "." << std::endl;
    } else {
        std::cout << " (that is " << (static_cast<double>(failCnt) / static_cast<double>(routeCnt)) * 100.
                  << " percent)." << std::endl;
    }
    if (failCnt && errorSum && aStarEffortSum) {
        std::cout << "Average deviation of CH Router result from the (smaller) A* result: "
                  << (errorSum / aStarEffortSum) * 100. << " percent." << std::endl;
    }
}

void
AFROTest::testQuery(const ROEdge* edge1, const ROEdge* edge2, const ROVehicle* const vehicle,
                    AFRouter<ROEdge, RONode, ROVehicle>* arcFlagRouter,
                    AStarRouter<ROEdge, ROVehicle>* aStar) {
    assert(edge1 && edge2);
    assert(edge1 != edge2);
    std::cout << "Testcase, edge1: " << edge1->getID() << ", edge2 : " << edge2->getID() << std::endl;
    SUMOTime msTime = 4611686018427387903; // dummy time
    ConstROEdgeVector into, into2;
    into.clear();
    into2.clear();
    if (arcFlagRouter->compute(edge1, edge2, vehicle, msTime, into)) {
        std::cout << "arc flag router succeeded." << std::endl;
        if (arcFlagRouter->flags(edge1).empty()) {
            std::cout << "arc flags of edge 1 empty!" << std::endl;
        } else {
            std::cout << "flags of edge 1: " << std::endl;
            for (bool flag : arcFlagRouter->flags(edge1)) {
                std::cout << flag << ' ';
            }
            std::cout << std::endl;
        }
        //
        if (!into.empty() && aStar->compute(edge1, edge2, vehicle, msTime, into2)) {
            double recomputedEffortArcFlagRouter = arcFlagRouter->recomputeCosts(into, vehicle, msTime);
            double recomputedEffortAStar = aStar->recomputeCosts(into2, vehicle, msTime);
            std::cout << "Recomputed arc flag router effort : " << recomputedEffortArcFlagRouter << ", recomputed A* effort : "
                      << recomputedEffortAStar << std::endl;
            double eps = 0.02;
            if (recomputedEffortArcFlagRouter > recomputedEffortAStar + eps || recomputedEffortArcFlagRouter < recomputedEffortAStar - eps) {
                std::cout << "WARNING: Recomputed arc flag router effort *not* less/greater than or equal to recomputed A* effort -/+ eps (eps = " << eps << ")!" << std::endl;
            }
            if (recomputedEffortArcFlagRouter > recomputedEffortAStar) {
                std::cout << "ERROR: Recomputed arc flag router effort greater than recomputed A* effort." << std::endl;
                size_t maxPathLength = MAX2(into.size(), into2.size());
                if (into.size() != into2.size()) {
                    std::cout << "Path lengths differ! into size: " << into.size() << ", into2 size: " << into2.size() << std::endl;
                }
                for (size_t i = 0; i < maxPathLength; i++) {
                    std::cout << "into[" << i << "]: " << ((i >= into.size())  ? "*" : into[i]->getID())
                              << ", into2[" << i << "]: " << into2[i]->getID()
                              << ((i >= into.size()) ? " - INTO ENDED!" : (into[i] != into2[i] ? " - DIFFERENT!" : ""))
                              << std::endl;
                }
                std::cout << "into:" << std::endl;
                for (size_t i = 0; i < into.size(); i++) {
                    std::cout << "into[" << i << "]: " << into[i]->getID() << std::endl;
                }
                std::cout << "into2:" << std::endl;
                for (size_t i = 0; i < into2.size(); i++) {
                    std::cout << "into2[" << i << "]: " << into2[i]->getID() << std::endl;
                }
            }
        } else {
            std::cout << "Either into is empty or A* did not succeed." << std::endl;
        }
    } else {
        std::cout << "UNREACHABLE!" << std::endl;
        if (aStar->compute(edge1, edge2, vehicle, msTime, into2)) {
            double recomputedEffort = aStar->recomputeCosts(into2, vehicle, msTime);
            std::cout << "But A* does it, A* effort: " << recomputedEffort << std::endl;
        }
    }
}

/****************************************************************************/

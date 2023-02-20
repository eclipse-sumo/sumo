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
/// @file    AStarLookupTable.h
/// @author  Jakob Erdmann
/// @date    July 2017
///
// Precomputed landmark distances to speed up the A* routing algorithm
/****************************************************************************/
#pragma once
#include <config.h>

#include <iostream>
#include <fstream>
#ifdef HAVE_ZLIB
#include <foreign/zstr/zstr.hpp>
#endif
#ifdef HAVE_FOX
#include <utils/foxtools/MFXWorkerThread.h>
#endif
#include <utils/router/ReversedEdge.h>

#define UNREACHABLE (std::numeric_limits<double>::max() / 1000.0)

//#define ASTAR_DEBUG_LOOKUPTABLE
//#define ASTAR_DEBUG_LOOKUPTABLE_FROM "disabled"
//#define ASTAR_DEBUG_UNREACHABLE

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LandmarkLookupTable
 * @brief Computes the shortest path through a network using the A* algorithm.
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge)
 * @param V The vehicle class to use (MSVehicle/ROVehicle)
 * @param PF The prohibition function to use (prohibited_withPermissions/noProhibitions)
 * @param EC The class to retrieve the effort for an edge from
 *
 * The router is edge-based. It must know the number of edges for internal reasons
 *  and whether a missing connection between two given edges (unbuild route) shall
 *  be reported as an error or as a warning.
 *
 */

template<class E, class V>
class AbstractLookupTable {
public:
    virtual ~AbstractLookupTable() {}

    /// @brief provide a lower bound on the distance between from and to (excluding traveltime of both edges)
    virtual double lowerBound(const E* from, const E* to, double speed, double speedFactor, double fromEffort, double toEffort) const = 0;

    /// @brief whether the heuristic ist consistent (found nodes are always visited on the shortest path the first time)
    virtual bool consistent() const = 0;
};


template<class E, class V>
class FullLookupTable : public AbstractLookupTable<E, V> {
public:
    FullLookupTable(const std::string& filename, const int size) :
        myTable(size) {
        std::ifstream strm(filename.c_str());
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                double val;
                strm >> val;
                myTable[i].push_back(val);
            }
        }
    }

    virtual ~FullLookupTable() {
    }

    double lowerBound(const E* from, const E* to, double /*speed*/, double speedFactor, double /*fromEffort*/, double /*toEffort*/) const {
        return myTable[from->getNumericalID()][to->getNumericalID()] / speedFactor;
    }

    bool consistent() const {
        return true;
    }

private:
    std::vector<std::vector<double> > myTable;
};


template<class E, class V>
class LandmarkLookupTable : public AbstractLookupTable<E, V> {
public:
    LandmarkLookupTable(const std::string& filename, const std::vector<E*>& edges, SUMOAbstractRouter<E, V>* router,
                        SUMOAbstractRouter<ReversedEdge<E, V>, V>* reverseRouter,
                        const V* defaultVehicle, const std::string& outfile, const int maxNumThreads) {
        myFirstNonInternal = -1;
        std::map<std::string, int> numericID;
        for (E* e : edges) {
            if (!e->isInternal()) {
                if (myFirstNonInternal == -1) {
                    myFirstNonInternal = e->getNumericalID();
                }
                numericID[e->getID()] = e->getNumericalID() - myFirstNonInternal;
            }
        }
#ifdef HAVE_ZLIB
        zstr::ifstream strm(filename.c_str(), std::fstream::in | std::fstream::binary);
#else
        std::ifstream strm(filename.c_str());
#endif
        if (!strm.good()) {
            throw ProcessError(TLF("Could not load landmark-lookup-table from '%'.", filename));
        }
        std::string line;
        std::vector<const E*> landmarks;
        int numLandMarks = 0;
        bool haveData = false;
        while (std::getline(strm, line)) {
            if (line == "") {
                break;
            }
            StringTokenizer st(line);
            if (st.size() == 1) {
                const std::string lm = st.get(0);
                if (myLandmarks.count(lm) != 0) {
                    throw ProcessError(TLF("Duplicate edge '%' in landmark file.", lm));
                }
                // retrieve landmark edge
                const auto& it = numericID.find(lm);
                if (it == numericID.end()) {
                    throw ProcessError(TLF("Landmark edge '%' does not exist in the network.", lm));
                }
                myLandmarks[lm] = numLandMarks++;
                myFromLandmarkDists.push_back(std::vector<double>(0));
                myToLandmarkDists.push_back(std::vector<double>(0));
                landmarks.push_back(edges[it->second + myFirstNonInternal]);
            } else if (st.size() == 4) {
                // legacy style landmark table
                const std::string lm = st.get(0);
                const std::string edge = st.get(1);
                if (numericID[edge] != (int)myFromLandmarkDists[myLandmarks[lm]].size()) {
                    throw ProcessError(TLF("Unknown or unordered edge '%' in landmark file.", edge));
                }
                const double distFrom = StringUtils::toDouble(st.get(2));
                const double distTo = StringUtils::toDouble(st.get(3));
                myFromLandmarkDists[myLandmarks[lm]].push_back(distFrom);
                myToLandmarkDists[myLandmarks[lm]].push_back(distTo);
                haveData = true;
            } else {
                const std::string edge = st.get(0);
                if ((int)st.size() != 2 * numLandMarks + 1) {
                    throw ProcessError(TLF("Broken landmark file, unexpected number of entries (%) for edge '%'.", st.size() - 1, edge));
                }
                if (numericID[edge] != (int)myFromLandmarkDists[0].size()) {
                    throw ProcessError(TLF("Unknown or unordered edge '%' in landmark file.", edge));
                }
                for (int i = 0; i < numLandMarks; i++) {
                    const double distFrom = StringUtils::toDouble(st.get(2 * i + 1));
                    const double distTo = StringUtils::toDouble(st.get(2 * i + 2));
                    myFromLandmarkDists[i].push_back(distFrom);
                    myToLandmarkDists[i].push_back(distTo);
                }
                haveData = true;
            }
        }
        if (myLandmarks.empty()) {
            WRITE_WARNINGF("No landmarks in '%', falling back to standard A*.", filename);
            return;
        }
#ifdef HAVE_FOX
        MFXWorkerThread::Pool threadPool;
        std::vector<RoutingTask*> currentTasks;
#endif
        if (!haveData) {
            WRITE_MESSAGE(TL("Calculating new lookup table."));
        }
        for (int i = 0; i < numLandMarks; ++i) {
            if ((int)myFromLandmarkDists[i].size() != (int)edges.size() - myFirstNonInternal) {
                const E* const landmark = landmarks[i];
                if (haveData) {
                    if (myFromLandmarkDists[i].empty()) {
                        WRITE_WARNINGF(TL("No lookup table for landmark edge '%', recalculating."), landmark->getID());
                    } else {
                        throw ProcessError(TLF("Not all network edges were found in the lookup table '%' for landmark edge '%'.", filename, landmark->getID()));
                    }
                }
#ifdef HAVE_FOX
                if (maxNumThreads > 0) {
                    const double lmCost = router->recomputeCosts({landmark}, defaultVehicle, 0);
                    router->setAutoBulkMode(true);
                    if (threadPool.size() == 0) {
                        if (reverseRouter == nullptr) {
                            // The CHRouter needs initialization
                            // before it gets cloned, so we do a dummy routing which is not in parallel
                            std::vector<const E*> route;
                            router->compute(landmark, landmark, defaultVehicle, 0, route);
                        } else {
                            reverseRouter->setAutoBulkMode(true);
                        }
                        while ((int)threadPool.size() < maxNumThreads) {
                            auto revClone = reverseRouter == nullptr ? nullptr : reverseRouter->clone();
                            new WorkerThread(threadPool, router->clone(), revClone, defaultVehicle);
                        }
                    }
                    for (int j = (int)myFromLandmarkDists[i].size() + myFirstNonInternal; j < (int)edges.size(); ++j) {
                        const E* const edge = edges[j];
                        if (landmark != edge) {
                            const double sourceDestCost = lmCost + router->recomputeCosts({edge}, defaultVehicle, 0);
                            currentTasks.push_back(new RoutingTask(landmark, edge, sourceDestCost));
                            threadPool.add(currentTasks.back(), i % maxNumThreads);
                        }
                    }
                }
#else
                UNUSED_PARAMETER(reverseRouter);
#endif
            }
        }
#ifdef HAVE_FOX
        threadPool.waitAll(false);
        int taskIndex = 0;
#endif
        for (int i = 0; i < numLandMarks; ++i) {
            if ((int)myFromLandmarkDists[i].size() != (int)edges.size() - myFirstNonInternal) {
                const E* landmark = landmarks[i];
                const double lmCost = router->recomputeCosts({landmark}, defaultVehicle, 0);
                for (int j = (int)myFromLandmarkDists[i].size() + myFirstNonInternal; j < (int)edges.size(); ++j) {
                    const E* edge = edges[j];
                    double distFrom = -1;
                    double distTo = -1;
                    if (landmark == edge) {
                        distFrom = 0;
                        distTo = 0;
                    } else {
                        if (maxNumThreads > 0) {
#ifdef HAVE_FOX
                            distFrom = currentTasks[taskIndex]->getFromCost();
                            distTo = currentTasks[taskIndex]->getToCost();
                            delete currentTasks[taskIndex++];
#endif
                        } else {
                            const double sourceDestCost = lmCost + router->recomputeCosts({edge}, defaultVehicle, 0);
                            std::vector<const E*> route;
                            std::vector<const ReversedEdge<E, V>*> reversedRoute;
                            // compute from-distance (skip taz-sources and other unreachable edges)
                            if (edge->getPredecessors().size() > 0 && landmark->getSuccessors().size() > 0) {
                                if (router->compute(landmark, edge, defaultVehicle, 0, route)) {
                                    distFrom = MAX2(0.0, router->recomputeCosts(route, defaultVehicle, 0) - sourceDestCost);
                                    route.clear();
                                }
                            }
                            // compute to-distance (skip unreachable landmarks)
                            if (landmark->getPredecessors().size() > 0 && edge->getSuccessors().size() > 0) {
                                if (router->compute(edge, landmark, defaultVehicle, 0, route)) {
                                    distTo = MAX2(0.0, router->recomputeCosts(route, defaultVehicle, 0) - sourceDestCost);
                                    route.clear();
                                }
                            }
                        }
                    }
                    myFromLandmarkDists[i].push_back(distFrom);
                    myToLandmarkDists[i].push_back(distTo);
                }
            }
        }
        if (!outfile.empty()) {
            std::ostream* ostrm = nullptr;
#ifdef HAVE_ZLIB
            if (StringUtils::endsWith(outfile, ".gz")) {
                ostrm = new zstr::ofstream(outfile.c_str(), std::ios_base::out);
            } else {
#endif
                ostrm = new std::ofstream(outfile.c_str());
#ifdef HAVE_ZLIB
            }
#endif
            if (!ostrm->good()) {
                delete ostrm;
                throw ProcessError(TLF("Could not open file '%' for writing.", outfile));
            }
            WRITE_MESSAGEF(TL("Saving new matrix to '%'."), outfile);
            for (int i = 0; i < numLandMarks; ++i) {
                (*ostrm) << getLandmark(i) << "\n";
            }
            for (int j = 0; j < (int)myFromLandmarkDists[0].size(); ++j) {
                (*ostrm) << edges[j + myFirstNonInternal]->getID();
                for (int i = 0; i < numLandMarks; ++i) {
                    (*ostrm) << " " << myFromLandmarkDists[i][j] << " " << myToLandmarkDists[i][j];
                }
                (*ostrm) << "\n";
            }
            delete ostrm;
        }
    }

    virtual ~LandmarkLookupTable() {
    }

    double lowerBound(const E* from, const E* to, double speed, double speedFactor, double fromEffort, double toEffort) const {
        double result = from->getDistanceTo(to) / speed;
#ifdef ASTAR_DEBUG_LOOKUPTABLE
        if (from->getID() == ASTAR_DEBUG_LOOKUPTABLE_FROM) {
            std::cout << " lowerBound to=" << to->getID() << " result1=" << result << "\n";
        }
#endif
        for (int i = 0; i < (int)myLandmarks.size(); ++i) {
            // a cost of -1 is used to encode unreachability.
            const double fl = myToLandmarkDists[i][from->getNumericalID() - myFirstNonInternal];
            const double tl = myToLandmarkDists[i][to->getNumericalID() - myFirstNonInternal];
            if (fl >= 0 && tl >= 0) {
                const double bound = (fl - tl - toEffort) / speedFactor;
#ifdef ASTAR_DEBUG_LOOKUPTABLE
                if (from->getID() == ASTAR_DEBUG_LOOKUPTABLE_FROM && result < bound) {
                    std::cout << "   landmarkTo=" << getLandmark(i) << " result2=" << bound
                              << " fl=" << fl << " tl=" << tl << "\n";
                }
#endif
                result = MAX2(result, bound);
            }
            const double lt = myFromLandmarkDists[i][to->getNumericalID() - myFirstNonInternal];
            const double lf = myFromLandmarkDists[i][from->getNumericalID() - myFirstNonInternal];
            if (lt >= 0 && lf >= 0) {
                const double bound = (lt - lf - fromEffort) / speedFactor;
#ifdef ASTAR_DEBUG_LOOKUPTABLE
                if (from->getID() == ASTAR_DEBUG_LOOKUPTABLE_FROM && result < bound) {
                    std::cout << "   landmarkFrom=" << getLandmark(i) << " result3=" << bound
                              << " lt=" << lt << " lf=" << lf << "\n";
                }
#endif
                result = MAX2(result, bound);
            }
            if ((tl >= 0 && fl < 0)
                    || (lf >= 0 && lt < 0)) {
                // target unreachable.
#ifdef ASTAR_DEBUG_UNREACHABLE
                std::cout << "   unreachable: from=" << from->getID() << " to=" << to->getID() << " landmark=" << getLandmark(i) << " "
                          << ((tl >= 0 && fl < 0) ? " (toLandmark)" : " (fromLandmark)")
                          << " fl=" << fl << " tl=" << tl << " lt=" << lt << " lf=" << lf
                          << "\n";
#endif
                return UNREACHABLE;
            }
        }
        return result;
    }

    bool consistent() const {
        return false;
    }

private:
    std::map<std::string, int> myLandmarks;
    std::vector<std::vector<double> > myFromLandmarkDists;
    std::vector<std::vector<double> > myToLandmarkDists;
    int myFirstNonInternal;

#ifdef HAVE_FOX
private:
    class WorkerThread : public MFXWorkerThread {
    public:
        WorkerThread(MFXWorkerThread::Pool& pool,
                     SUMOAbstractRouter<E, V>* router,
                     SUMOAbstractRouter<ReversedEdge<E, V>, V>* reverseRouter, const V* vehicle)
            : MFXWorkerThread(pool), myRouter(router), myReversedRouter(reverseRouter), myVehicle(vehicle) {}

        virtual ~WorkerThread() {
            delete myRouter;
            delete myReversedRouter;
        }

        const std::pair<double, double> compute(const E* src, const E* dest, const double costOff) {
            double fromResult = -1.;
            if (myRouter->compute(src, dest, myVehicle, 0, myRoute)) {
                fromResult = MAX2(0.0, myRouter->recomputeCosts(myRoute, myVehicle, 0) + costOff);
                myRoute.clear();
            }
            double toResult = -1.;
            if (myReversedRouter != nullptr) {
                if (myReversedRouter->compute(src->getReversedRoutingEdge(), dest->getReversedRoutingEdge(), myVehicle, 0, myReversedRoute)) {
                    toResult = MAX2(0.0, myReversedRouter->recomputeCosts(myReversedRoute, myVehicle, 0) + costOff);
                    myReversedRoute.clear();
                }
            } else {
                if (myRouter->compute(dest, src, myVehicle, 0, myRoute)) {
                    toResult = MAX2(0.0, myRouter->recomputeCosts(myRoute, myVehicle, 0) + costOff);
                    myRoute.clear();
                }
            }
            return std::make_pair(fromResult, toResult);
        }

    private:
        SUMOAbstractRouter<E, V>* myRouter;
        SUMOAbstractRouter<ReversedEdge<E, V>, V>* myReversedRouter;
        const V* myVehicle;
        std::vector<const E*> myRoute;
        std::vector<const ReversedEdge<E, V>*> myReversedRoute;
    };

    class RoutingTask : public MFXWorkerThread::Task {
    public:
        RoutingTask(const E* src, const E* dest, const double costOff)
            : mySrc(src), myDest(dest), myCostOff(-costOff) {}
        void run(MFXWorkerThread* context) {
            myCost = ((WorkerThread*)context)->compute(mySrc, myDest, myCostOff);
        }
        double getFromCost() {
            return myCost.first;
        }
        double getToCost() {
            return myCost.second;
        }
    private:
        const E* const mySrc;
        const E* const myDest;
        const double   myCostOff;
        std::pair<double, double> myCost;
    private:
        /// @brief Invalidated assignment operator.
        RoutingTask& operator=(const RoutingTask&) = delete;
    };

private:
    /// @brief for multi threaded routing
#endif

    std::string getLandmark(int i) const {
        for (std::map<std::string, int>::const_iterator it = myLandmarks.begin(); it != myLandmarks.end(); ++it) {
            if (it->second == i) {
                return it->first;
            }
        }
        return "";
    }
};

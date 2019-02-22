/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AStarLookupTable.h
/// @author  Jakob Erdmann
/// @date    July 2017
/// @version $Id$
///
// Precomputed landmark distances to speed up the A* routing algorithm
/****************************************************************************/
#ifndef AStarLookupTable_h
#define AStarLookupTable_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <fstream>

#ifdef HAVE_FOX
#include <utils/foxtools/FXWorkerThread.h>
#endif

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
        BinaryInputDevice dev(filename);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                double val;
                dev >> val;
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
    LandmarkLookupTable(const std::string& filename, const std::vector<E*>& edges, SUMOAbstractRouter<E, V>* router, const V* defaultVehicle, const std::string& outfile, const int maxNumThreads) {
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
        std::ifstream strm(filename.c_str());
        if (!strm.good()) {
            throw ProcessError("Could not load landmark-lookup-table from '" + filename + "'.");
        }
        std::ofstream* ostrm = nullptr;
        if (!outfile.empty()) {
            ostrm = new std::ofstream(outfile.c_str());
            if (!ostrm->good()) {
                throw ProcessError("Could not open file '" + outfile + "' for writing.");
            }
        }
        std::string line;
        int numLandMarks = 0;
        while (std::getline(strm, line)) {
            if (line == "") {
                break;
            }
            //std::cout << "'" << line << "'" << "\n";
            StringTokenizer st(line);
            if (st.size() == 1) {
                const std::string lm = st.get(0);
                myLandmarks[lm] = numLandMarks++;
                myFromLandmarkDists.push_back(std::vector<double>(0));
                myToLandmarkDists.push_back(std::vector<double>(0));
                if (ostrm != nullptr) {
                    (*ostrm) << lm << "\n";
                }
            } else {
                assert(st.size() == 4);
                const std::string lm = st.get(0);
                const std::string edge = st.get(1);
                if (numericID[edge] != (int)myFromLandmarkDists[myLandmarks[lm]].size()) {
                    WRITE_WARNING("Unknown or unordered edge '" + edge + "' in landmark file.");
                }
                const double distFrom = StringUtils::toDouble(st.get(2));
                const double distTo = StringUtils::toDouble(st.get(3));
                myFromLandmarkDists[myLandmarks[lm]].push_back(distFrom);
                myToLandmarkDists[myLandmarks[lm]].push_back(distTo);
            }
        }
        if (myLandmarks.empty()) {
            WRITE_WARNING("No landmarks in '" + filename + "', falling back to standard A*.");
            delete ostrm;
            return;
        }
#ifdef HAVE_FOX
        FXWorkerThread::Pool threadPool;
#endif
        for (int i = 0; i < (int)myLandmarks.size(); ++i) {
            if ((int)myFromLandmarkDists[i].size() != (int)edges.size() - myFirstNonInternal) {
                const std::string landmarkID = getLandmark(i);
                const E* landmark = nullptr;
                // retrieve landmark edge
                for (const E* const edge : edges) {
                    if (edge->getID() == landmarkID) {
                        landmark = edge;
                        break;
                    }
                }
                if (landmark == nullptr) {
                    WRITE_WARNING("Landmark '" + landmarkID + "' does not exist in the network.");
                    continue;
                }
                if (router != nullptr) {
                    const std::string missing = outfile.empty() ? filename + ".missing" : outfile;
                    WRITE_WARNING("Not all network edges were found in the lookup table '" + filename + "' for landmark '" + landmarkID + "'. Saving missing values to '" + missing + "'.");
                    if (ostrm == nullptr) {
                        ostrm = new std::ofstream(missing.c_str());
                        if (!ostrm->good()) {
                            throw ProcessError("Could not open file '" + missing + "' for writing.");
                        }
                    }
                } else {
                    throw ProcessError("Not all network edges were found in the lookup table '" + filename + "' for landmark '" + landmarkID + "'.");
                }
                std::vector<const E*> routeLM(1, landmark);
                const double lmCost = router->recomputeCosts(routeLM, defaultVehicle, 0);
                std::vector<const E*> route;
#ifdef HAVE_FOX
                if (maxNumThreads > 0) {
                    if (threadPool.size() == 0) {
                        // The CHRouter needs initialization
                        // before it gets cloned, so we do a dummy routing which is not in parallel
                        router->compute(landmark, landmark, defaultVehicle, 0, route);
                        route.clear();
                        while ((int)threadPool.size() < maxNumThreads) {
                            new WorkerThread(threadPool, router->clone(), defaultVehicle);
                        }
                    }
                    std::vector<RoutingTask*> currentTasks;
                    for (int j = (int)myFromLandmarkDists[i].size() + myFirstNonInternal; j < (int)edges.size(); ++j) {
                        const E* edge = edges[j];
                        if (landmark != edge) {
                            std::vector<const E*> routeE(1, edge);
                            const double sourceDestCost = lmCost + router->recomputeCosts(routeE, defaultVehicle, 0);
                            // compute from-distance (skip taz-sources and other unreachable edges)
                            if (edge->getPredecessors().size() > 0 && landmark->getSuccessors().size() > 0) {
                                currentTasks.push_back(new RoutingTask(landmark, edge, sourceDestCost));
                                threadPool.add(currentTasks.back());
                            }
                            // compute to-distance (skip unreachable landmarks)
                            if (landmark->getPredecessors().size() > 0 && edge->getSuccessors().size() > 0) {
                                currentTasks.push_back(new RoutingTask(edge, landmark, sourceDestCost));
                                threadPool.add(currentTasks.back());
                            }
                        }
                    }
                    threadPool.waitAll(false);
                    int taskIndex = 0;
                    for (int j = (int)myFromLandmarkDists[i].size() + myFirstNonInternal; j < (int)edges.size(); ++j) {
                        const E* edge = edges[j];
                        double distFrom = -1;
                        double distTo = -1;
                        if (landmark == edge) {
                            distFrom = 0;
                            distTo = 0;
                        } else {
                            if (edge->getPredecessors().size() > 0 && landmark->getSuccessors().size() > 0) {
                                distFrom = currentTasks[taskIndex]->getCost();
                                delete currentTasks[taskIndex++];
                            }
                            if (landmark->getPredecessors().size() > 0 && edge->getSuccessors().size() > 0) {
                                distTo = currentTasks[taskIndex]->getCost();
                                delete currentTasks[taskIndex++];
                            }
                        }
                        myFromLandmarkDists[i].push_back(distFrom);
                        myToLandmarkDists[i].push_back(distTo);
                        (*ostrm) << landmarkID << " " << edge->getID() << " " << distFrom << " " << distTo << "\n";
                    }
                    currentTasks.clear();
                    continue;
                }
#else
                UNUSED_PARAMETER(maxNumThreads);
#endif
                for (int j = (int)myFromLandmarkDists[i].size() + myFirstNonInternal; j < (int)edges.size(); ++j) {
                    const E* edge = edges[j];
                    double distFrom = -1.;
                    double distTo = -1.;
                    if (landmark == edge) {
                        distFrom = 0.;
                        distTo = 0.;
                    } else {
                        std::vector<const E*> routeE(1, edge);
                        const double sourceDestCost = lmCost + router->recomputeCosts(routeE, defaultVehicle, 0);
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
                    myFromLandmarkDists[i].push_back(distFrom);
                    myToLandmarkDists[i].push_back(distTo);
                    (*ostrm) << landmarkID << " " << edge->getID() << " " << distFrom << " " << distTo << "\n";
                }
            }
        }
        delete ostrm;
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
    class WorkerThread : public FXWorkerThread {
    public:
        WorkerThread(FXWorkerThread::Pool& pool,
                     SUMOAbstractRouter<E, V>* router, const V* vehicle)
            : FXWorkerThread(pool), myRouter(router), myVehicle(vehicle) {}
        virtual ~WorkerThread() {
            delete myRouter;
        }
        double compute(const E* src, const E* dest, const double costOff) {
            double result = -1.;
            if (myRouter->compute(src, dest, myVehicle, 0, myRoute)) {
                result = MAX2(0.0, myRouter->recomputeCosts(myRoute, myVehicle, 0) + costOff);
                myRoute.clear();
            }
            return result;
        }
    private:
        SUMOAbstractRouter<E, V>* myRouter;
        const V* myVehicle;
        std::vector<const E*> myRoute;
    };

    class RoutingTask : public FXWorkerThread::Task {
    public:
        RoutingTask(const E* src, const E* dest, const double costOff)
            : mySrc(src), myDest(dest), myCost(-costOff) {}
        void run(FXWorkerThread* context) {
            myCost = ((WorkerThread*)context)->compute(mySrc, myDest, myCost);
        }
        double getCost() {
            return myCost;
        }
    private:
        const E* const mySrc;
        const E* const myDest;
        double myCost;
    private:
        /// @brief Invalidated assignment operator.
        RoutingTask& operator=(const RoutingTask&);
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




#endif

/****************************************************************************/


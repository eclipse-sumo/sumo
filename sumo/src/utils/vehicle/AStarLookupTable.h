/****************************************************************************/
/// @file    LandmarkLookupTable.h
/// @author  Jakob Erdmann
/// @date    July 2017
/// @version $Id: LandmarkLookupTable.h 24460 2017-05-24 10:56:32Z namdre $
///
// Precomputed landmark distances to speed up the A* routing algorithm
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef AStarLookupTable_h
#define AStarLookupTable_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <fstream>

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
        myTable(size)
    {
        BinaryInputDevice dev(filename);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                double val;
                dev >> val;
                myTable[i].push_back(val);
            }
        }
    }

    double lowerBound(const E* from, const E* to, double /*speed*/, double speedFactor, double /*fromEffort*/, double /*toEffort*/) const { 
        return myTable[from->getNumericalID()][to->getNumericalID()] / speedFactor;
    }

    bool consistent() const { return true; }

private:
    std::vector<std::vector<double> > myTable;
};


template<class E, class V>
class LandmarkLookupTable : public AbstractLookupTable<E, V> {
public:
    LandmarkLookupTable(const std::string& filename, const std::vector<E*>& edges, SUMOAbstractRouter<E, V>* router, const V* defaultVehicle) {
        std::ifstream strm(filename.c_str());
        if (!strm.good()) {
            throw ProcessError("Could not load landmark-lookup-table from '" + filename + "'.");
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
                myLandmarks[line] = numLandMarks++;
                myFromLandmarkDists.push_back(std::vector<double>(0));
                myToLandmarkDists.push_back(std::vector<double>(0));
            } else {
                assert(st.size() == 4); 
                const std::string lm = st.get(0);
                const std::string edge = st.get(1);
                double distFrom = TplConvert::_2double(st.get(2).c_str());
                double distTo = TplConvert::_2double(st.get(3).c_str());
                myFromLandmarkDists[myLandmarks[lm]].push_back(distFrom);
                myToLandmarkDists[myLandmarks[lm]].push_back(distTo);
            }
        }
        const std::string missing = filename + ".missing";
        std::ofstream ostrm(missing.c_str());
        for (int i = 0; i < (int)myLandmarks.size(); ++i) {
            if (myFromLandmarkDists[i].size() != edges.size()) {
                const std::string landmarkID = getLandmark(i);
                const E* landmark = 0;
                // retrieve landmark edge
                for (typename std::vector<E*>::const_iterator it_e = edges.begin(); it_e != edges.end(); ++it_e) {
                    if ((*it_e)->getID() == landmarkID) {
                        landmark = *it_e;
                    }
                }
                if (landmark == 0) {
                    WRITE_WARNING("Landmark '" + landmarkID + "' does not exist in the network.");
                    continue;
                }
                if (router != 0) {
                    WRITE_WARNING("Not all network edges were found in the lookup table '" + filename + "' for landmark '" + landmarkID + "'. Saving missing values to '" + missing + "'.");
                    if (!ostrm.good()) {
                        throw ProcessError("Could not open file '" + missing + "' for writing.");
                    }
                } else {
                    throw ProcessError("Not all network edges were found in the lookup table '" + filename + "' for landmark '" + landmarkID + "'.");
                }
                std::vector<const E*> routeLM(1, landmark); 
                const double lmCost = router->recomputeCosts(routeLM, defaultVehicle, 0);
                for (int j = myFromLandmarkDists[i].size(); j < edges.size(); ++j) {
                    double distFrom = -1;
                    double distTo = -1;
                    std::vector<const E*> route; 
                    std::vector<const E*> routeE(1, edges[j]); 
                    const double sourceDestCost = lmCost + router->recomputeCosts(routeE, defaultVehicle, 0);
                    // compute from-distance
                    router->compute(landmark, edges[j], defaultVehicle, 0, route);
                    if (route.size() > 0) {
                        distFrom = MAX2(0.0, router->recomputeCosts(route, defaultVehicle, 0) - sourceDestCost);
                    }
                    // compute to-distance
                    route.clear();
                    router->compute(edges[j], landmark, defaultVehicle, 0, route);
                    if (route.size() > 0) {
                        distTo = MAX2(0.0, router->recomputeCosts(route, defaultVehicle, 0) - sourceDestCost);
                    }
                    myFromLandmarkDists[i].push_back(distFrom);
                    myToLandmarkDists[i].push_back(distTo);
                    ostrm << landmarkID << " " << edges[j]->getID() << " " << distFrom << " " << distTo << "\n";
                }
            }
        }
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
            const double fl = myToLandmarkDists[i][from->getNumericalID()];
            const double tl = myToLandmarkDists[i][to->getNumericalID()];
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
            const double lt = myFromLandmarkDists[i][to->getNumericalID()];
            const double lf = myFromLandmarkDists[i][from->getNumericalID()];
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
                    <<  ((tl >= 0 && fl < 0) ? " (toLandmark)" : " (fromLandmark)")
                    << " fl=" << fl << " tl=" << tl << " lt=" << lt << " lf=" << lf 
                    << "\n";
#endif
                return UNREACHABLE;
            }
        }
        return result;
    }

    bool consistent() const { return false; }

private:
    std::map<std::string, int> myLandmarks;
    std::vector<std::vector<double> > myFromLandmarkDists;
    std::vector<std::vector<double> > myToLandmarkDists;

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


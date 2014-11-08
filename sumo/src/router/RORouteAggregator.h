/****************************************************************************/
/// @file    RORouteAggregator.h
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    February 2012
/// @version $Id$
///
// Handles grouping of routes to supply input for BulkStarRouter
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RORouteAggregator_h
#define RORouteAggregator_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <algorithm>
#include <foreign/rtree/RTree.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/UtilExceptions.h>
#include <router/ROEdge.h>
#include <utils/vehicle/BulkStarRouter.h>


// ===========================================================================
// class definitions
// ===========================================================================
//
class RORouteAggregator {

public:

    /** @brief precomputes all routes grouped by their destination edge
     * @note: the current setup is not suitable for using RORouteDef_Complete
     * along with --repair since the sequence is not optimized for in-between destinations
     */
    static void processAllRoutes(RONet& net, SUMOAbstractRouter<ROEdge, ROVehicle>& router) {
        // simple version: group by destination edge
        SameTargetMap stm;
        NamedObjectCont<ROVehicle*>::IDMap vehicles = net.getVehicles().getMyMap();
        if (vehicles.size() == 0) {
            throw ProcessError("No vehicles loaded");
        }
        // XXX @todo: use a configurable default vehicle
        const ROVehicle* defaultVehicle = vehicles.begin()->second;
        for (NamedObjectCont<ROVehicle*>::IDMap::const_iterator it = vehicles.begin(); it != vehicles.end(); it++) {
            ROVehicle* veh = it->second;
            stm[veh->getRouteDefinition()->getDestination()].push_back(veh);
        }
        WRITE_MESSAGE("Loaded " + toString(vehicles.size()) + " vehicles with " + toString(stm.size()) + " unique destinations");

        // merge nearby destinations for efficiency
        const int MERGE_DISTANCE = 6; // XXX @todo make configurable
        for (int i = 1; i <= MERGE_DISTANCE; i++) {
            stm = mergeTargets(stm, MERGE_DISTANCE);
        }
        WRITE_MESSAGE("Kept " + toString(stm.size()) + " unique destinations after merging");

        // skip precomputation if not enough vehicles have the same destination
        // this value could be set automatically: num_edges / avg_number_of_nodes_visited_for_astar
        const size_t SKIP_LIMIT = 11; // XXX @todo make configurable
        int num_prepared = 0;
        int num_unprepared = 0;
        int num_routes_prepared = 0;
        int num_routes_unprepared = 0;
        // process by destination edge
        for (SameTargetMap::iterator it = stm.begin(); it != stm.end(); it++) {
            const ROEdge* dest = it->first;
            VehVec& bulkVehicles = it->second;
            bool skip = false;
            if (bulkVehicles.size() < SKIP_LIMIT) {
                skip = true;
                num_routes_unprepared += (int)bulkVehicles.size();
                num_unprepared += 1;
            } else {
                num_routes_prepared += (int)bulkVehicles.size();
                num_prepared += 1;
            }
            router.prepare(dest, defaultVehicle, skip);
            for (VehVec::iterator it = bulkVehicles.begin(); it != bulkVehicles.end(); it++) {
                ROVehicle* veh = *it;
                RORouteDef* routeDef = veh->getRouteDefinition();
                routeDef->preComputeCurrentRoute(router, veh->getDepartureTime(), *veh);
            }
        }
        WRITE_MESSAGE("Performed pre-computation for " + toString(num_prepared) + " destinations");
        WRITE_MESSAGE("Skipped pre-computation for " + toString(num_unprepared) + " destinations");
        WRITE_MESSAGE("Computed " + toString(num_routes_prepared) + " routes with pre-computation");
        WRITE_MESSAGE("Computed " + toString(num_routes_unprepared) + " routes without pre-computation");
    }

private:
    typedef std::vector<ROVehicle*> VehVec;
    typedef std::map<const ROEdge*, VehVec> SameTargetMap;
    typedef std::set<const ROEdge*> EdgeSet;


    /** Function-object for sorting from highest to lowest vehicle count. */
    struct ComparatorNumVehicles {

        ComparatorNumVehicles(SameTargetMap& sameTargetMap):
            mySameTargetMap(sameTargetMap) {}

        bool operator()(const ROEdge* const a, const ROEdge* const b) {
            return (mySameTargetMap[a].size() > mySameTargetMap[b].size());
        }

        SameTargetMap& mySameTargetMap;

    private:
        /// @brief Invalidated assignment operator.
        ComparatorNumVehicles& operator=(const ComparatorNumVehicles&);

    };


    static SameTargetMap mergeTargets(SameTargetMap& stm, const int distance) {
        SameTargetMap result;
        // we want to merg edges with few vehicles to edges with many vehicles
        // so we have to sort by number of vehicles first
        std::vector<const ROEdge*> heap;
        heap.reserve(stm.size());
        ComparatorNumVehicles cmp(stm);
        for (SameTargetMap::iterator it = stm.begin(); it != stm.end(); it++) {
            heap.push_back(it->first);
        }
        make_heap(heap.begin(), heap.end(), cmp);
        while (heap.size() > 0) {
            const ROEdge* dest = heap.front();
            pop_heap(heap.begin(), heap.end(), cmp);
            heap.pop_back();
            if (stm.count(dest) > 0 &&       // dest has not been merged yet
                    stm[dest].size() > 0) {  // for some strange reason 0-length vectors are found despite erase
                result[dest] = stm[dest];
                stm.erase(dest);
                EdgeSet nearby = getNearby(dest, distance);
                for (EdgeSet::iterator it = nearby.begin(); it != nearby.end(); it++) {
                    const ROEdge* nearEdge = *it;
                    if (stm.count(nearEdge) > 0) {
                        // nearEdge occurs as destination and has not been merged yet
                        result[dest].insert(result[dest].end(), stm[nearEdge].begin(), stm[nearEdge].end());
                        stm.erase(nearEdge);
                    }
                }
            }
        }
        return result;
    }


    static EdgeSet getNearby(const ROEdge* edge, const int distance) {
        EdgeSet result;
        result.insert(edge);
        EdgeSet fringe(result);
        for (int i = 0; i < distance; i++) {
            fringe = approachingEdges(fringe);
            result.insert(fringe.begin(), fringe.end());
        }
        return result;
    }


    static EdgeSet approachingEdges(EdgeSet edges) {
        EdgeSet result;
        for (EdgeSet::iterator it = edges.begin(); it != edges.end(); it++) {
            const unsigned int length_size = (*it)->getNumPredecessors();
            for (unsigned int i = 0; i < length_size; i++) {
                result.insert((*it)->getPredecessor(i));
            }
        }
        return result;
    }


};


#endif

/****************************************************************************/


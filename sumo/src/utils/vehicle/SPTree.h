/****************************************************************************/
/// @file    SPTree.h
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    February 2012
/// @version $Id$
///
// Shortest Path tree of limited depth using Dijkstras algorithm
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SPTree_h
#define SPTree_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <functional>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include <iterator>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>


template<class E, class C>
class SPTree {

public:
    typedef std::vector<C> CHConnections;
    typedef std::pair<const C*, const C*> CHConnectionPair;
    typedef std::vector<CHConnectionPair> CHConnectionPairs;

    /**
     * @class EdgeInfoByEffortComparator
     * Class to compare (and so sort) nodes by their effort
     */
    class EdgeByTTComparator {
    public:
        /// Comparing method
        bool operator()(const E* a, const E* b) const {
            if (a->traveltime == b->traveltime) {
                return a->edge->getNumericalID() > b->edge->getNumericalID();
            }
            return a->traveltime > b->traveltime;
        }
    };


    /**
     * @brief Constructor
     */
    SPTree(int maxDepth, bool validatePermissions) :
        myMaxDepth(maxDepth),
        myValidatePermissions(validatePermissions)
    { }


    void init() {
        // all EdgeInfos touched in the previous query are either in myFrontier or myFound: clean those up
        for (typename std::vector<E*>::iterator i = myFrontier.begin(); i != myFrontier.end(); i++) {
            (*i)->reset();
        }
        myFrontier.clear();
        for (typename std::vector<E*>::iterator i = myFound.begin(); i != myFound.end(); i++) {
            (*i)->reset();
        }
        myFound.clear();
    }


    /**
     * @brief build a shortest path tree from start to a depth of myMaxdepth. The given
     * edge is excluded from this tree
     */
    void rebuildFrom(E* start, const E* excluded) {
        init();
        start->traveltime = 0;
        start->depth = 0;
        start->permissions = start->edge->getPermissions();
        myFrontier.push_back(start);
        // build SPT
        while (!myFrontier.empty()) {
            E* min = myFrontier.front();
            pop_heap(myFrontier.begin(), myFrontier.end(), myCmp);
            myFrontier.pop_back();
            myFound.push_back(min);
            min->visited = true;
            if (min->depth < myMaxDepth) {
                for (typename CHConnections::iterator it = min->followers.begin(); it != min->followers.end(); it++) {
                    C& con = *it;
                    E* follower = con.target;
                    if (follower == excluded) {
                        continue;
                    }
                    const SUMOReal traveltime = min->traveltime + con.cost;
                    const SUMOReal oldTraveltime = follower->traveltime;
                    if (!follower->visited && traveltime < oldTraveltime) {
                        follower->traveltime = traveltime;
                        follower->depth = min->depth + 1;
                        follower->permissions = (min->permissions & con.permissions);
                        if (oldTraveltime == std::numeric_limits<SUMOReal>::max()) {
                            myFrontier.push_back(follower);
                            push_heap(myFrontier.begin(), myFrontier.end(), myCmp);
                        } else {
                            push_heap(myFrontier.begin(),
                                      find(myFrontier.begin(), myFrontier.end(), follower) + 1,
                                      myCmp);
                        }
                    }
                }
            }
        }
    }


    /// @brief whether permissions should be validated;
    inline bool validatePermissions() {
        return myValidatePermissions;
    }

    /// @brief save source/target pair for later validation
    void registerForValidation(const C* aInfo, const C* fInfo) {
        assert(myValidatePermissions);
        myShortcutsToValidate.push_back(CHConnectionPair(aInfo, fInfo));
    }


    /* @brief for each path source->excluded->target try to find a witness with a witness
     * with equal permissions */
    const CHConnectionPairs& getNeededShortcuts(const E* excluded) {
        assert(myValidatePermissions);
        myNeededShortcuts.clear();
        for (typename CHConnectionPairs::iterator it = myShortcutsToValidate.begin(); it != myShortcutsToValidate.end(); ++it) {
            const C* const aInfo = it->first;
            const C* const fInfo = it->second;
            const SUMOReal bestWitness = dijkstraTT(
                                             aInfo->target, fInfo->target, excluded, (aInfo->permissions & fInfo->permissions));
            const SUMOReal viaCost = aInfo->cost + fInfo->cost;
            if (viaCost < bestWitness) {
                myNeededShortcuts.push_back(*it);
            }
        }
        myShortcutsToValidate.clear();
        return myNeededShortcuts;
    }


private:
    // perform dijkstra search under permission constraints
    SUMOReal dijkstraTT(E* start, E* dest, const E* excluded, SVCPermissions permissions) {
        init();
        start->traveltime = 0;
        start->depth = 0;
        myFrontier.push_back(start);
        // build SPT
        while (!myFrontier.empty()) {
            E* min = myFrontier.front();
            if (min == dest) {
                return dest->traveltime;
            }
            pop_heap(myFrontier.begin(), myFrontier.end(), myCmp);
            myFrontier.pop_back();
            myFound.push_back(min);
            min->visited = true;
            if (min->depth < myMaxDepth) {
                for (typename CHConnections::iterator it = min->followers.begin(); it != min->followers.end(); it++) {
                    C& con = *it;
                    E* follower = con.target;
                    if (follower == excluded) {
                        continue;
                    }
                    if ((con.permissions & permissions) != permissions) {
                        continue;
                    }
                    const SUMOReal traveltime = min->traveltime + con.cost;
                    const SUMOReal oldTraveltime = follower->traveltime;
                    if (!follower->visited && traveltime < oldTraveltime) {
                        follower->traveltime = traveltime;
                        follower->depth = min->depth + 1;
                        follower->permissions = (min->permissions & con.permissions);
                        if (oldTraveltime == std::numeric_limits<SUMOReal>::max()) {
                            myFrontier.push_back(follower);
                            push_heap(myFrontier.begin(), myFrontier.end(), myCmp);
                        } else {
                            push_heap(myFrontier.begin(),
                                      find(myFrontier.begin(), myFrontier.end(), follower) + 1,
                                      myCmp);
                        }
                    }
                }
            }
        }
        return dest->traveltime;
    }


    // helper method for debugging
    void debugPrintVector(std::vector<E*>& vec, E* start, const E* excluded) {
        std::cout << "computed SPT from '" << start->edge->getID() << "' (excluding " << excluded->edge->getID() <<  ") with " << myFound.size() << " edges\n";
        for (typename std::vector<E*>::iterator it = vec.begin(); it != vec.end(); it++) {
            E* e = *it;
            std::cout << "(" << e->edge->getID() << "," << e->traveltime << ") ";
        }
        std::cout << "\n";
    }

    /// @brief the min edge heap
    std::vector<E*> myFrontier;
    /// @brief the list of visited edges (used when resetting)
    std::vector<E*> myFound;

    /// @brief comparator for search queue
    EdgeByTTComparator myCmp;

    /// @brief maximum search depth
    int myMaxDepth;

    /// @brief whether permissions should be validated
    bool myValidatePermissions;

    /// @brief vector of needed shortcuts after validation
    CHConnectionPairs myShortcutsToValidate;
    /// @brief vector of needed shortcuts after validation
    CHConnectionPairs myNeededShortcuts;
};

#endif

/****************************************************************************/


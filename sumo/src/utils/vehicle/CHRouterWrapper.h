/****************************************************************************/
/// @file    CHRouterWrapper.h
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    March 2012
/// @version $Id$
///
// Wraps multiple CHRouters for different vehicle types
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
#ifndef CHRouterWrapper_h
#define CHRouterWrapper_h


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
#include <utils/common/SysUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/vehicle/SUMOAbstractRouter.h>
#include <utils/common/SUMOVehicleClass.h>
#include "CHRouter.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CHRouterWrapper
 * @brief Computes the shortest path through a contracted network
 *
 * The template parameters are:
 * @param E The edge class to use (MSEdge/ROEdge)
 * @param V The vehicle class to use (MSVehicle/ROVehicle)
 * @param PF The prohibition function to use (prohibited_withRestrictions/prohibited_noRestrictions)
 *
 * The router is edge-based. It must know the number of edges for internal reasons
 *  and whether a missing connection between two given edges (unbuild route) shall
 *  be reported as an error or as a warning.
 *
 */
template<class E, class V, class PF>
class CHRouterWrapper: public SUMOAbstractRouter<E, V>, public PF {

public:
    /// Type of the function that is used to retrieve the edge effort.
    typedef SUMOReal(* Operation)(const E* const, const V* const, SUMOReal);

    /** @brief Constructor
     */
    CHRouterWrapper(size_t dictSize, bool ignoreErrors, Operation operation, SUMOTime begin, SUMOTime weightPeriod):
        SUMOAbstractRouter<E, V>(operation, "CHRouterWrapper"),
        myDictSize(dictSize),
        myIgnoreErrors(ignoreErrors),
        myBegin(begin),
        myWeightPeriod(weightPeriod)
    {}

    ~CHRouterWrapper() {
        for (typename RouterMap::iterator i = myRouters.begin(); i != myRouters.end(); ++i) {
            delete(*i).second;
        }
    }


    virtual SUMOAbstractRouter<E, V>* clone() const {
        return new CHRouterWrapper<E, V, PF>(myDictSize, myIgnoreErrors, this->myOperation, myBegin, myWeightPeriod);
    }

    void compute(const E* from, const E* to, const V* const vehicle,
                 SUMOTime msTime, std::vector<const E*>& into) {
        const std::pair<const SUMOVehicleClass, const SUMOReal> svc = std::make_pair(vehicle->getVClass(), vehicle->getMaxSpeed());
        if (myRouters.count(svc) == 0) {
            // create new router for the given permissions and maximum speed
            // XXX a new router may also be needed if vehicles differ in speed factor
            myRouters[svc] = new CHRouterType(
                myDictSize, myIgnoreErrors, &E::getTravelTimeStatic, svc.first, myWeightPeriod, false);
        }
        myRouters[svc]->compute(from, to, vehicle, msTime, into);
    }


    SUMOReal recomputeCosts(const std::vector<const E*>& edges,
                            const V* const v, SUMOTime msTime) const {
        const SUMOReal time = STEPS2TIME(msTime);
        SUMOReal costs = 0;
        for (typename std::vector<const E*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            if (PF::operator()(*i, v)) {
                WRITE_WARNING("Vehicle '" + v->getID() + "' is restricted from using its assigned route.");
                return -1;
            }
            costs += this->getEffort(*i, v, time + costs);
        }
        return costs;
    }


private:
    typedef CHRouter<E, V, prohibited_noRestrictions<E, V> > CHRouterType;
    typedef std::map<std::pair<const SUMOVehicleClass, const SUMOReal>, CHRouterType*> RouterMap;

    RouterMap myRouters;

    /// @brief number of edges with numerical id
    size_t myDictSize;

    bool myIgnoreErrors;

    SUMOTime myBegin;
    SUMOTime myWeightPeriod;
};


#endif

/****************************************************************************/


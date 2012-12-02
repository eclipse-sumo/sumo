/****************************************************************************/
/// @file    ODMatrix.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    05 Apr. 2006
/// @version $Id$
///
// An O/D (origin/destination) matrix
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "ODMatrix.h"
#include <utils/options/OptionsCont.h>
#include <utils/common/StdDefs.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <iostream>
#include <algorithm>
#include <list>
#include <iterator>
#include <utils/common/RandHelper.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ODMatrix::ODMatrix(const ODDistrictCont& dc)
    : myDistricts(dc), myNoLoaded(0), myNoWritten(0), myNoDiscarded(0) {}


ODMatrix::~ODMatrix() {
    for (std::vector<ODCell*>::iterator i = myContainer.begin(); i != myContainer.end(); ++i) {
        delete *i;
    }
    myContainer.clear();
}


void
ODMatrix::add(SUMOReal vehicleNumber, SUMOTime begin,
              SUMOTime end, const std::string& origin, const std::string& destination,
              const std::string& vehicleType) {
    myNoLoaded += vehicleNumber;
    if (myDistricts.get(origin) == 0 && myDistricts.get(destination) == 0) {
        WRITE_WARNING("Missing origin '" + origin + "' and destination '" + destination + "' (" + toString(vehicleNumber) + " vehicles).");
    } else if (myDistricts.get(origin) == 0 && vehicleNumber > 0) {
        WRITE_ERROR("Missing origin '" + origin + "' (" + toString(vehicleNumber) + " vehicles).");
        myNoDiscarded += vehicleNumber;
    } else if (myDistricts.get(destination) == 0 && vehicleNumber > 0) {
        WRITE_ERROR("Missing destination '" + destination + "' (" + toString(vehicleNumber) + " vehicles).");
        myNoDiscarded += vehicleNumber;
    } else {
        if (myDistricts.get(origin)->sourceNumber() == 0) {
            WRITE_ERROR("District '" + origin + "' has no source.");
            myNoDiscarded += vehicleNumber;
        } else if (myDistricts.get(destination)->sinkNumber() == 0) {
            WRITE_ERROR("District '" + destination + "' has no sink.");
            myNoDiscarded += vehicleNumber;
        } else {
            ODCell* cell = new ODCell();
            cell->begin = begin;
            cell->end = end;
            cell->origin = origin;
            cell->destination = destination;
            cell->vehicleType = vehicleType;
            cell->vehicleNumber = vehicleNumber;
            myContainer.push_back(cell);
        }
    }
}


SUMOReal
ODMatrix::computeDeparts(ODCell* cell,
                         size_t& vehName, std::vector<ODVehicle>& into,
                         bool uniform, const std::string& prefix) {
    int vehicles2insert = (int) cell->vehicleNumber;
    // compute whether the fraction forces an additional vehicle insertion
    SUMOReal mprob = (SUMOReal) cell->vehicleNumber - (SUMOReal) vehicles2insert;
    if (RandHelper::rand() < mprob) {
        vehicles2insert++;
    }

    const SUMOReal offset = (SUMOReal)(cell->end - cell->begin) / (SUMOReal) vehicles2insert / (SUMOReal) 2.;
    for (int i = 0; i < vehicles2insert; ++i) {
        ODVehicle veh;
        veh.id = prefix + toString(vehName++);

        if (uniform) {
            veh.depart = (SUMOTime)(offset + cell->begin + ((SUMOReal)(cell->end - cell->begin) * (SUMOReal) i / (SUMOReal) vehicles2insert));
        } else {
            veh.depart = (SUMOTime)RandHelper::rand(cell->begin, cell->end);
        }

        veh.from = myDistricts.getRandomSourceFromDistrict(cell->origin);
        veh.to = myDistricts.getRandomSinkFromDistrict(cell->destination);
        veh.cell = cell;
        into.push_back(veh);
    }
    return cell->vehicleNumber - vehicles2insert;
}


void
ODMatrix::write(SUMOTime begin, SUMOTime end,
                OutputDevice& dev, bool uniform, bool noVtype,
                const std::string& prefix, bool stepLog) {
    if (myContainer.size() == 0) {
        return;
    }
    OptionsCont& oc = OptionsCont::getOptions();
    std::map<std::pair<std::string, std::string>, SUMOReal> fractionLeft;
    size_t vehName = 0;
    sort(myContainer.begin(), myContainer.end(), cell_by_begin_sorter());
    // recheck begin time
    begin = MAX2(begin, myContainer.front()->begin);
    std::vector<ODCell*>::iterator next = myContainer.begin();
    std::vector<ODVehicle> vehicles;
    SUMOTime lastOut = -DELTA_T;
    // go through the time steps
    for (SUMOTime t = begin; t != end;) {
        if (stepLog && t - lastOut >= DELTA_T) {
            std::cout << "Parsing time " + time2string(t) << '\r';
            lastOut = t;
        }
        // recheck whether a new cell got valid
        bool changed = false;
        while (next != myContainer.end() && (*next)->begin <= t && (*next)->end > t) {
            std::pair<std::string, std::string> odID = std::make_pair((*next)->origin, (*next)->destination);
            // check whether the current cell must be extended by the last fraction
            if (fractionLeft.find(odID) != fractionLeft.end()) {
                (*next)->vehicleNumber += fractionLeft[odID];
                fractionLeft[odID] = 0;
            }
            // get the new departures (into tmp)
            const size_t oldSize = vehicles.size();
            const SUMOReal fraction = computeDeparts(*next, vehName, vehicles, uniform, prefix);
            if (oldSize != vehicles.size()) {
                changed = true;
            }
            if (fraction != 0) {
                fractionLeft[odID] = fraction;
            }
            ++next;
        }
        if (changed) {
            sort(vehicles.begin(), vehicles.end(), descending_departure_comperator());
        }
        for (std::vector<ODVehicle>::reverse_iterator i = vehicles.rbegin(); i != vehicles.rend() && (*i).depart == t; ++i) {
            myNoWritten++;
            dev.openTag(SUMO_TAG_TRIP).writeAttr(SUMO_ATTR_ID, (*i).id).writeAttr(SUMO_ATTR_DEPART, time2string(t)).writeAttr(SUMO_ATTR_FROM, (*i).from).writeAttr(SUMO_ATTR_TO, (*i).to);
            if (!noVtype && (*i).cell->vehicleType.length() != 0) {
                dev.writeAttr(SUMO_ATTR_TYPE, (*i).cell->vehicleType);
            }
            dev.writeAttr(SUMO_ATTR_FROM_TAZ, (*i).cell->origin).writeAttr(SUMO_ATTR_TO_TAZ, (*i).cell->destination);
            if (oc.isSet("departlane") && oc.getString("departlane") != "default") {
                dev.writeAttr(SUMO_ATTR_DEPARTLANE, oc.getString("departlane"));
            }
            if (oc.isSet("departpos")) {
                dev.writeAttr(SUMO_ATTR_DEPARTPOS, oc.getString("departpos"));
            }
            if (oc.isSet("departspeed") && oc.getString("departspeed") != "default") {
                dev.writeAttr(SUMO_ATTR_DEPARTSPEED, oc.getString("departspeed"));
            }
            if (oc.isSet("arrivallane")) {
                dev.writeAttr(SUMO_ATTR_ARRIVALLANE, oc.getString("arrivallane"));
            }
            if (oc.isSet("arrivalpos")) {
                dev.writeAttr(SUMO_ATTR_ARRIVALPOS, oc.getString("arrivalpos"));
            }
            if (oc.isSet("arrivalspeed")) {
                dev.writeAttr(SUMO_ATTR_ARRIVALSPEED, oc.getString("arrivalspeed"));
            }
            dev.closeTag(true);
        }
        while (vehicles.size() != 0 && vehicles.back().depart == t) {
            vehicles.pop_back();
        }
        if (!vehicles.empty()) {
            t = vehicles.back().depart;
        }
        if (next != myContainer.end() && (t > (*next)->begin || vehicles.empty())) {
            t = (*next)->begin;
        }
        if (next == myContainer.end() && vehicles.empty()) {
            break;
        }
    }
}


SUMOReal
ODMatrix::getNoLoaded() const {
    return myNoLoaded;
}


SUMOReal
ODMatrix::getNoWritten() const {
    return myNoWritten;
}


SUMOReal
ODMatrix::getNoDiscarded() const {
    return myNoDiscarded;
}


void
ODMatrix::applyCurve(const Distribution_Points& ps, ODCell* cell, std::vector<ODCell*>& newCells) {
    for (size_t i = 0; i < ps.getAreaNo(); ++i) {
        ODCell* ncell = new ODCell();
        ncell->begin = TIME2STEPS(ps.getAreaBegin(i));
        ncell->end = TIME2STEPS(ps.getAreaEnd(i));
        ncell->origin = cell->origin;
        ncell->destination = cell->destination;
        ncell->vehicleType = cell->vehicleType;
        ncell->vehicleNumber = cell->vehicleNumber * ps.getAreaPerc(i);
        newCells.push_back(ncell);
    }
}


void
ODMatrix::applyCurve(const Distribution_Points& ps) {
    std::vector<ODCell*> oldCells = myContainer;
    myContainer.clear();
    for (std::vector<ODCell*>::iterator i = oldCells.begin(); i != oldCells.end(); ++i) {
        std::vector<ODCell*> newCells;
        applyCurve(ps, *i, newCells);
        copy(newCells.begin(), newCells.end(), back_inserter(myContainer));
        delete *i;
    }
}



/****************************************************************************/


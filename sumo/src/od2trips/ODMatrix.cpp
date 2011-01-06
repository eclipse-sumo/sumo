/****************************************************************************/
/// @file    ODMatrix.cpp
/// @author  Daniel Krajzewicz
/// @date    05 Apr. 2006
/// @version $Id$
///
// An O/D (origin/destination) matrix
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
ODMatrix::ODMatrix(const ODDistrictCont &dc) throw()
        : myDistricts(dc), myNoLoaded(0), myNoWritten(0), myNoDiscarded(0) {}


ODMatrix::~ODMatrix() throw() {
    for (CellVector::iterator i=myContainer.begin(); i!=myContainer.end(); ++i) {
        delete *i;
    }
    myContainer.clear();
}


void
ODMatrix::add(SUMOReal vehicleNumber, SUMOTime begin,
              SUMOTime end, const std::string &origin, const std::string &destination,
              const std::string &vehicleType) throw() {
    myNoLoaded += vehicleNumber;
    if (myDistricts.get(origin)==0&&myDistricts.get(destination)==0) {
        MsgHandler::getWarningInstance()->inform("Missing origin '" + origin + "' and destination '" + destination + "' (" + toString(vehicleNumber) + " vehicles).");
    } else if (myDistricts.get(origin)==0&&vehicleNumber>0) {
        MsgHandler::getErrorInstance()->inform("Missing origin '" + origin + "' (" + toString(vehicleNumber) + " vehicles).");
        myNoDiscarded += vehicleNumber;
    } else if (myDistricts.get(destination)==0&&vehicleNumber>0) {
        MsgHandler::getErrorInstance()->inform("Missing destination '" + destination + "' (" + toString(vehicleNumber) + " vehicles).");
        myNoDiscarded += vehicleNumber;
    } else {
        if (myDistricts.get(origin)->sourceNumber()==0) {
            MsgHandler::getErrorInstance()->inform("District '" + origin + "' has no source.");
            myNoDiscarded += vehicleNumber;
        } else if (myDistricts.get(destination)->sinkNumber()==0) {
            MsgHandler::getErrorInstance()->inform("District '" + destination + "' has no sink.");
            myNoDiscarded += vehicleNumber;
        } else {
            ODCell *cell = new ODCell();
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
ODMatrix::computeEmissions(ODCell *cell,
                           size_t &vehName, std::vector<ODVehicle> &into,
                           bool uniform, const std::string &prefix) throw() {
    int vehicles2insert = (int) cell->vehicleNumber;
    // compute whether the fraction forces an additional vehicle emission
    SUMOReal mrand = RandHelper::rand();
    SUMOReal mprob = (SUMOReal) cell->vehicleNumber - (SUMOReal) vehicles2insert;
    if (mrand<mprob) {
        vehicles2insert++;
    }

    SUMOReal offset = (SUMOReal)(cell->end - cell->begin) / (SUMOReal) vehicles2insert / (SUMOReal) 2.;
    for (int i=0; i<vehicles2insert; ++i) {
        ODVehicle veh;
        veh.id = prefix + toString(vehName++);

        if (uniform) {
            veh.depart = (unsigned int)(offset + cell->begin + ((SUMOReal)(cell->end - cell->begin) * (SUMOReal) i / (SUMOReal) vehicles2insert));
        } else {
            veh.depart = (unsigned int) RandHelper::rand((int) cell->begin, (int) cell->end);
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
                OutputDevice &dev, bool uniform, bool noVtype,
                const std::string &prefix) throw() {
    if (myContainer.size()==0) {
        return;
    }
    OptionsCont &oc = OptionsCont::getOptions();
    std::map<std::pair<std::string, std::string>, SUMOReal> fractionLeft;
    size_t vehName = 0;
    sort(myContainer.begin(), myContainer.end(), cell_by_begin_sorter());
    // recheck begin time
    ODCell *first = *myContainer.begin();
    begin = MAX2(begin, first->begin);
    CellVector::iterator next = myContainer.begin();
    std::vector<ODVehicle> vehicles;
    // go through the time steps
    for (SUMOTime t=begin; t!=end; t++) {
        MsgHandler::getMessageInstance()->progressMsg("Parsing time " + toString(t));
        // recheck whether a new cell got valid
        bool changed = false;
        while (next!=myContainer.end()&&(*next)->begin<=t&&(*next)->end>t) {
            std::pair<std::string, std::string> odID = std::make_pair((*next)->origin, (*next)->destination);
            // check whether the current cell must be extended by the last fraction
            if (fractionLeft.find(odID)!=fractionLeft.end()) {
                (*next)->vehicleNumber += fractionLeft[odID];
                fractionLeft[odID] = 0;
            }
            // get the new emissions (into tmp)
            std::vector<ODVehicle> tmp;
            SUMOReal fraction = computeEmissions(*next, vehName, tmp, uniform, prefix);
            // copy new emissions if any
            if (tmp.size()!=0) {
                copy(tmp.begin(), tmp.end(), back_inserter(vehicles));
                changed = true;
            }
            // save the fraction
            if (fraction!=0) {
                if (fractionLeft.find(odID)==fractionLeft.end()) {
                    fractionLeft[odID] = fraction;
                } else {
                    fractionLeft[odID] += fraction;
                }
            }
            //
            ++next;
        }
        if (changed) {
            sort(vehicles.begin(), vehicles.end(), descending_departure_comperator());
        }
        std::vector<ODVehicle>::reverse_iterator i = vehicles.rbegin();
        for (; i!=vehicles.rend()&&(*i).depart==t; ++i) {
            myNoWritten++;
            dev.openTag("tripdef") << " id=\"" << (*i).id << "\" depart=\"" << t << ".00\" "
            << "from=\"" << (*i).from << "\" "
            << "to=\"" << (*i).to << "\"";
            if (!noVtype&&(*i).cell->vehicleType.length()!=0) {
                dev << " type=\"" << (*i).cell->vehicleType << "\"";
            }
            dev << " fromtaz=\"" << (*i).cell->origin << "\"";
            dev << " totaz=\"" << (*i).cell->destination << "\"";
            if (oc.isSet("departlane") && oc.getString("departlane")!="default") {
                dev << " departlane=\"" << oc.getString("departlane") << "\"";
            }
            if (oc.isSet("departpos")) {
                dev << " departpos=\"" << oc.getString("departpos") << "\"";
            }
            if (oc.isSet("departspeed") && oc.getString("departspeed")!="default") {
                dev << " departspeed=\"" << oc.getString("departspeed") << "\"";
            }
            if (oc.isSet("arrivallane")) {
                dev << " arrivallane=\"" << oc.getString("arrivallane") << "\"";
            }
            if (oc.isSet("arrivalpos")) {
                dev << " arrivalpos=\"" << oc.getString("arrivalpos") << "\"";
            }
            if (oc.isSet("arrivalspeed")) {
                dev << " arrivalspeed=\"" << oc.getString("arrivalspeed") << "\"";
            }
            dev.closeTag(true);
        }
        while (vehicles.size()!=0&&(*vehicles.rbegin()).depart==t) {
            vehicles.pop_back();
        }
    }
}


SUMOReal
ODMatrix::getNoLoaded() const throw() {
    return myNoLoaded;
}


SUMOReal
ODMatrix::getNoWritten() const throw() {
    return myNoWritten;
}


SUMOReal
ODMatrix::getNoDiscarded() const throw() {
    return myNoDiscarded;
}


void
ODMatrix::applyCurve(const Distribution_Points &ps, ODCell *cell, CellVector &newCells) throw() {
    for (size_t i=0; i<ps.getAreaNo(); ++i) {
        ODCell *ncell = new ODCell();
        ncell->begin = (SUMOTime) ps.getAreaBegin(i);
        ncell->end = (SUMOTime) ps.getAreaEnd(i);
        ncell->origin = cell->origin;
        ncell->destination = cell->destination;
        ncell->vehicleType = cell->vehicleType;
        ncell->vehicleNumber = cell->vehicleNumber * ps.getAreaPerc(i);
        newCells.push_back(ncell);
    }
}


void
ODMatrix::applyCurve(const Distribution_Points &ps) throw() {
    CellVector oldCells = myContainer;
    myContainer.clear();
    for (CellVector::iterator i=oldCells.begin(); i!=oldCells.end(); ++i) {
        CellVector newCells;
        applyCurve(ps, *i, newCells);
        copy(newCells.begin(), newCells.end(), back_inserter(myContainer));
        delete *i;
    }
}



/****************************************************************************/


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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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
#include <utils/importio/LineReader.h>
#include <utils/common/StringUtils.h>
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/SUMOTime.h>

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
ODMatrix::writeDefaultAttrs(OutputDevice& dev, const bool noVtype,
                            const ODCell* const cell) {
    const OptionsCont& oc = OptionsCont::getOptions();
    if (!noVtype && cell->vehicleType != "") {
        dev.writeAttr(SUMO_ATTR_TYPE, cell->vehicleType);
    }
    dev.writeAttr(SUMO_ATTR_FROM_TAZ, cell->origin).writeAttr(SUMO_ATTR_TO_TAZ, cell->destination);
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
}


void
ODMatrix::write(SUMOTime begin, const SUMOTime end,
                OutputDevice& dev, const bool uniform, const bool noVtype,
                const std::string& prefix, const bool stepLog) {
    if (myContainer.size() == 0) {
        return;
    }
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
            dev.openTag(SUMO_TAG_TRIP).writeAttr(SUMO_ATTR_ID, (*i).id).writeAttr(SUMO_ATTR_DEPART, time2string(t));
            dev.writeAttr(SUMO_ATTR_FROM, (*i).from).writeAttr(SUMO_ATTR_TO, (*i).to);
            writeDefaultAttrs(dev, noVtype, i->cell);
            dev.closeTag();
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


void
ODMatrix::writeFlows(const SUMOTime begin, const SUMOTime end,
                     OutputDevice& dev, bool noVtype,
                     const std::string& prefix) {
    if (myContainer.size() == 0) {
        return;
    }
    size_t flowName = 0;
    sort(myContainer.begin(), myContainer.end(), cell_by_begin_sorter());
    // recheck begin time
    for (std::vector<ODCell*>::const_iterator i = myContainer.begin(); i != myContainer.end(); ++i) {
        const ODCell* const c = *i;
        if (c->end > begin && c->begin < end) {
            dev.openTag(SUMO_TAG_FLOW).writeAttr(SUMO_ATTR_ID, prefix + toString(flowName++));
            dev.writeAttr(SUMO_ATTR_BEGIN, time2string(c->begin));
            dev.writeAttr(SUMO_ATTR_END, time2string(c->end)).writeAttr(SUMO_ATTR_NUMBER, int(c->vehicleNumber));
            writeDefaultAttrs(dev, noVtype, *i);
            dev.closeTag();
        }
    }
}


std::string
ODMatrix::getNextNonCommentLine(LineReader& lr) {
    std::string line;
    do {
        line = lr.readLine();
        if (line[0] != '*') {
            return StringUtils::prune(line);
        }
    } while (lr.good() && lr.hasMore());
    throw ProcessError();
}


SUMOTime
ODMatrix::parseSingleTime(const std::string& time) {
    if (time.find('.') == std::string::npos) {
        throw OutOfBoundsException();
    }
    std::string hours = time.substr(0, time.find('.'));
    std::string minutes = time.substr(time.find('.') + 1);
    return TIME2STEPS(TplConvert::_2int(hours.c_str()) * 3600 + TplConvert::_2int(minutes.c_str()) * 60);
}


std::pair<SUMOTime, SUMOTime>
ODMatrix::readTime(LineReader& lr) {
    std::string line = getNextNonCommentLine(lr);
    try {
        StringTokenizer st(line, StringTokenizer::WHITECHARS);
        SUMOTime begin = parseSingleTime(st.next());
        SUMOTime end = parseSingleTime(st.next());
        if (begin >= end) {
            throw ProcessError("Begin time is larger than end time.");
        }
        return std::make_pair(begin, end);
    } catch (OutOfBoundsException&) {
        throw ProcessError("Broken period definition '" + line + "'.");
    } catch (NumberFormatException&) {
        throw ProcessError("Broken period definition '" + line + "'.");
    }
}

SUMOReal
ODMatrix::readFactor(LineReader& lr, SUMOReal scale) {
    std::string line = getNextNonCommentLine(lr);
    SUMOReal factor = -1;
    try {
        factor = TplConvert::_2SUMOReal(line.c_str()) * scale;
    } catch (NumberFormatException&) {
        throw ProcessError("Broken factor: '" + line + "'.");
    }
    return factor;
}

void
ODMatrix::readV(LineReader& lr, SUMOReal scale,
                std::string vehType, bool matrixHasVehType) {
    PROGRESS_BEGIN_MESSAGE("Reading matrix '" + lr.getFileName() + "' stored as VMR");
    // parse first defs
    std::string line;
    if (matrixHasVehType) {
        line = getNextNonCommentLine(lr);
        if (vehType == "") {
            vehType = StringUtils::prune(line);
        }
    }

    // parse time
    std::pair<SUMOTime, SUMOTime> times = readTime(lr);
    SUMOTime begin = times.first;
    SUMOTime end = times.second;

    // factor
    SUMOReal factor = readFactor(lr, scale);

    // districts
    line = getNextNonCommentLine(lr);
    int districtNo = TplConvert::_2int(StringUtils::prune(line).c_str());
    // parse district names (normally ints)
    std::vector<std::string> names;
    do {
        line = getNextNonCommentLine(lr);
        StringTokenizer st2(line, StringTokenizer::WHITECHARS);
        while (st2.hasNext()) {
            names.push_back(st2.next());
        }
    } while ((int) names.size() != districtNo);

    // parse the cells
    for (std::vector<std::string>::iterator si = names.begin(); si != names.end(); ++si) {
        std::vector<std::string>::iterator di = names.begin();
        //
        do {
            line = getNextNonCommentLine(lr);
            if (line.length() == 0) {
                continue;
            }
            try {
                StringTokenizer st2(line, StringTokenizer::WHITECHARS);
                while (st2.hasNext()) {
                    assert(di != names.end());
                    SUMOReal vehNumber = TplConvert::_2SUMOReal(st2.next().c_str()) * factor;
                    if (vehNumber != 0) {
                        add(vehNumber, begin, end, *si, *di, vehType);
                    }
                    if (di == names.end()) {
                        throw ProcessError("More entries than districts found.");
                    }
                    ++di;
                }
            } catch (NumberFormatException&) {
                throw ProcessError("Not numeric vehicle number in line '" + line + "'.");
            }
            if (!lr.hasMore()) {
                break;
            }
        } while (di != names.end());
    }
    PROGRESS_DONE_MESSAGE();
}


void
ODMatrix::readO(LineReader& lr, SUMOReal scale,
                std::string vehType, bool matrixHasVehType) {
    PROGRESS_BEGIN_MESSAGE("Reading matrix '" + lr.getFileName() + "' stored as OR");
    // parse first defs
    std::string line;
    if (matrixHasVehType) {
        line = getNextNonCommentLine(lr);
        int type = TplConvert::_2int(StringUtils::prune(line).c_str());
        if (vehType == "") {
            vehType = toString(type);
        }
    }

    // parse time
    std::pair<SUMOTime, SUMOTime> times = readTime(lr);
    SUMOTime begin = times.first;
    SUMOTime end = times.second;

    // factor
    SUMOReal factor = readFactor(lr, scale);

    // parse the cells
    while (lr.hasMore()) {
        line = getNextNonCommentLine(lr);
        if (line.length() == 0) {
            continue;
        }
        StringTokenizer st2(line, StringTokenizer::WHITECHARS);
        if (st2.size() == 0) {
            continue;
        }
        try {
            std::string sourceD = st2.next();
            std::string destD = st2.next();
            SUMOReal vehNumber = TplConvert::_2SUMOReal(st2.next().c_str()) * factor;
            if (vehNumber != 0) {
                add(vehNumber, begin, end, sourceD, destD, vehType);
            }
        } catch (OutOfBoundsException&) {
            throw ProcessError("Missing at least one information in line '" + line + "'.");
        } catch (NumberFormatException&) {
            throw ProcessError("Not numeric vehicle number in line '" + line + "'.");
        }
    }
    PROGRESS_DONE_MESSAGE();
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

void
ODMatrix::loadMatrix(OptionsCont& oc) {
    std::vector<std::string> files = oc.getStringVector("od-files");
    //  check
    if (files.size() == 0) {
        throw ProcessError("No files to parse are given.");
    }
    //  parse
    for (std::vector<std::string>::iterator i = files.begin(); i != files.end(); ++i) {
        LineReader lr(*i);
        if (!lr.good()) {
            throw ProcessError("Could not open '" + (*i) + "'.");
        }
        std::string type = lr.readLine();
        // get the type only
        if (type.find(';') != std::string::npos) {
            type = type.substr(0, type.find(';'));
        }
        // parse type-dependant
        if (type.length() > 1 && type[1] == 'V') {
            // process ptv's 'V'-matrices
            if (type.find('N') != std::string::npos) {
                throw ProcessError("'" + *i + "' does not contain the needed information about the time described.");
            }
            readV(lr, oc.getFloat("scale"), oc.getString("vtype"), type.find('M') != std::string::npos);
        } else if (type.length() > 1 && type[1] == 'O') {
            // process ptv's 'O'-matrices
            if (type.find('N') != std::string::npos) {
                throw ProcessError("'" + *i + "' does not contain the needed information about the time described.");
            }
            readO(lr, oc.getFloat("scale"), oc.getString("vtype"), type.find('M') != std::string::npos);
        } else {
            throw ProcessError("'" + *i + "' uses an unknown matrix type '" + type + "'.");
        }
    }
}


Distribution_Points
ODMatrix::parseTimeLine(const std::vector<std::string>& def, bool timelineDayInHours) {
    bool interpolating = !timelineDayInHours;
    PositionVector points;
    SUMOReal prob = 0;
    if (timelineDayInHours) {
        if (def.size() != 24) {
            throw ProcessError("Assuming 24 entries for a day timeline, but got " + toString(def.size()) + ".");
        }
        for (int chour = 0; chour < 24; ++chour) {
            prob = TplConvert::_2SUMOReal(def[chour].c_str());
            points.push_back(Position((SUMOReal)(chour * 3600), prob));
        }
        points.push_back(Position((SUMOReal)(24 * 3600), prob));
    } else {
        size_t i = 0;
        while (i < def.size()) {
            StringTokenizer st2(def[i++], ":");
            if (st2.size() != 2) {
                throw ProcessError("Broken time line definition: missing a value in '" + def[i - 1] + "'.");
            }
            int time = TplConvert::_2int(st2.next().c_str());
            prob = TplConvert::_2SUMOReal(st2.next().c_str());
            points.push_back(Position((SUMOReal) time, prob));
        }
    }
    return Distribution_Points("N/A", points, interpolating);
}

/****************************************************************************/
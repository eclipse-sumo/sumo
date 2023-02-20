/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2006-2023 German Aerospace Center (DLR) and others.
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
/// @file    ODMatrix.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Yun-Pang Floetteroed
/// @date    05 Apr. 2006
///
// An O/D (origin/destination) matrix
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <algorithm>
#include <list>
#include <iterator>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StdDefs.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/common/RandHelper.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/SUMOTime.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/importio/LineReader.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <router/RORoute.h>
#include "ODAmitranHandler.h"
#include "ODMatrix.h"


// ===========================================================================
// method definitions
// ===========================================================================
ODMatrix::ODMatrix(const ODDistrictCont& dc, double scale) :
    myDistricts(dc),
    myNumLoaded(0),
    myNumWritten(0),
    myNumDiscarded(0),
    myBegin(-1),
    myEnd(-1),
    myScale(scale)
{}


ODMatrix::~ODMatrix() {
    for (ODCell* const cell : myContainer) {
        for (RORoute* const r : cell->pathsVector) {
            delete r;
        }
        delete cell;
    }
    myContainer.clear();
}


bool
ODMatrix::add(double vehicleNumber, const std::pair<SUMOTime, SUMOTime>& beginEnd,
              const std::string& origin, const std::string& destination,
              const std::string& vehicleType, const bool originIsEdge, const bool destinationIsEdge,
              bool noScaling) {
    myNumLoaded += vehicleNumber;
    if (!originIsEdge && !destinationIsEdge && myDistricts.get(origin) == nullptr && myDistricts.get(destination) == nullptr) {
        WRITE_WARNINGF(TL("Missing origin '%' and destination '%' (% vehicles)."), origin, destination, toString(vehicleNumber));
        myNumDiscarded += vehicleNumber;
        myMissingDistricts.insert(origin);
        myMissingDistricts.insert(destination);
        return false;
    } else if (!originIsEdge && myDistricts.get(origin) == 0 && vehicleNumber > 0) {
        WRITE_ERRORF(TL("Missing origin '%' (% vehicles)."), origin, toString(vehicleNumber));
        myNumDiscarded += vehicleNumber;
        myMissingDistricts.insert(origin);
        return false;
    } else if (!destinationIsEdge && myDistricts.get(destination) == 0 && vehicleNumber > 0) {
        WRITE_ERRORF(TL("Missing destination '%' (% vehicles)."), destination, toString(vehicleNumber));
        myNumDiscarded += vehicleNumber;
        myMissingDistricts.insert(destination);
        return false;
    }
    if (!originIsEdge && myDistricts.get(origin)->sourceNumber() == 0) {
        WRITE_ERRORF(TL("District '%' has no source."), origin);
        myNumDiscarded += vehicleNumber;
        return false;
    } else if (!destinationIsEdge && myDistricts.get(destination)->sinkNumber() == 0) {
        WRITE_ERRORF(TL("District '%' has no sink."), destination);
        myNumDiscarded += vehicleNumber;
        return false;
    }
    ODCell* cell = new ODCell();
    cell->begin = beginEnd.first;
    cell->end = beginEnd.second;
    cell->origin = origin;
    cell->destination = destination;
    cell->vehicleType = vehicleType;
    cell->vehicleNumber = vehicleNumber * (noScaling ? 1 : myScale);
    cell->originIsEdge = originIsEdge;
    cell->destinationIsEdge = destinationIsEdge;
    myContainer.push_back(cell);
    if (myBegin == -1 || cell->begin < myBegin) {
        myBegin = cell->begin;
    }
    if (cell->end > myEnd) {
        myEnd = cell->end;
    }
    return true;
}


bool
ODMatrix::add(const std::string& id, const SUMOTime depart,
              const std::string& fromTaz, const std::string& toTaz,
              const std::string& vehicleType, const bool originIsEdge, const bool destinationIsEdge) {
    if (myMissingDistricts.count(fromTaz) > 0 || myMissingDistricts.count(toTaz) > 0) {
        myNumLoaded += 1.;
        myNumDiscarded += 1.;
        return false;
    }
    // we start looking from the end because there is a high probability that the input is sorted by time
    std::vector<ODCell*>& odList = myShortCut[std::make_pair(fromTaz, toTaz)];
    ODCell* cell = nullptr;
    for (std::vector<ODCell*>::const_reverse_iterator c = odList.rbegin(); c != odList.rend(); ++c) {
        if ((*c)->begin <= depart && (*c)->end > depart && (*c)->vehicleType == vehicleType) {
            cell = *c;
            break;
        }
    }
    if (cell == nullptr) {
        const SUMOTime interval = string2time(OptionsCont::getOptions().getString("aggregation-interval"));
        const int intervalIdx = (int)(depart / interval);
        // single vehicles are already scaled
        if (add(1., std::make_pair(intervalIdx * interval, (intervalIdx + 1) * interval),
                fromTaz, toTaz, vehicleType, originIsEdge, destinationIsEdge, true)) {
            cell = myContainer.back();
            odList.push_back(cell);
        } else {
            return false;
        }
    } else {
        myNumLoaded += 1.;
        cell->vehicleNumber += 1.;
    }
    cell->departures[depart].push_back(id);
    return true;
}


double
ODMatrix::computeDeparts(ODCell* cell,
                         int& vehName, std::vector<ODVehicle>& into,
                         const bool uniform, const bool differSourceSink,
                         const std::string& prefix) {
    int vehicles2insert = (int) cell->vehicleNumber;
    // compute whether the fraction forces an additional vehicle insertion
    if (RandHelper::rand() < cell->vehicleNumber - (double)vehicles2insert) {
        vehicles2insert++;
    }
    if (vehicles2insert == 0) {
        return cell->vehicleNumber;
    }

    const double offset = (double)(cell->end - cell->begin) / (double) vehicles2insert / (double) 2.;
    for (int i = 0; i < vehicles2insert; ++i) {
        ODVehicle veh;
        veh.id = prefix + toString(vehName++);

        if (uniform) {
            veh.depart = cell->begin + (SUMOTime)(offset + ((double)(cell->end - cell->begin) * (double) i / (double) vehicles2insert));
        } else {
            veh.depart = (SUMOTime)RandHelper::rand(cell->begin, cell->end);
        }
        const bool canDiffer = myDistricts.get(cell->origin)->sourceNumber() > 1 || myDistricts.get(cell->destination)->sinkNumber() > 1;
        do {
            veh.from = myDistricts.getRandomSourceFromDistrict(cell->origin);
            veh.to = myDistricts.getRandomSinkFromDistrict(cell->destination);
        } while (canDiffer && differSourceSink && (veh.to == veh.from));
        if (!canDiffer && differSourceSink && (veh.to == veh.from)) {
            WRITE_WARNINGF(TL("Cannot find different source and sink edge for origin '%' and destination '%'."), cell->origin, cell->destination);
        }
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
                OutputDevice& dev, const bool uniform,
                const bool differSourceSink, const bool noVtype,
                const std::string& prefix, const bool stepLog,
                bool pedestrians, bool persontrips,
                const std::string& modes) {
    if (myContainer.size() == 0) {
        return;
    }
    std::map<std::pair<std::string, std::string>, double> fractionLeft;
    int vehName = 0;
    sortByBeginTime();
    // recheck begin time
    begin = MAX2(begin, myContainer.front()->begin);
    std::vector<ODCell*>::iterator next = myContainer.begin();
    std::vector<ODVehicle> vehicles;
    SUMOTime lastOut = -DELTA_T;

    const OptionsCont& oc = OptionsCont::getOptions();
    std::string personDepartPos = oc.isSet("departpos") ? oc.getString("departpos") : "random";
    std::string personArrivalPos = oc.isSet("arrivalpos") ? oc.getString("arrivalpos") : "random";
    SumoXMLAttr fromAttr = oc.getBool("junctions") ? SUMO_ATTR_FROMJUNCTION : SUMO_ATTR_FROM;
    SumoXMLAttr toAttr = oc.getBool("junctions") ? SUMO_ATTR_TOJUNCTION : SUMO_ATTR_TO;

    // go through the time steps
    for (SUMOTime t = begin; t < end;) {
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
            const int oldSize = (int)vehicles.size();
            const double fraction = computeDeparts(*next, vehName, vehicles, uniform, differSourceSink, prefix);
            if (oldSize != (int)vehicles.size()) {
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
            if (t >= begin) {
                myNumWritten++;
                if (pedestrians) {
                    dev.openTag(SUMO_TAG_PERSON).writeAttr(SUMO_ATTR_ID, (*i).id).writeAttr(SUMO_ATTR_DEPART, time2string(t));
                    dev.writeAttr(SUMO_ATTR_DEPARTPOS, personDepartPos);
                    dev.openTag(SUMO_TAG_WALK);
                    dev.writeAttr(fromAttr, (*i).from);
                    dev.writeAttr(toAttr, (*i).to);
                    dev.writeAttr(SUMO_ATTR_FROM_TAZ, (*i).cell->origin).writeAttr(SUMO_ATTR_TO_TAZ, (*i).cell->destination);
                    dev.writeAttr(SUMO_ATTR_ARRIVALPOS, personArrivalPos);
                    dev.closeTag();
                    dev.closeTag();
                } else if (persontrips) {
                    dev.openTag(SUMO_TAG_PERSON).writeAttr(SUMO_ATTR_ID, (*i).id).writeAttr(SUMO_ATTR_DEPART, time2string(t));
                    dev.writeAttr(SUMO_ATTR_DEPARTPOS, personDepartPos);
                    dev.openTag(SUMO_TAG_PERSONTRIP);
                    dev.writeAttr(fromAttr, (*i).from);
                    dev.writeAttr(toAttr, (*i).to);
                    dev.writeAttr(SUMO_ATTR_FROM_TAZ, (*i).cell->origin).writeAttr(SUMO_ATTR_TO_TAZ, (*i).cell->destination);
                    dev.writeAttr(SUMO_ATTR_ARRIVALPOS, personArrivalPos);
                    if (modes != "") {
                        dev.writeAttr(SUMO_ATTR_MODES, modes);
                    }
                    dev.closeTag();
                    dev.closeTag();
                } else {
                    dev.openTag(SUMO_TAG_TRIP).writeAttr(SUMO_ATTR_ID, (*i).id).writeAttr(SUMO_ATTR_DEPART, time2string(t));
                    dev.writeAttr(fromAttr, (*i).from);
                    dev.writeAttr(toAttr, (*i).to);
                    writeDefaultAttrs(dev, noVtype, i->cell);
                    dev.closeTag();
                }
            }
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
                     const std::string& prefix,
                     bool asProbability, bool pedestrians, bool persontrips,
                     const std::string& modes) {
    if (myContainer.size() == 0) {
        return;
    }
    int flowName = 0;
    sortByBeginTime();
    // recheck begin time
    for (std::vector<ODCell*>::const_iterator i = myContainer.begin(); i != myContainer.end(); ++i) {
        const ODCell* const c = *i;
        if (c->end > begin && c->begin < end) {
            const double probability = asProbability ? float(c->vehicleNumber) / STEPS2TIME(c->end - c->begin) : 1;
            if (probability <= 0) {
                continue;
            }
            //Person flows
            if (pedestrians) {
                dev.openTag(SUMO_TAG_PERSONFLOW).writeAttr(SUMO_ATTR_ID, prefix + toString(flowName++));
                dev.writeAttr(SUMO_ATTR_BEGIN, time2string(c->begin)).writeAttr(SUMO_ATTR_END, time2string(c->end));
                if (!asProbability) {
                    dev.writeAttr(SUMO_ATTR_NUMBER, int(c->vehicleNumber));
                } else {
                    if (probability > 1) {
                        WRITE_WARNINGF(TL("Flow density of % vehicles per second, cannot be represented with a simple probability. Falling back to even spacing."), toString(probability));
                        dev.writeAttr(SUMO_ATTR_NUMBER, int(c->vehicleNumber));
                    } else {
                        dev.setPrecision(6);
                        dev.writeAttr(SUMO_ATTR_PROB, probability);
                        dev.setPrecision();
                    }
                }
                dev.openTag(SUMO_TAG_WALK);
                dev.writeAttr(SUMO_ATTR_FROM_TAZ, c->origin).writeAttr(SUMO_ATTR_TO_TAZ, c->destination);
                dev.writeAttr(SUMO_ATTR_ARRIVALPOS, "random");
                dev.closeTag();
                dev.closeTag();
            } else if (persontrips) {
                dev.openTag(SUMO_TAG_PERSONFLOW).writeAttr(SUMO_ATTR_ID, prefix + toString(flowName++));
                dev.writeAttr(SUMO_ATTR_BEGIN, time2string(c->begin)).writeAttr(SUMO_ATTR_END, time2string(c->end));
                if (!asProbability) {
                    dev.writeAttr(SUMO_ATTR_NUMBER, int(c->vehicleNumber));
                } else {
                    if (probability > 1) {
                        WRITE_WARNINGF(TL("Flow density of % vehicles per second, cannot be represented with a simple probability. Falling back to even spacing."), toString(probability));
                        dev.writeAttr(SUMO_ATTR_NUMBER, int(c->vehicleNumber));
                    } else {
                        dev.setPrecision(6);
                        dev.writeAttr(SUMO_ATTR_PROB, probability);
                        dev.setPrecision();
                    }
                }
                dev.openTag(SUMO_TAG_PERSONTRIP);
                dev.writeAttr(SUMO_ATTR_FROM_TAZ, c->origin).writeAttr(SUMO_ATTR_TO_TAZ, c->destination);
                dev.writeAttr(SUMO_ATTR_ARRIVALPOS, "random");
                if (modes != "") {
                    dev.writeAttr(SUMO_ATTR_MODES, modes);
                }
                dev.closeTag();
                dev.closeTag();
            } else {
                // Normal flow output
                dev.openTag(SUMO_TAG_FLOW).writeAttr(SUMO_ATTR_ID, prefix + toString(flowName++));
                dev.writeAttr(SUMO_ATTR_BEGIN, time2string(c->begin));
                dev.writeAttr(SUMO_ATTR_END, time2string(c->end));

                if (!asProbability) {
                    dev.writeAttr(SUMO_ATTR_NUMBER, int(c->vehicleNumber));
                } else {
                    if (probability > 1) {
                        WRITE_WARNINGF(TL("Flow density of % vehicles per second, cannot be represented with a simple probability. Falling back to even spacing."), toString(probability));
                        dev.writeAttr(SUMO_ATTR_NUMBER, int(c->vehicleNumber));
                    } else {
                        dev.setPrecision(6);
                        dev.writeAttr(SUMO_ATTR_PROB, probability);
                        dev.setPrecision();
                    }
                }
                writeDefaultAttrs(dev, noVtype, *i);
                dev.closeTag();
            }
        }
    }
}


std::string
ODMatrix::getNextNonCommentLine(LineReader& lr) {
    while (lr.good() && lr.hasMore()) {
        const std::string line = lr.readLine();
        if (line[0] != '*') {
            return StringUtils::prune(line);
        }
    }
    throw ProcessError(TLF("End of file while reading %.", lr.getFileName()));
}


SUMOTime
ODMatrix::parseSingleTime(const std::string& time) {
    if (time.find('.') == std::string::npos) {
        throw NumberFormatException("no separator");
    }
    const std::string hours = time.substr(0, time.find('.'));
    const std::string minutes = time.substr(time.find('.') + 1);
    return TIME2STEPS(StringUtils::toInt(hours) * 3600 + StringUtils::toInt(minutes) * 60);
}


std::pair<SUMOTime, SUMOTime>
ODMatrix::readTime(LineReader& lr) {
    std::string line = getNextNonCommentLine(lr);
    try {
        StringTokenizer st(line, StringTokenizer::WHITECHARS);
        const SUMOTime begin = parseSingleTime(st.next());
        const SUMOTime end = parseSingleTime(st.next());
        if (begin >= end) {
            throw ProcessError("Matrix begin time " + time2string(begin) + " is larger than end time " + time2string(end) + ".");
        }
        return std::make_pair(begin, end);
    } catch (OutOfBoundsException&) {
        throw ProcessError(TLF("Broken period definition '%'.", line));
    } catch (NumberFormatException& e) {
        throw ProcessError("Broken period definition '" + line + "' (" + e.what() + ").");
    }
}


double
ODMatrix::readFactor(LineReader& lr, double scale) {
    std::string line = getNextNonCommentLine(lr);
    double factor = -1;
    try {
        factor = StringUtils::toDouble(line) * scale;
    } catch (NumberFormatException&) {
        throw ProcessError(TLF("Broken factor: '%'.", line));
    }
    return factor;
}

void
ODMatrix::readV(LineReader& lr, double scale,
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

    const std::pair<SUMOTime, SUMOTime> beginEnd = readTime(lr);
    const double factor = readFactor(lr, scale);

    // districts
    line = getNextNonCommentLine(lr);
    const int numDistricts = StringUtils::toInt(StringUtils::prune(line));
    // parse district names (normally ints)
    std::vector<std::string> names;
    while ((int)names.size() != numDistricts && lr.hasMore()) {
        line = getNextNonCommentLine(lr);
        StringTokenizer st2(line, StringTokenizer::WHITECHARS);
        while (st2.hasNext()) {
            names.push_back(st2.next());
        }
    }
    if (!lr.hasMore()) {
        throw ProcessError(TLF("Missing line with % district names.", toString(numDistricts)));
    }

    // parse the cells
    for (std::vector<std::string>::iterator si = names.begin(); si != names.end(); ++si) {
        std::vector<std::string>::iterator di = names.begin();
        do {
            try {
                line = getNextNonCommentLine(lr);
            } catch (ProcessError&) {
                throw ProcessError(TLF("Missing line for district %.", (*si)));
            }
            if (line.length() == 0) {
                continue;
            }
            try {
                StringTokenizer st2(line, StringTokenizer::WHITECHARS);
                while (st2.hasNext()) {
                    assert(di != names.end());
                    double vehNumber = StringUtils::toDouble(st2.next()) * factor;
                    if (vehNumber != 0) {
                        add(vehNumber, beginEnd, *si, *di, vehType);
                    }
                    if (di == names.end()) {
                        throw ProcessError(TL("More entries than districts found."));
                    }
                    ++di;
                }
            } catch (NumberFormatException&) {
                throw ProcessError(TLF("Not numeric vehicle number in line '%'.", line));
            }
            if (!lr.hasMore()) {
                break;
            }
        } while (di != names.end());
    }
    PROGRESS_DONE_MESSAGE();
}


void
ODMatrix::readO(LineReader& lr, double scale,
                std::string vehType, bool matrixHasVehType) {
    PROGRESS_BEGIN_MESSAGE("Reading matrix '" + lr.getFileName() + "' stored as OR");
    // parse first defs
    std::string line;
    if (matrixHasVehType) {
        line = getNextNonCommentLine(lr);
        int type = StringUtils::toInt(StringUtils::prune(line));
        if (vehType == "") {
            vehType = toString(type);
        }
    }

    const std::pair<SUMOTime, SUMOTime> beginEnd = readTime(lr);
    const double factor = readFactor(lr, scale);

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
            double vehNumber = StringUtils::toDouble(st2.next()) * factor;
            if (vehNumber != 0) {
                add(vehNumber, beginEnd, sourceD, destD, vehType);
            }
        } catch (OutOfBoundsException&) {
            throw ProcessError(TLF("Missing at least one information in line '%'.", line));
        } catch (NumberFormatException&) {
            throw ProcessError(TLF("Not numeric vehicle number in line '%'.", line));
        }
    }
    PROGRESS_DONE_MESSAGE();
}



double
ODMatrix::getNumLoaded() const {
    return myNumLoaded;
}


double
ODMatrix::getNumWritten() const {
    return myNumWritten;
}


double
ODMatrix::getNumDiscarded() const {
    return myNumDiscarded;
}


void
ODMatrix::applyCurve(const Distribution_Points& ps, ODCell* cell, std::vector<ODCell*>& newCells) {
    const std::vector<double>& times = ps.getVals();
    for (int i = 0; i < (int)times.size() - 1; ++i) {
        ODCell* ncell = new ODCell();
        ncell->begin = TIME2STEPS(times[i]);
        ncell->end = TIME2STEPS(times[i + 1]);
        ncell->origin = cell->origin;
        ncell->destination = cell->destination;
        ncell->vehicleType = cell->vehicleType;
        ncell->vehicleNumber = cell->vehicleNumber * ps.getProbs()[i] / ps.getOverallProb();
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
    std::vector<std::string> files = oc.getStringVector("od-matrix-files");
    for (std::vector<std::string>::iterator i = files.begin(); i != files.end(); ++i) {
        LineReader lr(*i);
        if (!lr.good()) {
            throw ProcessError(TLF("Could not open '%'.", (*i)));
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
                throw ProcessError(TLF("'%' does not contain the needed information about the time described.", *i));
            }
            readV(lr, 1, oc.getString("vtype"), type.find('M') != std::string::npos);
        } else if (type.length() > 1 && type[1] == 'O') {
            // process ptv's 'O'-matrices
            if (type.find('N') != std::string::npos) {
                throw ProcessError(TLF("'%' does not contain the needed information about the time described.", *i));
            }
            readO(lr, 1, oc.getString("vtype"), type.find('M') != std::string::npos);
        } else {
            throw ProcessError("'" + *i + "' uses an unknown matrix type '" + type + "'.");
        }
    }
    std::vector<std::string> amitranFiles = oc.getStringVector("od-amitran-files");
    for (std::vector<std::string>::iterator i = amitranFiles.begin(); i != amitranFiles.end(); ++i) {
        if (!FileHelpers::isReadable(*i)) {
            throw ProcessError(TLF("Could not access matrix file '%' to load.", *i));
        }
        PROGRESS_BEGIN_MESSAGE("Loading matrix in Amitran format from '" + *i + "'");
        ODAmitranHandler handler(*this, *i);
        if (!XMLSubSys::runParser(handler, *i)) {
            PROGRESS_FAILED_MESSAGE();
        } else {
            PROGRESS_DONE_MESSAGE();
        }
    }
    myVType = oc.getString("vtype");
    for (std::string file : oc.getStringVector("tazrelation-files")) {
        if (!FileHelpers::isReadable(file)) {
            throw ProcessError(TLF("Could not access matrix file '%' to load.", file));
        }
        PROGRESS_BEGIN_MESSAGE("Loading matrix in tazRelation format from '" + file + "'");

        std::vector<SAXWeightsHandler::ToRetrieveDefinition*> retrieverDefs;
        retrieverDefs.push_back(new SAXWeightsHandler::ToRetrieveDefinition(oc.getString("tazrelation-attribute"), true, *this));
        SAXWeightsHandler handler(retrieverDefs, "");
        if (!XMLSubSys::runParser(handler, file)) {
            PROGRESS_FAILED_MESSAGE();
        } else {
            PROGRESS_DONE_MESSAGE();
        }
    }
}

void
ODMatrix::addTazRelWeight(const std::string intervalID, const std::string& from, const std::string& to,
                          double val, double beg, double end) {
    add(val, std::make_pair(TIME2STEPS(beg), TIME2STEPS(end)), from, to, myVType == "" ? intervalID : myVType);
}


void
ODMatrix::loadRoutes(OptionsCont& oc, SUMOSAXHandler& handler) {
    std::vector<std::string> routeFiles = oc.getStringVector("route-files");
    for (std::vector<std::string>::iterator i = routeFiles.begin(); i != routeFiles.end(); ++i) {
        if (!FileHelpers::isReadable(*i)) {
            throw ProcessError(TLF("Could not access route file '%' to load.", *i));
        }
        PROGRESS_BEGIN_MESSAGE("Loading routes and trips from '" + *i + "'");
        if (!XMLSubSys::runParser(handler, *i)) {
            PROGRESS_FAILED_MESSAGE();
        } else {
            PROGRESS_DONE_MESSAGE();
        }
    }
}


Distribution_Points
ODMatrix::parseTimeLine(const std::vector<std::string>& def, bool timelineDayInHours) {
    Distribution_Points result("N/A");
    if (timelineDayInHours) {
        if (def.size() != 24) {
            throw ProcessError(TLF("Assuming 24 entries for a day timeline, but got %.", toString(def.size())));
        }
        for (int chour = 0; chour < 24; ++chour) {
            result.add(chour * 3600., StringUtils::toDouble(def[chour]));
        }
        result.add(24 * 3600., 0.); // dummy value to finish the last interval
    } else {
        for (int i = 0; i < (int)def.size(); i++) {
            StringTokenizer st2(def[i], ":");
            if (st2.size() != 2) {
                throw ProcessError(TLF("Broken time line definition: missing a value in '%'.", def[i]));
            }
            const double time = StringUtils::toDouble(st2.next());
            result.add(time, StringUtils::toDouble(st2.next()));
        }
    }
    return result;
}


void
ODMatrix::sortByBeginTime() {
    std::sort(myContainer.begin(), myContainer.end(), cell_by_begin_comparator());
}


/****************************************************************************/

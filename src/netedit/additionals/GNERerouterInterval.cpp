/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNERerouterInterval.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2017
/// @version $Id$
///
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include "GNERerouterInterval.h"
#include <netedit/netelements/GNEEdge.h>
#include <netedit/netelements/GNELane.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <netedit/dialogs/GNERerouterDialog.h>

#include "GNEClosingLaneReroute.h"
#include "GNEParkingAreaReroute.h"
#include "GNEClosingReroute.h"
#include "GNEDestProbReroute.h"
#include "GNERouteProbReroute.h"
#include "GNERerouter.h"
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/GNENet.h>

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterInterval::GNERerouterInterval(GNERerouterDialog* rerouterDialog) :
    GNEAttributeCarrier(SUMO_TAG_INTERVAL, ICON_EMPTY),
    myRerouterParent(rerouterDialog->getEditedRerouter()),
    myBegin(getDefaultValue<double>(SUMO_TAG_INTERVAL, SUMO_ATTR_BEGIN)),
    myEnd(getDefaultValue<double>(SUMO_TAG_INTERVAL, SUMO_ATTR_END)) {
}


GNERerouterInterval::GNERerouterInterval(GNERerouter* rerouterParent, double begin, double end) :
    GNEAttributeCarrier(SUMO_TAG_INTERVAL, ICON_EMPTY),
    myRerouterParent(rerouterParent),
    myBegin(begin),
    myEnd(end) {
}


GNERerouterInterval::~GNERerouterInterval() {}


void
GNERerouterInterval::writeRerouterInterval(OutputDevice& device) const {
    // openTag
    device.openTag(getTag());
    // write begin
    writeAttribute(device, SUMO_ATTR_BEGIN);
    //write end
    writeAttribute(device, SUMO_ATTR_END);
    // write closing reroutes
    for (auto i : myClosingReroutes) {
        i->writeClosingReroute(device);
    }
    // write closing lane reroutes
    for (auto i : myClosingLaneReroutes) {
        i->writeClosingLaneReroute(device);
    }
    // write dest prob reroutes
    for (auto i : myDestProbReroutes) {
        i->writeDestProbReroute(device);
    }
    // write parkingAreaReroutes
    for (auto i : myParkingAreaReroutes) {
        i->writeParkingAreaReroute(device);
    }
    // write route prob reroutes
    for (auto i : myRouteProbReroutes) {
        i->writeRouteProbReroute(device);
    }
    // Close tag
    device.closeTag();
}


GNERerouter*
GNERerouterInterval::getRerouterParent() const {
    return myRerouterParent;
}


double
GNERerouterInterval::getBegin() const {
    return myBegin;
}


double
GNERerouterInterval::getEnd() const {
    return myEnd;
}


void 
GNERerouterInterval::selectAttributeCarrier(bool) {
    // this AC cannot be selected
}


void 
GNERerouterInterval::unselectAttributeCarrier(bool) {
    // this AC cannot be unselected
}


bool 
GNERerouterInterval::isAttributeCarrierSelected() const {
    // this AC doesn't own a select flag
    return false;
}


std::string
GNERerouterInterval::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return myRerouterParent->getID() + "_" + toString(myBegin) + "_" + toString(myEnd);
        case SUMO_ATTR_BEGIN:
            return toString(myBegin);
        case SUMO_ATTR_END:
            return toString(myEnd);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNERerouterInterval::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_BEGIN:
        case SUMO_ATTR_END:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNERerouterInterval::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_BEGIN:
            return canParse<double>(value) && (parse<double>(value) >= 0) && (parse<double>(value) < myEnd);
        case SUMO_ATTR_END:
            return canParse<double>(value) && (parse<double>(value) >= 0) && (parse<double>(value) > myBegin);
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


const std::vector<GNEClosingLaneReroute*>&
GNERerouterInterval::getClosingLaneReroutes() const {
    return myClosingLaneReroutes;
}


const std::vector<GNEClosingReroute*>&
GNERerouterInterval::getClosingReroutes() const {
    return myClosingReroutes;
}


const std::vector<GNEDestProbReroute*>&
GNERerouterInterval::getDestProbReroutes() const {
    return myDestProbReroutes;
}


const std::vector<GNERouteProbReroute*>&
GNERerouterInterval::getRouteProbReroutes() const {
    return myRouteProbReroutes;
}

const std::vector<GNEParkingAreaReroute*>&
GNERerouterInterval::getParkingAreaReroutes() const {
    return myParkingAreaReroutes;
}


void
GNERerouterInterval::addClosingLaneReroute(GNEClosingLaneReroute* closingLaneReroute) {
    auto it = std::find(myClosingLaneReroutes.begin(), myClosingLaneReroutes.end(), closingLaneReroute);
    if (it == myClosingLaneReroutes.end()) {
        myClosingLaneReroutes.push_back(closingLaneReroute);
    } else {
        throw ProcessError("Closing lane Reroute already exist");
    }
}


void
GNERerouterInterval::removeClosingLaneReroute(GNEClosingLaneReroute* closingLaneReroute) {
    auto it = std::find(myClosingLaneReroutes.begin(), myClosingLaneReroutes.end(), closingLaneReroute);
    if (it != myClosingLaneReroutes.end()) {
        myClosingLaneReroutes.erase(it);
    } else {
        throw ProcessError("Closing lane Reroute doesn't exist");
    }
}


void
GNERerouterInterval::addClosingReroute(GNEClosingReroute* closingReroute) {
    auto it = std::find(myClosingReroutes.begin(), myClosingReroutes.end(), closingReroute);
    if (it == myClosingReroutes.end()) {
        myClosingReroutes.push_back(closingReroute);
    } else {
        throw ProcessError("Closing Reroute already exist");
    }
}


void
GNERerouterInterval::removeClosingReroute(GNEClosingReroute* closingReroute) {
    auto it = std::find(myClosingReroutes.begin(), myClosingReroutes.end(), closingReroute);
    if (it != myClosingReroutes.end()) {
        myClosingReroutes.erase(it);
    } else {
        throw ProcessError("Closing Reroute doesn't exist");
    }
}


void
GNERerouterInterval::addDestProbReroute(GNEDestProbReroute* destProbReroute) {
    auto it = std::find(myDestProbReroutes.begin(), myDestProbReroutes.end(), destProbReroute);
    if (it == myDestProbReroutes.end()) {
        myDestProbReroutes.push_back(destProbReroute);
    } else {
        throw ProcessError("Destiny Probability Reroute already exist");
    }
}


void
GNERerouterInterval::removeDestProbReroute(GNEDestProbReroute* destProbReroute) {
    auto it = std::find(myDestProbReroutes.begin(), myDestProbReroutes.end(), destProbReroute);
    if (it != myDestProbReroutes.end()) {
        myDestProbReroutes.erase(it);
    } else {
        throw ProcessError("Destiny Probability Reroute doesn't exist");
    }
}


void
GNERerouterInterval::addRouteProbReroute(GNERouteProbReroute* routeProbabilityReroute) {
    auto it = std::find(myRouteProbReroutes.begin(), myRouteProbReroutes.end(), routeProbabilityReroute);
    if (it == myRouteProbReroutes.end()) {
        myRouteProbReroutes.push_back(routeProbabilityReroute);
    } else {
        throw ProcessError("Route Probability Reroute already exist");
    }
}


void
GNERerouterInterval::removeRouteProbReroute(GNERouteProbReroute* routeProbabilityReroute) {
    auto it = std::find(myRouteProbReroutes.begin(), myRouteProbReroutes.end(), routeProbabilityReroute);
    if (it != myRouteProbReroutes.end()) {
        myRouteProbReroutes.erase(it);
    } else {
        throw ProcessError("Route Probability Reroute doesn't exist");
    }
}


void
GNERerouterInterval::addParkingAreaReroute(GNEParkingAreaReroute* parkingAreaReroute) {
    auto it = std::find(myParkingAreaReroutes.begin(), myParkingAreaReroutes.end(), parkingAreaReroute);
    if (it == myParkingAreaReroutes.end()) {
        myParkingAreaReroutes.push_back(parkingAreaReroute);
    } else {
        throw ProcessError("parkingAreaReroute already exist");
    }
}


void
GNERerouterInterval::removeParkingAreaReroute(GNEParkingAreaReroute* parkingAreaReroute) {
    auto it = std::find(myParkingAreaReroutes.begin(), myParkingAreaReroutes.end(), parkingAreaReroute);
    if (it != myParkingAreaReroutes.end()) {
        myParkingAreaReroutes.erase(it);
    } else {
        throw ProcessError("parkingAreaReroute doesn't exist");
    }
}



// ===========================================================================
// private
// ===========================================================================

void
GNERerouterInterval::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_BEGIN: {
            myBegin = parse<double>(value);
            break;
        }
        case SUMO_ATTR_END: {
            myEnd = parse<double>(value);
            break;
        }
        default:
            throw InvalidArgument(toString(getTag()) + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

/****************************************************************************/

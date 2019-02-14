/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AGActivityTripWriter.cpp
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Class for writing Trip objects in a SUMO-route file.
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/RGBColor.h>
#include <utils/iodevices/OutputDevice.h>
#include "activities/AGTrip.h"
#include "city/AGStreet.h"
#include "AGActivityTripWriter.h"


// ===========================================================================
// method definitions
// ===========================================================================
AGActivityTripWriter::AGActivityTripWriter(OutputDevice& file) : myTripOutput(file) {
    myTripOutput.openTag(SUMO_TAG_VTYPE)
    .writeAttr(SUMO_ATTR_ID, "default")
    .writeAttr(SUMO_ATTR_VCLASS, "passenger")
    .writeAttr(SUMO_ATTR_COLOR, RGBColor::RED).closeTag();
    myTripOutput.openTag(SUMO_TAG_VTYPE)
    .writeAttr(SUMO_ATTR_ID, "random")
    .writeAttr(SUMO_ATTR_VCLASS, "passenger")
    .writeAttr(SUMO_ATTR_COLOR, RGBColor::BLUE).closeTag();
    myTripOutput.openTag(SUMO_TAG_VTYPE)
    .writeAttr(SUMO_ATTR_ID, "bus")
    .writeAttr(SUMO_ATTR_VCLASS, "bus")
    .writeAttr(SUMO_ATTR_COLOR, RGBColor::GREEN).closeTag();
    myTripOutput.lf();
}


void
AGActivityTripWriter::addTrip(const AGTrip& trip) {
    int time = (trip.getDay() - 1) * 86400 + trip.getTime();

    myTripOutput.openTag(SUMO_TAG_TRIP)
    .writeAttr(SUMO_ATTR_ID, trip.getVehicleName())
    .writeAttr(SUMO_ATTR_TYPE, trip.getType())
    .writeAttr(SUMO_ATTR_DEPART, time2string(TIME2STEPS(time)))
    .writeAttr(SUMO_ATTR_DEPARTPOS, trip.getDep().getPosition())
    .writeAttr(SUMO_ATTR_ARRIVALPOS, trip.getArr().getPosition())
    .writeAttr(SUMO_ATTR_ARRIVALSPEED, 0.)
    .writeAttr(SUMO_ATTR_FROM, trip.getDep().getStreet().getID());

    if (!trip.getPassed()->empty()) {
        std::ostringstream oss;
        for (std::list<AGPosition>::const_iterator it = trip.getPassed()->begin(); it != trip.getPassed()->end(); ++it) {
            if (it != trip.getPassed()->begin()) {
                oss << " ";
            }
            oss << it->getStreet().getID();
        }
        myTripOutput.writeAttr(SUMO_ATTR_VIA, oss.str());
    }
    myTripOutput.writeAttr(SUMO_ATTR_TO, trip.getArr().getStreet().getID());
    myTripOutput.closeTag();
}


/****************************************************************************/

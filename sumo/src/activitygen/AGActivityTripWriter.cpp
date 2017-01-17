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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
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
    .writeAttr(SUMO_ATTR_DEPART, time)
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

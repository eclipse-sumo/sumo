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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include "city/AGStreet.h"
#include "AGActivityTripWriter.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
AGActivityTripWriter::initialize() {
    vtypes();
}

void
AGActivityTripWriter::vtypes() {
    routes << "    <vType id=\"default\" accel=\"4.0\" decel=\"8.0\" sigma=\"0.0\" length=\"5\" minGap=\"2.5\" maxSpeed=\"90\"/>\n";
    routes << "    <vType id=\"random\" accel=\"4.0\" decel=\"8.0\" sigma=\"0.0\" length=\"5\" minGap=\"2.5\" maxSpeed=\"90\"/>\n";
    routes << "    <vType id=\"bus\" accel=\"2.0\" decel=\"4.0\" sigma=\"0.0\" length=\"10\" minGap=\"3\" maxSpeed=\"70\"/>\n\n";

    colors["default"] = "1,0,0";
    colors["bus"] = "0,1,0";
    colors["random"] = "0,0,1";
}

void
AGActivityTripWriter::addTrip(AGTrip trip) {
    std::list<AGPosition>::iterator it;
    int time = (trip.getDay() - 1) * 86400 + trip.getTime();

    //the vehicle:
    routes << "    <vehicle"
           << " id=\"" << trip.getVehicleName()
           << "\" type=\"" << trip.getType()
           << "\" depart=\"" << time
           << "\" departPos=\"" << trip.getDep().getPosition()
           << "\" arrivalPos=\"" << trip.getArr().getPosition()
           << "\" departSpeed=\"" << 0
           << "\" arrivalSpeed=\"" << 0
           << "\" color=\"" << colors[trip.getType()]
           << "\">\n";

    //the route
    routes << "        <route edges=\"" << trip.getDep().getStreet().getName();
    for (it = trip.getPassed()->begin(); it != trip.getPassed()->end(); ++it) {
        routes << " " << it->getStreet().getName();
    }
    routes << " " << trip.getArr().getStreet().getName();
    routes << "\"/>\n";

    routes << "    </vehicle>\n";
}


/****************************************************************************/

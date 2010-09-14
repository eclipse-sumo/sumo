/****************************************************************************/
/// @file    AGActivityTripWriter.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Class for writing Trip objects in a SUMO-route file.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
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

#include "AGActivityTripWriter.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGActivityTripWriter::initialize()
{
	routes << "<?xml version=\"1.0\"?>" << endl << endl;
	routes << "<routes>" << endl;
	vtypes();
}

void
AGActivityTripWriter::vtypes()
{
	routes << "\t<vtype id=\"default\" accel=\"0.8\" decel=\"4.5\" sigma=\"0.5\" length=\"5\" maxspeed=\"70\"/>" << endl;
	routes << "\t<vtype id=\"random\" accel=\"0.8\" decel=\"4.5\" sigma=\"0.5\" length=\"5\" maxspeed=\"70\"/>" << endl;
	routes << "\t<vtype id=\"bus\" accel=\"0.4\" decel=\"2.5\" sigma=\"0.5\" length=\"10\" maxspeed=\"70\"/>" << endl << endl;

	colors["default"] = "1,0,0";
	colors["bus"] = "0,1,0";
	colors["random"] = "0,0,1";
}

void
AGActivityTripWriter::addTrip(AGTrip trip)
{
	list<AGPosition>::iterator it;
	int time = (trip.getDay()-1)*86400 + trip.getTime();

	//the vehicle:
	routes << "\t<vehicle";
	routes << " id=\"" << trip.getVehicleName();
	routes << "\" type=\"" << trip.getType();
	routes << "\" depart=\"" << time;
	routes << "\" departpos=\"" << trip.getDep().getPosition();
	routes << "\" arrivalpos=\"" << trip.getArr().getPosition();
	routes << "\" departspeed=\"" << 0;
	routes << "\" arrivalspeed=\"" << 0;
	routes << "\" color=\"" << colors[trip.getType()];
	routes << "\">" << endl;

	//the route
	routes << "\t\t<route edges=\"";
	routes << trip.getDep().getStreet().getName();
	for(it=trip.getPassed()->begin() ; it!=trip.getPassed()->end() ; ++it)
	{
		routes << " " << it->getStreet().getName();
	}
	routes << " " << trip.getArr().getStreet().getName();
	routes << "\" />" << endl;

	routes << "\t</vehicle>" << endl;
}

void
AGActivityTripWriter::writeOutputFile()
{
	routes << "</routes>" << endl;
	routes.close();
}

/****************************************************************************/

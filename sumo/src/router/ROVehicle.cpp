/****************************************************************************/
/// @file    ROVehicle.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A vehicle as used by router
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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

#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>
#include <string>
#include <iostream>
#include "ROVehicleBuilder.h"
#include "ROVehicleType.h"
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "RORouteDef_Alternatives.h"
#include "RORoute.h"
#include "ROHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
ROVehicle::ROVehicle(const SUMOVehicleParameter &pars,
                     RORouteDef *route, ROVehicleType *type) throw()
        : myParameter(pars), myType(type), myRoute(route)
{}


ROVehicle::~ROVehicle() throw()
{}


void
ROVehicle::saveXMLVehicle(OutputDevice &dev) const throw(IOError)
{
    dev << "<vehicle id=\"" << myParameter.id << "\"";
    if (myParameter.wasSet(VEHPARS_VTYPE_SET)) {
        dev << " type=\"" << myType->getID() << "\"";
    }
    dev << " depart=\"" << myParameter.depart << "\"";

    // optional parameter
    // depart-values
    // departlane
    if (myParameter.wasSet(VEHPARS_DEPARTLANE_SET)) {
        string val;
        switch (myParameter.departLaneProcedure) {
        case DEPART_LANE_GIVEN:
            val = toString(myParameter.departLane);
            break;
        case DEPART_LANE_RANDOM:
            val = "random";
            break;
        case DEPART_LANE_FREE:
            val = "free";
            break;
        case DEPART_LANE_DEPARTLANE:
            val = "departlane";
            break;
        case DEPART_LANE_DEFAULT:
        default:
            cerr << "should not happen..." << endl;
            break;
        }
        dev << " departlane=\"" << val << "\"";
    }
    // departpos
    if (myParameter.wasSet(VEHPARS_DEPARTPOS_SET)) {
        string val;
        switch (myParameter.departPosProcedure) {
        case DEPART_POS_GIVEN:
            val = toString(myParameter.departPos);
            break;
        case DEPART_POS_RANDOM:
            val = "random";
            break;
        case DEPART_POS_FREE:
            val = "free";
        case DEPART_POS_DEFAULT:
        default:
            break;
        }
        dev << " departpos=\"" << val << "\"";
    }
    // departspeed
    if (myParameter.wasSet(VEHPARS_DEPARTSPEED_SET)) {
        string val;
        switch (myParameter.departSpeedProcedure) {
        case DEPART_SPEED_GIVEN:
            val = toString(myParameter.departSpeed);
            break;
        case DEPART_SPEED_RANDOM:
            val = "random";
            break;
        case DEPART_SPEED_MAX:
            val = "max";
            break;
        case DEPART_SPEED_DEFAULT:
        default:
            cerr << "should not happen..." << endl;
            break;
        }
        dev << " departspeed=\"" << val << "\"";
    }
    // color
    if (myParameter.wasSet(VEHPARS_COLOR_SET)) {
        dev << " color=\"" << myParameter.color << "\"";
    }
    // repetition values
    if (myParameter.wasSet(VEHPARS_PERIODNUM_SET)) {
        dev << " repno=\"" << myParameter.repetitionNumber << "\"";
    }
    if (myParameter.wasSet(VEHPARS_PERIODFREQ_SET)) {
        dev << " period=\"" << myParameter.repetitionOffset << "\"";
    }
    dev << ">\n";
}


void
ROVehicle::saveAllAsXML(OutputDevice &os,
                        OutputDevice * const altos, bool withExitTimes) const throw(IOError)
{
    // check whether the vehicle's type was saved before
    if (myType!=0&&!myType->isSaved()) {
        // ... save if not
        myType->writeXMLDefinition(os);
        if (altos!=0) {
            myType->writeXMLDefinition(*altos);
        }
        myType->markSaved();
    }

    // write the vehicle (new style, with included routes)
    os << "   ";
    saveXMLVehicle(os);
    if (altos!=0) {
        (*altos) << "   ";
        saveXMLVehicle(*altos);
    }

    // check whether the route shall be saved
    if (!myRoute->isSaved()) {
        os << "      ";
        myRoute->writeXMLDefinition(os, this, false, withExitTimes);
        if (altos!=0) {
            (*altos) << "      ";
            myRoute->writeXMLDefinition(*altos, this, true, withExitTimes);
        }
    }
    os << "   </vehicle>\n";
    if (altos!=0) {
        (*altos) << "   </vehicle>\n";
    }
}


ROVehicle *
ROVehicle::copy(const std::string &id, unsigned int depTime,
                RORouteDef *newRoute) throw()
{
    SUMOVehicleParameter pars(myParameter);
    pars.id = id;
    pars.depart = depTime;
    return new ROVehicle(pars, newRoute, myType);
}


/****************************************************************************/


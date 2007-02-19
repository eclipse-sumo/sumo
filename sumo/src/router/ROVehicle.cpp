/****************************************************************************/
/// @file    ROVehicle.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A single vehicle
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <string>
#include <iostream>
#include "ROVehicleType.h"
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "RORouteDef_Alternatives.h"
#include "RORoute.h"

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
ROVehicle::ROVehicle(ROVehicleBuilder &,
                     const std::string &id, RORouteDef *route,
                     unsigned int depart, ROVehicleType *type,
                     const RGBColor &color,
                     int period, int repNo)
        : myID(id), myColor(color), myType(type), myRoute(route),
        myDepartTime(depart),
        myRepetitionPeriod(period), myRepetitionNumber(repNo)
{}

ROVehicle::~ROVehicle()
{}


RORouteDef * const
ROVehicle::getRoute() const
{
    return myRoute;
}


const ROVehicleType * const
ROVehicle::getType() const
{
    return myType;
}


const std::string&
ROVehicle::getID() const
{
    return myID;
}

SUMOTime
ROVehicle::getDepartureTime() const
{
    return myDepartTime;
}


bool
ROVehicle::periodical() const
{
    return myRepetitionPeriod!=-1;
}


void
ROVehicle::saveXMLVehicle(std::ostream * const os) const
{
    (*os) << "<vehicle id=\"" << myID << "\"";
    (*os) << " type=\"" << myType->getID() << "\"";
    (*os) << " depart=\"" << myDepartTime << "\"";
    if (myColor!=RGBColor(-1,-1,-1)) {
        (*os) << " color=\"" << myColor << "\"";
    }
    if (myRepetitionPeriod!=-1) {
        (*os) << " period=\"" << myRepetitionPeriod << "\"";
        (*os) << " repno=\"" << myRepetitionNumber << "\"";
    }
    (*os) << ">" << endl;
}

void
ROVehicle::saveAllAsXML(std::ostream * const os,
                        std::ostream * const altos,
                        ROVehicleType &defType,
                        const RORouteDef * const route) const
{
    // get the reference vehicle's type
    ROVehicleType &type = getTypeForSaving(defType);
    // check whether it was saved before
    if (!type.isSaved()) {
        // ... save if not
        type.xmlOut(*os);
        if (altos!=0) {
            type.xmlOut(*altos);
        }
        type.markSaved();
    }

    // write the vehicle (new style, with included routes)
    (*os) << "   ";
    saveXMLVehicle(os);
    if (altos!=0) {
        (*altos) << "   ";
        saveXMLVehicle(altos);
    }

    // check whether the route shall be saved
    if (!route->isSaved()) {
        // write the route
        const ROEdgeVector &routee = route->getCurrentEdgeVector();
        (*os) << "      <route";
        if (periodical()) {
            // remark for further usage if referenced by more than one vehicle
            (*os) << " multi_ref=\"x\"";
        }
        const RGBColor &c = route->getColor();
        if (c!=RGBColor(-1,-1,-1)) {
            (*os) << " color=\"" << c << "\"";
        }
        (*os) << ">" << routee << "</route>" << endl;
        //route->xmlOutCurrent(*os, periodical());
        // check whether the alternatives shall be written
        if (altos!=0) {
            (*altos) << "      <routealt last=\"" << myRoute->getLastUsedIndex() << "\"";
            if (c!=RGBColor(-1,-1,-1)) {
                (*altos) << " color=\"" << c << "\"";
            }
            (*altos) << ">" << endl;
            if (myRoute->getAlternativesSize()!=1) {
                // ok, we have here a RORouteDef_Alternatives
                for (size_t i=0; i!=myRoute->getAlternativesSize(); i++) {
                    const RORoute &alt =
                        static_cast<RORouteDef_Alternatives*>(myRoute)->getAlternative(i);//_alternatives[i];
                    (*altos) << "         <route cost=\"" << alt.getCosts();
                    (*altos) << "\" probability=\"" << alt.getProbability();
                    (*altos) << "\">";
                    alt.xmlOutEdges((*altos));
                    (*altos) << "</route>" << endl;
                }
            } else {
                // ok, only one alternative; let's write it plain
                (*altos) << "         <route cost=\"" << routee.recomputeCosts(this, getDepartureTime());
                (*altos) << "\" probability=\"1";
                (*altos) << "\">" << routee << "</route>" << endl;
            }
            (*altos) << "      </routealt>" << endl;
        }
    }

    (*os) << "   </vehicle>" << endl;
    if (altos!=0) {
        (*altos) << "   </vehicle>" << endl;
    }
}


ROVehicleType &
ROVehicle::getTypeForSaving(ROVehicleType &defType) const
{
    if (myType==0) {
        WRITE_WARNING("The vehicle '" + getID() + "' has no valid type; Using default.");
        return defType;
    } else {
        return *myType;
    }
}


ROVehicle *
ROVehicle::copy(ROVehicleBuilder &vb,
                const std::string &id, unsigned int depTime,
                RORouteDef *newRoute)
{
    return new ROVehicle(vb, id, newRoute, depTime, myType, myColor,
                         myRepetitionPeriod, myRepetitionNumber);
}



/****************************************************************************/


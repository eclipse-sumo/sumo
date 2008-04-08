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
ROVehicle::ROVehicle(ROVehicleBuilder &,
                     const SUMOVehicleParameter &pars, 
                     RORouteDef *route, ROVehicleType *type)
        : myParameter(pars), myType(type), myRoute(route)
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
    return myParameter.id;
}

SUMOTime
ROVehicle::getDepartureTime() const
{
    return myParameter.depart;
}


void
ROVehicle::saveXMLVehicle(OutputDevice &dev) const
{
    dev << "<vehicle id=\"" << myParameter.id << "\"";
    if (myType!=0) {
        dev << " type=\"" << myType->getID() << "\"";
    }
    dev << " depart=\"" << myParameter.depart << "\"";
    if ((myParameter.setParameter&VEHPARS_COLOR_SET)!=0) {
        dev << " color=\"" << myParameter.color << "\"";
    }
    if ((myParameter.setParameter&VEHPARS_PERIODNUM_SET)!=0) {
        dev << " repno=\"" << myParameter.repetitionNumber << "\"";
    }
    if ((myParameter.setParameter&VEHPARS_PERIODFREQ_SET)!=0) {
        dev << " period=\"" << myParameter.repetitionOffset << "\"";
    }
    dev << ">\n";
}


void
ROVehicle::saveAllAsXML(OutputDevice &os,
                        OutputDevice * const altos,
                        const RORouteDef * const route) const
{
    // check whether the vehicle's type was saved before
    if (myType!=0&&!myType->isSaved()) {
        // ... save if not
        myType->xmlOut(os);
        if (altos!=0) {
            myType->xmlOut(*altos);
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
    if (!route->isSaved()) {
        // write the route
        const std::vector<const ROEdge*> &routee = route->getCurrentEdgeVector();
        os << "      <route";
        const RGBColor &c = route->getColor();
        if (c!=RGBColor()) {
            os << " color=\"" << c << "\"";
        }
        os << ">" << routee << "</route>\n";
        // check whether the alternatives shall be written
        if (altos!=0) {
            (*altos) << "      <routealt last=\"" << myRoute->getLastUsedIndex() << "\"";
            if (c!=RGBColor()) {
                (*altos) << " color=\"" << c << "\"";
            }
            (*altos) << ">\n";
            if (myRoute->getAlternativesSize()!=1) {
                // ok, we have here a RORouteDef_Alternatives
                for (size_t i=0; i!=myRoute->getAlternativesSize(); i++) {
                    const RORoute &alt =
                        static_cast<RORouteDef_Alternatives*>(myRoute)->getAlternative(i);//myAlternatives[i];
                    (*altos) << "         <route cost=\"" << alt.getCosts();
                    (*altos) << "\" probability=\"" << alt.getProbability();
                    (*altos) << "\">";
                    alt.xmlOutEdges((*altos));
                    (*altos) << "</route>\n";
                }
            } else {
                // ok, only one alternative; let's write it plain
                (*altos) << "         <route cost=\"" << ROHelper::recomputeCosts(routee, this, getDepartureTime());
                (*altos) << "\" probability=\"1";
                (*altos) << "\">" << routee << "</route>\n";
            }
            (*altos) << "      </routealt>\n";
        }
    }

    os << "   </vehicle>\n";
    if (altos!=0) {
        (*altos) << "   </vehicle>\n";
    }
}


ROVehicle *
ROVehicle::copy(ROVehicleBuilder &vb,
                const std::string &id, unsigned int depTime,
                RORouteDef *newRoute)
{
    SUMOVehicleParameter pars(myParameter);
    pars.id = id;
    pars.depart = depTime;
    return new ROVehicle(vb, pars, newRoute, myType);
}


/****************************************************************************/


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
                     RORouteDef *route, ROVehicleType *type) throw()
        : myParameter(pars), myType(type), myRoute(route)
{}


ROVehicle::~ROVehicle() throw()
{}


void
ROVehicle::saveXMLVehicle(OutputDevice &dev) const throw(IOError)
{
    dev << "<vehicle id=\"" << myParameter.id << "\"";
    if (myType!=0) {
        dev << " type=\"" << myType->getID() << "\"";
    }
    dev << " depart=\"" << myParameter.depart << "\"";

    // optional parameter
        // depart-values
            // departlane
    if ((myParameter.setParameter&VEHPARS_DEPARTLANE_SET)!=0) {
        string val;
        switch(myParameter.departLaneProcedure) {
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
    if ((myParameter.setParameter&VEHPARS_DEPARTPOS_SET)!=0) {
        string val;
        switch(myParameter.departPosProcedure) {
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
    if ((myParameter.setParameter&VEHPARS_DEPARTSPEED_SET)!=0) {
        string val;
        switch(myParameter.departSpeedProcedure) {
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
    if ((myParameter.setParameter&VEHPARS_COLOR_SET)!=0) {
        dev << " color=\"" << myParameter.color << "\"";
    }
        // repetition values
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
                        const RORouteDef * const route) const throw(IOError)
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
                RORouteDef *newRoute) throw()
{
    SUMOVehicleParameter pars(myParameter);
    pars.id = id;
    pars.depart = depTime;
    return new ROVehicle(vb, pars, newRoute, myType);
}


/****************************************************************************/


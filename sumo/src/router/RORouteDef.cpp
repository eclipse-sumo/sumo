//---------------------------------------------------------------------------//
//                        RORouteDef.cpp -
//  Basic class for route definitions (not the computed routes)
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.12  2003/12/09 11:30:37  dkrajzew
// false detection whether a vehicle is too long to start from an edge patched
//
// Revision 1.11  2003/11/11 08:04:46  dkrajzew
// avoiding emissions of vehicles on too short edges
//
// Revision 1.10  2003/09/05 15:23:23  dkrajzew
// umlaute conversion added
//
// Revision 1.9  2003/08/21 12:59:35  dkrajzew
// some bugs patched
//
// Revision 1.8  2003/07/30 09:26:33  dkrajzew
// all vehicles, routes and vehicle types may now have specific colors
//
// Revision 1.7  2003/06/18 11:20:54  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.6  2003/05/20 09:48:35  dkrajzew
// debugging
//
// Revision 1.5  2003/03/20 16:39:17  dkrajzew
// periodical car emission implemented; windows eol removed
//
// Revision 1.4  2003/03/17 14:25:28  dkrajzew
// windows eol removed
//
// Revision 1.3  2003/03/03 15:22:34  dkrajzew
// debugging
//
// Revision 1.2  2003/02/07 10:45:06  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/convert/TplConvert.h>
#include <utils/convert/ToString.h>
#include <utils/common/Named.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include "ROEdge.h"
#include "RORoute.h"
#include "RORouter.h"
#include "ReferencedItem.h"
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "ROVehicleType.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
RORouteDef::RORouteDef(const std::string &id, const RGBColor &color)
    : ReferencedItem(), Named(StringUtils::convertUmlaute(id)),
    myColor(color)
{
}

RORouteDef::~RORouteDef()
{
}


bool
RORouteDef::computeAndSave(OptionsCont &options,
                           RORouter &router, long begin,
                           std::ostream &res, std::ostream &altres,
                           bool isPeriodical, ROVehicle &veh)
{
    RORoute *current =
        buildCurrentRoute(router, begin,
            options.getBool("continue-on-unbuild"), veh);
    if(current==0) {
        return false;
    }
    // check whether the route is valid and does not end on the starting edge
    if(current->size()<2) {
        // check whether the route ends at the starting edge
        //  unbuild routes due to missing connections are reported within the
        //  router
        if(current->size()!=0) {
            cout << endl;
            MsgHandler::getWarningInstance()->inform(
                string("The route '") + _id
                + string("' is too short, propably ending at the starting edge."));
            MsgHandler::getWarningInstance()->inform("Skipping...");
        }
        delete current;
        return false;
    }
    // check whether the vehicle is able to start at this edge
    //  (the edge must be longer than the vehicle)
    if(current->getFirst()->getLength()<=veh.getType()->getLength()) {
        MsgHandler::getErrorInstance()->inform(string("The vehicle '") + veh.getID()
            + string("' is too long to start at edge '")
            + current->getFirst()->getID() + string("'."));
        delete current;
        return false;
    }
    // add build route
    addAlternative(current, begin);
    // save route
    xmlOutCurrent(res, isPeriodical);
    xmlOutAlternatives(altres);
    return true;
}


void
RORouteDef::patchID()
{
    // patch the name
    size_t idx = _id.rfind('_');
    if(idx!=string::npos) {
        try {
            int no = TplConvert<char>::_2int(_id.substr(idx+1).c_str());
            _id = _id.substr(0, idx+1) + toString<int>(no+1);
        } catch (NumberFormatException) {
            _id = _id + "_0";
        }
    } else {
        _id = _id + "_0";
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "RORouteDef.icc"
//#endif

// Local Variables:
// mode:C++
// End:



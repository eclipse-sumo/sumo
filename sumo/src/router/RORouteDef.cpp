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
// Revision 1.18  2006/01/26 08:44:14  dkrajzew
// adapted the new router API
//
// Revision 1.17  2006/01/09 12:00:59  dkrajzew
// debugging vehicle color usage
//
// Revision 1.16  2005/10/07 11:42:15  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.15  2005/09/23 06:04:36  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.14  2005/09/15 12:05:11  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.13  2004/01/26 08:01:21  dkrajzew
// loaders and route-def types are now renamed in an senseful way;
//  further changes in order to make both new routers work;
//  documentation added
//
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
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/Named.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include "ROEdge.h"
#include "RORoute.h"
#include "ROAbstractRouter.h"
#include "ReferencedItem.h"
#include "RORouteDef.h"
#include "ROVehicle.h"
#include "ROVehicleType.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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


int
RORouteDef::getLastUsedIndex() const
{
    return 0;
}


size_t
RORouteDef::getAlternativesSize() const
{
    return 1;
}


const RGBColor &
RORouteDef::getColor() const
{
    return myColor;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



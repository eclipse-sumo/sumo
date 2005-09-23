//---------------------------------------------------------------------------//
//                        ODDistrict.cpp -
//  A district
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
// Revision 1.10  2005/09/23 06:04:23  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 12:04:48  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/05/04 08:44:57  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2003/10/31 08:01:13  dkrajzew
// hope to have patched false usage of RAND_MAX when using gcc
//
// Revision 1.6  2003/10/15 11:53:14  dkrajzew
// false usage of rand() patched
//
// Revision 1.5  2003/08/04 11:37:37  dkrajzew
// added the generation of colors from districts
//
// Revision 1.4  2003/06/18 11:20:24  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.3  2003/05/20 09:46:53  dkrajzew
// usage of split and non-split od-matrices from visum and vissim rechecked
//
// Revision 1.2  2003/02/07 10:44:19  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H
#include <vector>
#include <string>
#include <utility>
#include <utils/common/UtilExceptions.h>
#include <utils/common/Named.h>
#include <utils/common/MsgHandler.h>
#include "ODDistrict.h"

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
ODDistrict::ODDistrict(const std::string &id)
    : Named(id)
{
}

ODDistrict::~ODDistrict()
{
}


void
ODDistrict::addSource(const std::string &id, SUMOReal weight)
{
    _sources.push_back(std::pair<std::string, SUMOReal>(id, weight));
}


void
ODDistrict::addSink(const std::string &id, SUMOReal weight)
{
    _sinks.push_back(std::pair<std::string, SUMOReal>(id, weight));
}


std::string
ODDistrict::getRandomSource() const
{
    return getRandom(_sources);
}


std::string
ODDistrict::getRandomSink() const
{
    return getRandom(_sinks);
}


std::string
ODDistrict::getRandom(const WeightedEdgeIDCont &cont) const
{
    // check whether at least one elements exists
    if(cont.size()==0) {
        MsgHandler::getErrorInstance()->inform(
            string("Trying to use a not existing source/sink at district: ")
            + string(getID()));
        throw 1;//ProcessError();
    }
    // compute which item to retrieve
    SUMOReal val = rand() /
        ( static_cast<SUMOReal>(RAND_MAX) + 1);
    // go through the list to retrieve the item
    for(WeightedEdgeIDCont::const_iterator i=cont.begin(); i!=cont.end(); i++) {
        val -= (*i).second;
        if(val<=0) {
            return (*i).first;
        }
    }
    // return the last item when no other was found
    //  !!! maybe a warning should be printed
    return (*(cont.end()-1)).first;
}


void
ODDistrict::setColor(SUMOReal val)
{
    myColor = val;
}


SUMOReal
ODDistrict::getColor()  const
{
    return myColor;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



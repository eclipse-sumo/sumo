//---------------------------------------------------------------------------//
//                        ODDistrictCont.cpp -
//  The container for districts
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
// Revision 1.4  2003/08/04 11:37:37  dkrajzew
// added the generation of colors from districts
//
// Revision 1.3  2003/02/07 10:44:19  dkrajzew
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
#include <utils/common/UtilExceptions.h>
#include "ODDistrict.h"
#include <utils/common/NamedObjectCont.h>
#include "ODDistrictCont.h"

using namespace std;

ODDistrictCont::ODDistrictCont()
    : myHadColorized(false)
{
}


ODDistrictCont::~ODDistrictCont()
{
}


std::string
ODDistrictCont::getRandomSourceFromDistrict(const std::string &name) const
{
    ODDistrict *district = get(name);
    if(district==0) {
        throw InvalidArgument(
            string("There is no district '") + name + string("'."));
    }
    return district->getRandomSource();
}


std::string
ODDistrictCont::getRandomSinkFromDistrict(const std::string &name) const
{
    ODDistrict *district = get(name);
    if(district==0) {
        throw InvalidArgument(
            string("There is no district '") + name + string("'."));
    }
    return district->getRandomSink();
}


void
ODDistrictCont::colorize()
{
    size_t pos = 0;
    for(myContIt i=_cont.begin(); i!=_cont.end(); i++) {
        ODDistrict *district = (*i).second;
        district->setColor((double) pos / (double) _cont.size());
        pos++;
    }
}

double
ODDistrictCont::getDistrictColor(const std::string &name) const
{
    ODDistrict *district = get(name);
    if(district==0) {
        throw InvalidArgument(
            string("There is no district '") + name + string("'."));
    }
    return district->getColor();
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ODDistrictCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:



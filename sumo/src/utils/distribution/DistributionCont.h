#ifndef DistributionCont_h
#define DistributionCont_h
//---------------------------------------------------------------------------//
//                        DistributionCont.h -
//  A container for distributions
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
// $Log$
// Revision 1.5  2005/04/28 09:02:48  dkrajzew
// level3 warnings removed
//
// Revision 1.4  2004/01/28 12:35:37  dkrajzew
// documentation added
//
// Revision 1.3  2003/06/06 11:01:09  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/06/05 14:33:44  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <map>
#include <utils/distribution/Distribution.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class DistributionCont
 * A container for distributions of different type.
 */
class DistributionCont {
public:
    /// Adds a distribution of the given type and name to the container
    static bool dictionary(const std::string &type, const std::string &id,
        Distribution *d);

    /// retrieves the distribution described by a type and a name from the container
    static Distribution *dictionary(const std::string &type,
        const std::string &id);

private:
    /// Definition of a map from distribution ids to distributions
    typedef std::map<std::string, Distribution*> DistDict;

    /// Definition of a map from distribution types to distribution ids to distributions
    typedef std::map<std::string, DistDict> TypedDistDict;

    /// Map from distribution types to distribution ids to distributions
    static TypedDistDict myDict;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


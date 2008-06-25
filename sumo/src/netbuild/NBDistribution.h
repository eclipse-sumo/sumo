/****************************************************************************/
/// @file    NBDistribution.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for statistical distribution descriptions
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
#ifndef NBDistribution_h
#define NBDistribution_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <utils/distribution/Distribution.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBDistribution
 * @brief The base class for statistical distribution descriptions
 */
class NBDistribution
{
public:
    static bool dictionary(const std::string &type, const std::string &id,
                           Distribution *d);
    static Distribution *dictionary(const std::string &type,
                                    const std::string &id);
    static void clear();
private:
    typedef std::map<std::string, Distribution*> DistDict;
    typedef std::map<std::string, DistDict> TypedDistDict;
    static TypedDistDict myDict;
};


#endif

/****************************************************************************/


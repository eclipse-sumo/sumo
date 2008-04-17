/****************************************************************************/
/// @file    NINavTeqHelper.h
/// @author  Daniel Krajzewicz
/// @date    Jul 2006
/// @version $Id$
///
// Some parser methods shared around several formats containing NavTeq-Nets
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
#ifndef NINavTeqHelper_h
#define NINavTeqHelper_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NINavTeqHelper
 * @brief Some parser methods shared around several formats containing NavTeq-Nets
 */
class NINavTeqHelper
{
public:
    /// Returns the speed evaluating the given Navteq-description
    static SUMOReal getSpeed(const std::string &id,
                             const std::string &speedClassS);

    /// Returns the lane number evaluating the given Navteq-description
    static size_t getLaneNumber(const std::string &id,
                                const std::string &laneNoS, SUMOReal speed);

    /// Adds vehicle classes parsing the given list of allowed vehicles
    static void addVehicleClasses(NBEdge &e,
                                  const std::string &classS);

protected:
    /// Adds a single vehicle class to all lanes of the given edge
    static void addVehicleClass(NBEdge &e, SUMOVehicleClass c);

};


#endif

/****************************************************************************/


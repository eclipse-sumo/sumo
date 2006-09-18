#ifndef NINavTeqHelper_h
#define NINavTeqHelper_h
/***************************************************************************
                          NINavTeqHelper.h
    Some parser methods shared around several formats containing NavTeq-Nets
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Jul 2006
    copyright            : (C) 2006 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.2  2006/09/18 10:12:57  dkrajzew
// added import of vclasses
//
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
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOVehicleClass.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBEdge;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NINavTeqHelper
 * @brief Some parser methods shared around several formats containing NavTeq-Nets
 */
class NINavTeqHelper {
public:
    /// Returns the speed evaluating the given Navteq-description
	static SUMOReal getSpeed(const std::string &id,
        const std::string &speedClassS);

    /// Returns the lane number evaluating the given Navteq-description
    static size_t getLaneNumber(const std::string &id,
		const std::string &laneNoS, SUMOReal speed, bool useNewLaneNumberInfoPlain);

    /// Adds vehicle classes parsing the given list of allowed vehicles
    static void addVehicleClasses(NBEdge &e,
        const std::string &classS);

protected:
    /// Adds a single vehicle class to all lanes of the given edge
    static void addVehicleClass(NBEdge &e, SUMOVehicleClass c);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

/****************************************************************************/
/// @file    PCLoaderVisum.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 02.11.2006
/// @version $Id: PCLoaderVisum.h 4389 2007-08-28 10:21:00Z behrisch $
///
// A reader of pois and polygons stored in VISUM-format
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
#ifndef PCLoaderVisum_h
#define PCLoaderVisum_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "PCPolyContainer.h"
#include "PCTypeMap.h"


// ===========================================================================
// class definitions
// ===========================================================================
class OptionsCont;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class PCLoaderVisum
 * @brief A reader of pois and polygons stored in VISUM-format
 */
class PCLoaderVisum
{
public:
    /** @brief Loads pois/polygons assumed to be stored using VISUM-format
     *
     * If the option "visum-files" is set within the given options container,
     *  the files stored herein are parsed using "load", assuming this
     *  option contains file paths to files containing pois and polygons stored 
     *  in VISUM ".net"-format.
     *
     * @param[in] oc The options container to get further options from
     * @param[in] toFill The poly/pois container to add loaded polys/pois to
     * @param[in] tm The type map to use for setting values of loaded polys/pois
     * @exception ProcessError if something fails
     */
    static void loadIfSet(OptionsCont &oc, PCPolyContainer &toFill,
        PCTypeMap &tm) throw(ProcessError);


protected:
    /** @brief Parses pois/polys stored within the given file
     * @param[in] oc The options container to get further options from
     * @param[in] toFill The poly/pois container to add loaded polys/pois to
     * @param[in] tm The type map to use for setting values of loaded polys/pois
     * @exception ProcessError if something fails
     */
    static void load(const std::string &file, OptionsCont &oc, PCPolyContainer &toFill,
        PCTypeMap &tm) throw(ProcessError);


};


#endif

/****************************************************************************/


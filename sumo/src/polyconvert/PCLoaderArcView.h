/****************************************************************************/
/// @file    PCLoaderArcView.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A reader of pois and polygons from shape files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef PCLoaderArcView_h
#define PCLoaderArcView_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class PCPolyContainer;
class PCTypeMap;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PCLoaderArcView
 * @brief A reader of pois and polygons from shape files
 *
 * The current importer works only if SUMO was compiled with GDAL-support.
 *  If not, an error message is generated.
 *
 * @todo reinsert import via shapelib
 */
class PCLoaderArcView
{
public:
    /** @brief Loads pois/polygons assumed to be stored as shape files-files
     *
     * If the option "shape-files" is set within the given options container,
     *  the files stored herein are parsed using "load", assuming this
     *  option contains file paths to files containing pois and polygons stored
     *  as shape-files.
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


private:
    /// @brief Invalidated copy constructor.
    PCLoaderArcView(const PCLoaderArcView&);

    /// @brief Invalidated assignment operator.
    PCLoaderArcView& operator=(const PCLoaderArcView&);

};


#endif

/****************************************************************************/


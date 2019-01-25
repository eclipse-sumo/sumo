/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    PCLoaderVisum.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Thu, 02.11.2006
/// @version $Id$
///
// A reader of pois and polygons stored in VISUM-format
/****************************************************************************/
#ifndef PCLoaderVisum_h
#define PCLoaderVisum_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
class PCLoaderVisum {
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
    static void loadIfSet(OptionsCont& oc, PCPolyContainer& toFill,
                          PCTypeMap& tm);


protected:
    /** @brief Parses pois/polys stored within the given file
     * @param[in] oc The options container to get further options from
     * @param[in] toFill The poly/pois container to add loaded polys/pois to
     * @param[in] tm The type map to use for setting values of loaded polys/pois
     * @exception ProcessError if something fails
     */
    static void load(const std::string& file, OptionsCont& oc, PCPolyContainer& toFill,
                     PCTypeMap& tm);


};


#endif

/****************************************************************************/


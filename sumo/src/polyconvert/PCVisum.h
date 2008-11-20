/****************************************************************************/
/// @file    PCVisum.h
/// @author  Danilot Teta Boyom
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// A reader of polygons stored in visum-format
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
#ifndef PCVisum_h
#define PCVisum_h


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
class NBNetBuilder;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class PCVisum
 * @brief A reader of polygons stored in visum-format
 */
class PCVisum
{
public:
    PCVisum(PCPolyContainer &toFill,
            PCTypeMap &tm);

    ~PCVisum();

    /** loads data from the files specified in the given option container */
    void load(std::string opt, std::string file);

    /** loads data from visum-input-file */
    void load(OptionsCont &oc);


protected:
    /// Container to store the read polygons into
    PCPolyContainer &myCont;

    /// The type map to use
    PCTypeMap &myTypeMap;

};


#endif

/****************************************************************************/


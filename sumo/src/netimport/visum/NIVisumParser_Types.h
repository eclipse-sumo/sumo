/****************************************************************************/
/// @file    NIVisumParser_Types.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id$
///
// Parser for road types
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
#ifndef NIVisumParser_Types_h
#define NIVisumParser_Types_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "NIVisumLoader.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBCapacity2Lanes;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVisumParser_Types
 * This class parses edge types from visum-files.
 */
class NIVisumParser_Types :
            public NIVisumLoader::NIVisumSingleDataTypeParser
{
public:
    /// Constructor
    NIVisumParser_Types(
        NIVisumLoader &parent, NBTypeCont &tc,
        const std::string &dataName,
        NBCapacity2Lanes &cap2lanes);

    /// Destructor
    ~NIVisumParser_Types();

protected:
    /** @brief Parses a single edge type using data from the inherited NamedColumnsParser. */
    void myDependentReport();

private:
    /// Transfer function from the edge's capacity to his lane number
    NBCapacity2Lanes &myCap2Lanes;

    NBTypeCont &myTypeCont;

};


#endif

/****************************************************************************/


/****************************************************************************/
/// @file    NIVisumParser_Districts.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id$
///
// Parser for visum-districts
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
#ifndef NIVisumParser_Districts_h
#define NIVisumParser_Districts_h


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
// class definitions
// ===========================================================================
/**
 * @class NIVisumParser_Districts
 * Within visum, districts are used as the origin and the end point of trips.
 * This class parses districts from visum-files into a SUMO-XML format.
 */
class NIVisumParser_Districts :
            public NIVisumLoader::NIVisumSingleDataTypeParser
{
public:
    /// Constructor
    NIVisumParser_Districts(NIVisumLoader &parent,
                            NBDistrictCont &dc, const std::string &dataName);

    /// Destructor
    ~NIVisumParser_Districts();

protected:
    /** @brief Parses a single district using data from the inherited NamedColumnsParser. */
    void myDependentReport();

protected:
    NBDistrictCont &myDistrictCont;

};


#endif

/****************************************************************************/


/****************************************************************************/
/// @file    NIVisumParser_VSysTypes.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 14 Nov 2002
/// @version $Id$
///
// Parser for modality types (private/public)
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
#ifndef NIVisumParser_VSysTypes_h
#define NIVisumParser_VSysTypes_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "NIVisumLoader.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NIVisumParser_VSysTypes
 * Visum differs between private and public transport. As their names are
 * used within later processing, they must be parsed, too.
 */
class NIVisumParser_VSysTypes :
            public NIVisumLoader::NIVisumSingleDataTypeParser
{
public:
    /// Constructor
    NIVisumParser_VSysTypes(NIVisumLoader &parent,
                            const std::string &dataName,
                            NIVisumLoader::VSysTypeNames &vsystypes);

    /// Destructor
    ~NIVisumParser_VSysTypes();

protected:
    /** @brief Parses a single modality type name using data from the inherited NamedColumnsParser. */
    void myDependentReport();

private:
    /// a map of VSysTypes to the traffic type they represent
    NIVisumLoader::VSysTypeNames &usedVSysTypes;

};


#endif

/****************************************************************************/


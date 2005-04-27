#ifndef NIVisumParser_VSysTypes_h
#define NIVisumParser_VSysTypes_h
/***************************************************************************
                          NIVisumParser_VSysTypes.h
			  Parser for modality types (private/public)
                             -------------------
    project              : SUMO
    begin                : Thu, 14 Nov 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.2  2005/04/27 12:24:42  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NIVisumLoader.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVisumParser_VSysTypes
 * Visum differs between private and public transport. As their names are
 * used within later processing, they must be parsed, too.
 */
class NIVisumParser_VSysTypes :
        public NIVisumLoader::NIVisumSingleDataTypeParser {
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

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

#ifndef NIVisumParser_Types_h
#define NIVisumParser_Types_h
/***************************************************************************
                          NIVisumParser_Types.h
			  Parser for road types
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
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NIVisumLoader.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBCapacity2Lanes;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVisumParser_Types
 * This class parses edge types from visum-files.
 */
class NIVisumParser_Types :
        public NIVisumLoader::NIVisumSingleDataTypeParser {
public:
    /// Constructor
    NIVisumParser_Types(
        NIVisumLoader &parent,
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

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIVisumParser_Types.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

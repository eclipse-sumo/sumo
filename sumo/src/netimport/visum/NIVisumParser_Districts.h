#ifndef NIVisumParser_Districts_h
#define NIVisumParser_Districts_h
/***************************************************************************
                          NIVisumParser_Districts.h
			  Parser for visum-districts
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
 * class definitions
 * ======================================================================= */
/**
 * @class NIVisumParser_Districts
 * Within visum, districts are used as the origin and the end point of trips.
 * This class parses districts from visum-files into a SUMO-XML format.
 */
class NIVisumParser_Districts :
        public NIVisumLoader::NIVisumSingleDataTypeParser {
public:
    /// Constructor
    NIVisumParser_Districts(NIVisumLoader &parent,
        const std::string &dataName);

    /// Destructor
    ~NIVisumParser_Districts();

protected:
    /** @brief Parses a single district using data from the inherited NamedColumnsParser. */
    void myDependentReport();

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIVisumParser_Districts.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


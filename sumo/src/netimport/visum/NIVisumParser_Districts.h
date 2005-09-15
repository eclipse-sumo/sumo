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
// Revision 1.3  2005/09/15 12:03:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/04/27 12:24:41  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.1  2003/02/07 11:14:54  dkrajzew
// updated
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

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
        NBDistrictCont &dc, const std::string &dataName);

    /// Destructor
    ~NIVisumParser_Districts();

protected:
    /** @brief Parses a single district using data from the inherited NamedColumnsParser. */
    void myDependentReport();

protected:
	NBDistrictCont &myDistrictCont;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:


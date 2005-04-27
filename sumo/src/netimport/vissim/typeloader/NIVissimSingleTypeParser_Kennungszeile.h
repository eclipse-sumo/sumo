#ifndef NIVissimSingleTypeParser_Kennungszeile_h
#define NIVissimSingleTypeParser_Kennungszeile_h
/***************************************************************************
                          NIVissimSingleTypeParser_Kennungszeile.h

                             -------------------
    begin                : Thu, 6 Mar 2003
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.2  2005/04/27 12:24:38  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.1  2003/03/06 17:12:55  dkrajzew
// further data parsing
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include "../NIVissimLoader.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVissimSingleTypeParser_Kennungszeile
 *
 */
class NIVissimSingleTypeParser_Kennungszeile :
        public NIVissimLoader::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Kennungszeile(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Kennungszeile();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

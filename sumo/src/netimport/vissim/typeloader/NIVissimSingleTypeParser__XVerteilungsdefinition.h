#ifndef NIVissimSingleTypeParser__XVerteilungsdefinition_h
#define NIVissimSingleTypeParser__XVerteilungsdefinition_h
/***************************************************************************
    NIVissimSingleTypeParser__XVerteilungsdefinition.h

                             -------------------
    begin                : Wed, 30 Apr 2003
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
// Revision 1.2  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.1  2003/05/20 09:42:38  dkrajzew
// all data types implemented
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
 * @class NIVissimSingleTypeParser__XVerteilungsdefinition
 *
 */
class NIVissimSingleTypeParser__XVerteilungsdefinition :
        public NIVissimLoader::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser__XVerteilungsdefinition(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser__XVerteilungsdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

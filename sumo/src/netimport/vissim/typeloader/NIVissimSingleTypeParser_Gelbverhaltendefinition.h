#ifndef NIVissimSingleTypeParser_Gelbverhaltendefinition_h
#define NIVissimSingleTypeParser_Gelbverhaltendefinition_h
/***************************************************************************
    NIVissimSingleTypeParser_Gelbverhaltendefinition.h

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
// Revision 1.1  2003/05/20 09:42:37  dkrajzew
// all data types implemented
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include "../NIVissimLoader.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NIVissimSingleTypeParser_Gelbverhaltendefinition
 *
 */
class NIVissimSingleTypeParser_Gelbverhaltendefinition :
        public NIVissimLoader::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Gelbverhaltendefinition(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Gelbverhaltendefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimSingleTypeParser_Gelbverhaltendefinition.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

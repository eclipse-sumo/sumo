#ifndef NIVissimSingleTypeParser_Fensterdefinition_h
#define NIVissimSingleTypeParser_Fensterdefinition_h
/***************************************************************************
                          NIVissimSingleTypeParser_Fensterdefinition.h

                             -------------------
    begin                : Fri, 21 Mar 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.1  2003/03/26 12:17:14  dkrajzew
// further debugging/improvements of Vissim-import
//
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
 * @class NIVissimSingleTypeParser_Fensterdefinition
 *
 */
class NIVissimSingleTypeParser_Fensterdefinition :
        public NIVissimLoader::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Fensterdefinition(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Fensterdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimSingleTypeParser_Fensterdefinition.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

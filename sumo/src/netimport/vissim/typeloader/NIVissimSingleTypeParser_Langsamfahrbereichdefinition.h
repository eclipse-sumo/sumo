#ifndef NIVissimSingleTypeParser_Langsamfahrbereichdefinition_h
#define NIVissimSingleTypeParser_Langsamfahrbereichdefinition_h
/***************************************************************************
                          NIVissimSingleTypeParser_Langsamfahrbereichdefinition.h

                             -------------------
    begin                : Wed, 18 Dec 2002
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
// Revision 1.1  2003/02/07 11:08:42  dkrajzew
// Vissim import added (preview)
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
 * @class NIVissimSingleTypeParser_Langsamfahrbereichdefinition
 *
 */
class NIVissimSingleTypeParser_Langsamfahrbereichdefinition :
        public NIVissimLoader::VissimSingleTypeParser {
public:
    /// Constructor
    NIVissimSingleTypeParser_Langsamfahrbereichdefinition(NIVissimLoader &parent);

    /// Destructor
    ~NIVissimSingleTypeParser_Langsamfahrbereichdefinition();

    /// Parses the data type from the given stream
    bool parse(std::istream &from);

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "NIVissimSingleTypeParser_Langsamfahrbereichdefinition.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

#ifndef ODMatrix_h
#define ODMatrix_h
//---------------------------------------------------------------------------//
//                        ODmatrix.h -
//  some matrix usage functions
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.11  2005/09/23 06:04:23  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.10  2005/09/15 12:04:48  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/05/04 08:44:57  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.8  2004/07/02 09:38:21  dkrajzew
// coding style adaptations
//
// Revision 1.7  2003/08/21 12:57:59  dkrajzew
// buffer overflow bug#1 removed
//
// Revision 1.6  2003/08/04 11:37:37  dkrajzew
// added the generation of colors from districts
//
// Revision 1.5  2003/05/20 09:46:53  dkrajzew
// usage of split and non-split od-matrices from visum and vissim rechecked
//
// Revision 1.2  2003/02/07 10:44:19  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

# include <iostream>
# include <sstream>
# include <fstream>
# include <vector>
# include <cstdlib>
# include <ctime>
# include <string>
#include <utils/common/SUMOTime.h>

typedef int (*CMPFUN)(SUMOTime, SUMOTime);


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
/// OD attributes input
class OD_IN {
public:
    // source district name
    std::string from;
    /// destination district name
    std::string to;

    unsigned short int how_many;
};

/// OD attributes output
class OD_OUT {
public:
    // source district name
    std::string from;
    /// destination district name
    std::string to;
    SUMOTime type;
    SUMOTime time;
};

/// Meta data input
const int MAX_CARTYPES=100;
const int MAX_INFILES=200;
const int MAX_LINELENGTH=500;
const int MAX_CONTENT=200;

typedef struct content {
    int id;
    int max;
    int cartype[MAX_CARTYPES];
    SUMOReal fraction[MAX_CARTYPES];
} ODContent;




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


#ifndef ODSubroutines_h
#define ODSubroutines_h
//---------------------------------------------------------------------------//
//                        ODsubroutines.h -
//  some sorting functions
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
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <vector>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ODDistrictCont;



/* =========================================================================
 * method declarations
 * ======================================================================= */
extern int ODread (std::string, std::vector<OD_IN>&, long*, long*,
                   long*, long*, float* );

extern int ODPtvread (std::string, std::vector<OD_IN>&, long*, long*, long*,
                      long*, float* );

extern int ODWrite (std::string , std::vector<OD_OUT>& , long int,
                    ODDistrictCont &districts);

extern int Get_rand (int, int, int, int*, int*, bool);

extern void IndexSort (long int*, long int*, CMPFUN, long int);

extern void ODInpread (std::string,
                       std::vector<std::string> &infiles/*string infiles[MAX_INFILES]*/,
                       std::vector<ODContent> &content/*[MAX_CONTENT]*//*, int**/);


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ODsubroutines.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


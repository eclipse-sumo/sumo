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
// Revision 1.3  2003/04/01 15:22:21  dkrajzew
// parsing of multiple (vissim)-matrices added
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
	extern int ODPtvread (string, vector<OD_IN>&, long*, long*, long*,
						  long*, float* );
	extern int ODWrite (string , vector<OD_OUT>& , long int);
    extern int Get_rand (int, int, int, int*, int*, bool);
	extern void IndexSort (long int*, long int*, CMPFUN, long int);
	extern void ODInpread (string , string infiles[MAX_INFILES],
						   content content[MAX_CONTENT],int*);

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ODsubroutines.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


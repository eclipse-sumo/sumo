#ifndef ODMatrix_h
#define ODMatrix_h
//---------------------------------------------------------------------------//
//                        ODmatrix.h -
//  some matrix usage functions
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Peter Mieth
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Peter.Mieth@dlr.de
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
// Revision 1.4  2003/04/09 15:57:06  dkrajzew
// reinsertion of simple OD-matrix
//
// Revision 1.3  2003/04/01 15:22:20  dkrajzew
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
#endif // HAVE_CONFIG_H

# include <iostream>
# include <sstream>
# include <fstream>
# include <vector>
# include <cstdlib>
# include <ctime>
# include <string>

typedef int (*CMPFUN)(long int, long int);

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
/// OD attributes input
class OD_IN {
public:
  string from;
  string to;
  unsigned short int how_many;
};

/// OD attributes output
class OD_OUT {
public:
  string from;
  string to;
  unsigned int type;
  unsigned int time;
};

/// Meta data input
const int MAX_CARTYPES=100;
const int MAX_INFILES=200;
const int MAX_LINELENGTH=500;
const int MAX_CONTENT=200;

struct content {
	int id;
	int max;
	int	cartype[MAX_CARTYPES];
	float fraction[MAX_CARTYPES];
	};




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ODmatrix.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


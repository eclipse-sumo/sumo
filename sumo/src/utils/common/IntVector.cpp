//---------------------------------------------------------------------------//
//                        IntVector.cpp -
//  A simple vector of integers
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.5  2003/07/21 05:16:08  dkrajzew
// inifinite loop patched; one should make both classes a single templte!
//
// Revision 1.4  2003/06/05 11:54:48  dkrajzew
// class templates applied; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <algorithm>
#include "IntVector.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
void
IntVectorHelper::removeDouble(IntVector &v)
{
    size_t pos = 0;
    IntVector::iterator i=v.begin();
    while(i!=v.end()) {
        for(IntVector::iterator j=i+1; j!=v.end()&&!changed; ) {
            if(*i==*j) {
                j = v.erase(j);
            } else {
                j++;
            }
        }
        i++;
    }
}


bool
IntVectorHelper::subSetExists(const IntVector &v1, const IntVector &v2)
{
    for(IntVector::const_iterator i=v1.begin(); i!=v1.end(); i++) {
        int val1 = (*i);
		if(find(v2.begin(), v2.end(), val1)!=v2.end()) {
			return true;
		}
    }
    return false;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "IntVector.icc"
//#endif

// Local Variables:
// mode:C++
// End:



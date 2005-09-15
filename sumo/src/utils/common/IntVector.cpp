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
// Revision 1.9  2005/09/15 12:13:08  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/04/28 09:02:46  dkrajzew
// level3 warnings removed
//
// Revision 1.7  2003/10/30 13:43:09  dkrajzew
// debug output added
//
// Revision 1.6  2003/07/21 05:35:18  dkrajzew
// forgot to remove the usage of an undefinef variable; patched
//
// Revision 1.5  2003/07/21 05:16:08  dkrajzew
// inifinite loop patched; one should make both classes a single templte!
//
// Revision 1.4  2003/06/05 11:54:48  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <vector>
#include <algorithm>
#include "IntVector.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


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
        for(IntVector::iterator j=i+1; j!=v.end(); ) {
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


std::ostream &
operator<<(std::ostream &os, const IntVector &iv)
{
    for(IntVector::const_iterator i=iv.begin(); i!=iv.end(); i++) {
        if(i!=iv.begin()) {
            os << ", ";
        }
        os << (*i);
    }
    return os;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



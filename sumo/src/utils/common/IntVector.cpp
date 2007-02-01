/****************************************************************************/
/// @file    IntVector.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// A simple vector of integers
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <algorithm>
#include "IntVector.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
IntVectorHelper::removeDouble(IntVector &v)
{
    IntVector::iterator i=v.begin();
    while (i!=v.end()) {
        for (IntVector::iterator j=i+1; j!=v.end();) {
            if (*i==*j) {
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
    for (IntVector::const_iterator i=v1.begin(); i!=v1.end(); i++) {
        int val1 = (*i);
        if (find(v2.begin(), v2.end(), val1)!=v2.end()) {
            return true;
        }
    }
    return false;
}


std::ostream &
operator<<(std::ostream &os, const IntVector &iv)
{
    for (IntVector::const_iterator i=iv.begin(); i!=iv.end(); i++) {
        if (i!=iv.begin()) {
            os << ", ";
        }
        os << (*i);
    }
    return os;
}



/****************************************************************************/


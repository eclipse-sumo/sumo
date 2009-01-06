/****************************************************************************/
/// @file    Bresenham.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 07 Jun 2002
/// @version $Id$
///
// A class to realise a uniform n:m - relationship using the
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include "Bresenham.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
Bresenham::compute(BresenhamCallBack *callBack, SUMOReal val1, SUMOReal val2)
{
    // case1: both numbers are equal
    if (val1==val2) {
        for (SUMOReal step=0; step<val1; step++) {
            callBack->execute(step, step);
        }
        return;
    }
    // case2: the first value is higher
    if (val1>val2) {
        SUMOReal pos = 0;
        SUMOReal prop = val2 / val1;
        SUMOReal cnt = prop / 2;
        for (SUMOReal i=0; i<val1; i++) {
            callBack->execute(i, pos);
            cnt += prop;
            if (cnt>=1.0) {
                pos++;
                cnt -= 1.0;
            }
        }
        return;
    }
    // case3: the first value is smaller than the second
    if (val1<val2) {
        SUMOReal pos = 0;
        SUMOReal prop = val1 / val2;
        SUMOReal cnt = prop / 2;
        for (SUMOReal i=0; i<val2; i++) {
            callBack->execute(pos, i);
            cnt += prop;
            if (cnt>=1.0) {
                pos++;
                cnt -= 1.0;
            }
        }
        return;
    }
}



/****************************************************************************/


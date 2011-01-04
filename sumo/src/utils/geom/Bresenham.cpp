/****************************************************************************/
/// @file    Bresenham.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 07 Jun 2002
/// @version $Id$
///
// A class to realise a uniform n:m - relationship using the
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/StdDefs.h>
#include "Bresenham.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
Bresenham::compute(BresenhamCallBack *callBack, const unsigned int val1, const unsigned int val2) {
    const unsigned int smaller = MIN2(val1, val2);
    const unsigned int greater = MAX2(val1, val2);
    unsigned int pos = 0;
    unsigned int c = smaller;
    for (unsigned int i = 0; i < greater; i++) {
        if (smaller == val1) {
            callBack->execute(pos, i);
        } else {
            callBack->execute(i, pos);
        }
        c += 2 * smaller;
        if (c >= 2 * greater) {
            pos++;
            c -= 2 * greater;
        }
    }
}



/****************************************************************************/


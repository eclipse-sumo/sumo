/****************************************************************************/
/// @file    Bresenham.cpp
/// @author  Daniel Krajzewicz
/// @date    Fri, 07 Jun 2002
/// @version $Id: $
///
// A class to realise a uniform n:m - relationship using the
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

#include <iostream>
#include "Bresenham.h"

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


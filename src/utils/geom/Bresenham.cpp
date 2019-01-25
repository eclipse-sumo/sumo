/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Bresenham.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Fri, 07 Jun 2002
/// @version $Id$
///
// A class to realise a uniform n:m - relationship using the
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <iostream>
#include <utils/common/StdDefs.h>
#include "Bresenham.h"


// ===========================================================================
// method definitions
// ===========================================================================
void
Bresenham::compute(BresenhamCallBack* callBack, const int val1, const int val2) {
    const int smaller = MIN2(val1, val2);
    const int greater = MAX2(val1, val2);
    int pos = 0;
    int c = smaller;
    for (int i = 0; i < greater; i++) {
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


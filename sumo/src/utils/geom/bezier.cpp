/****************************************************************************/
/// @file    bezier.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2003-11-19
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2003-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


/* Subroutine to generate a Bezier curve.
    Copyright (c) 2000 David F. Rogers. All rights reserved.

    b[]        = array containing the defining polygon vertices
                  b[1] contains the x-component of the vertex
                  b[2] contains the y-component of the vertex
                  b[3] contains the z-component of the vertex
    Basis      = function to calculate the Bernstein basis value (see MECG Eq 5-65)
    cpts       = number of points to be calculated on the curve
    Fractrl    = function to calculate the factorial of a number
    j[]        = array containing the basis functions for a single value of t
    npts       = number of defining polygon vertices
    p[]        = array containing the curve points
                 p[1] contains the x-component of the point
                 p[2] contains the y-component of the point
                 p[3] contains the z-component of the point
    t          = parameter value 0 <= t <= 1
*/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <math.h>
#include <iostream>
#include <utils/common/StdDefs.h>
#include "PositionVector.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

/* function to calculate the factorial */

SUMOReal factrl(int n) {
    static int ntop = 6;
    static SUMOReal a[33] = {
        1.0, 1.0, 2.0, 6.0, 24.0, 120.0, 720.0
    }
    ; /* fill in the first few values */
    int j1;

    if (n < 0) {
        throw 1;
    } //cout << "\nNegative factorial in routine FACTRL\n";
    if (n > 32) {
        throw 1;
    } //cout << "\nFactorial value too large in routine FACTRL\n";

    while (ntop < n) { /* use the precalulated value for n = 0....6 */
        j1 = ntop++;
        a[ntop] = a[j1] * ntop;
    }
    return a[n]; /* returns the value n! as a SUMOReal */
}

/* function to calculate the factorial function for Bernstein basis */

SUMOReal Ni(int n, int i) {
    return factrl(n) / (factrl(i) * factrl(n - i));
}

/* function to calculate the Bernstein basis */

SUMOReal Basis(int n, int i, SUMOReal t) {
    /* handle the special cases to avoid domain problem with pow */
    const SUMOReal ti = (i == 0) ? 1.0 : pow(t, i); /* this is t^i */
    const SUMOReal tni = (n == i) ? 1.0 : pow(1 - t, n - i); /* this is (1-t)^(n-i) */
    return Ni(n, i) * ti * tni;
}

/* Bezier curve subroutine */
void
bezier(int npts, SUMOReal b[], int cpts, SUMOReal p[]) {
    int i;
    int j;
    int i1;
    int icount;
    int jcount;

    const SUMOReal step = (SUMOReal) 1.0 / (cpts - 1);
    SUMOReal t;

    /*    calculate the points on the Bezier curve */

    icount = 0;
    t = 0;

    for (i1 = 1; i1 <= cpts; i1++) { /* main loop */

        if ((1.0 - t) < 5e-6) {
            t = 1.0;
        }

        for (j = 1; j <= 3; j++) { /* generate a point on the curve */
            jcount = j;
            p[icount + j] = 0.;
            for (i = 1; i <= npts; i++) { /* Do x,y,z components */
                p[icount + j] = p[icount + j] + Basis(npts - 1, i - 1, t) * b[jcount];
                jcount = jcount + 3;
            }
        }

        icount = icount + 3;
        t = t + step;
    }
}


PositionVector
bezier(const PositionVector& init, int numPoints) {
    PositionVector ret;
    SUMOReal* def = new SUMOReal[1 + (int)init.size() * 3];
    for (int i = 0; i < (int)init.size(); ++i) {
        // starts at index 1
        def[i * 3 + 1] = init[i].x();
        def[i * 3 + 2] = init[i].z();
        def[i * 3 + 3] = init[i].y();
    }
    SUMOReal* ret_buf = new SUMOReal[numPoints * 3 + 1];
    bezier((int)init.size(), def, numPoints, ret_buf);
    delete[] def;
    Position prev;
    for (int i = 0; i < (int)numPoints; i++) {
        Position current(ret_buf[i * 3 + 1], ret_buf[i * 3 + 3], ret_buf[i * 3 + 2]);
        if (prev != current && !ISNAN(current.x()) && !ISNAN(current.y()) && !ISNAN(current.z())) {
            ret.push_back(current);
        }
        prev = current;
    }
    delete[] ret_buf;
    return ret;
}

/****************************************************************************/


/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2003-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    bezier.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2003-11-19
/// @version $Id$
///
// missing_desc
/****************************************************************************/
#ifndef bezier_h
#define bezier_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "PositionVector.h"

/* Bezier curve subroutine */
void
bezier(int npts, double b[], int cpts, double p[]);


PositionVector
bezier(const PositionVector& init, int numPoints);


#endif

/****************************************************************************/


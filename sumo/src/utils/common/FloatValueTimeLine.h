/****************************************************************************/
/// @file    FloatValueTimeLine.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 22 Jan 2004
/// @version $Id$
///
// A list of time ranges with SUMOReal values
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
#ifndef FloatValueTimeLine_h
#define FloatValueTimeLine_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "ValueTimeLine.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class FloatValueTimeLine
 * A time line being a list of time period with assigned SUMOReal values.
 * Derived from the template time line to allow the computation of
 *  non-existing values through interpolation.
 */
class FloatValueTimeLine : public ValueTimeLine<SUMOReal>
{
public:
    /// Constructor
    FloatValueTimeLine() { }

    /// Destructor
    ~FloatValueTimeLine() { }

};


#endif

/****************************************************************************/


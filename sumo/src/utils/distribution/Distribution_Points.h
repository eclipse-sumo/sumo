/****************************************************************************/
/// @file    Distribution_Points.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The description of a distribution by a curve
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
#ifndef Distribution_Points_h
#define Distribution_Points_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Distribution.h"
#include <utils/geom/Position2DVector.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Distribution_Points
 * A description of a distribution that uses a set of points in a 2d-space
 *  to describe the values (each points x-value) and theri possibilities
 *  (each points y-value)
 */
class Distribution_Points :
            public Distribution
{
public:
    /// Constructor
    Distribution_Points(const std::string &id,
                        const Position2DVector &points, bool interpolating=false) throw();

    /// Destructor
    virtual ~Distribution_Points() throw();

    /// Returns the maximum value of this distribution
    SUMOReal getMax() const;

    size_t getAreaNo() const;
    SUMOReal getAreaBegin(size_t index) const;
    SUMOReal getAreaEnd(size_t index) const;
    SUMOReal getAreaPerc(size_t index) const;


protected:
    /// The list of points that describe the distribution
    Position2DVector myPoints;

    mutable bool myProbabilitiesAreComputed;

    mutable std::vector<SUMOReal> myProbabilities;

    bool myInterpolateDist;

};


#endif

/****************************************************************************/


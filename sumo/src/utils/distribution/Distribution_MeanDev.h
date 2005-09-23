#ifndef Distribution_MeanDev_h
#define Distribution_MeanDev_h
//---------------------------------------------------------------------------//
//                        Distribution_MeanDev.h -
//      A distribution described by the mean value and std-dev amount
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
// $Log$
// Revision 1.6  2005/09/23 06:06:37  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 12:17:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2004/01/28 12:35:22  dkrajzew
// retrival of a distribution maximum value added; documentation added
//
// Revision 1.3  2003/06/06 11:01:09  dkrajzew
// windows eol removed
//
// Revision 1.2  2003/06/05 14:33:44  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "Distribution.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class Distribution_MeanDev
 * A description of distribution by the distribution's mean value and a
 *  standard deviation.
 * Incomplete and unused yet. This class should be overridden by derived
 *  classes
 */
class Distribution_MeanDev :
        public Distribution {
public:
    /// Constructor
    Distribution_MeanDev(const std::string &id, SUMOReal mean,
        SUMOReal deviation);

    /// Destructor
    virtual ~Distribution_MeanDev();

    /// Returns the maximum value of this distribution
    SUMOReal getMax() const;

private:
    /// The distribution's mean
    SUMOReal myMeanValue;

    /// The distribution's standard deviation
    SUMOReal myDeviation;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


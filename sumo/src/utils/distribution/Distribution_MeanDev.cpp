//---------------------------------------------------------------------------//
//                        Distribution_MeanDev.cpp -
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
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
#include "Distribution_MeanDev.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
Distribution_MeanDev::Distribution_MeanDev(const std::string &id,
                                           double mean, double deviation)
    : Distribution(id), myMeanValue(mean), myDeviation(deviation)
{
}


Distribution_MeanDev::~Distribution_MeanDev()
{
}


double
Distribution_MeanDev::getMax() const
{
    return myMeanValue + myDeviation;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



#ifndef Distribution_MeanDev_h
#define Distribution_MeanDev_h
//---------------------------------------------------------------------------//
//                        Distribution_MeanDev.h -  ccc
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
// Revision 1.2  2003/06/05 14:33:44  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include "Distribution.h"

class Distribution_MeanDev :
        public Distribution {
public:
    Distribution_MeanDev(const std::string &id, double mean,
        double deviation);
    virtual ~Distribution_MeanDev();
private:
    double myMeanValue;
    double myDeviation;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "Distribution_MeanDev.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


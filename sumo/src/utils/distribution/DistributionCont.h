#ifndef DistributionCont_h
#define DistributionCont_h
//---------------------------------------------------------------------------//
//                        DistributionCont.h -  ccc
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


#include <string>
#include <map>
#include <utils/distribution/Distribution.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class DistributionCont {
public:
    static bool dictionary(const std::string &type, const std::string &id,
        Distribution *d);
    static Distribution *dictionary(const std::string &type,
        const std::string &id);
private:
    typedef std::map<std::string, Distribution*> DistDict;
    typedef std::map<std::string, DistDict> TypedDistDict;
    static TypedDistDict myDict;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "DistributionCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


#ifndef NBTrafficLightLogicCont_h
#define NBTrafficLightLogicCont_h
//---------------------------------------------------------------------------//
//                        NBTrafficLightLogicCont.h -
//  A container for traffic light logic vectors
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
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <map>
#include <string>
#include "NBTrafficLightLogicVector.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class NBTrafficLightLogicCont {
private:
    typedef std::map<std::string, NBTrafficLightLogicVector*> ContType;
    static ContType _cont;
public:
    static bool insert(const std::string &id,
        NBTrafficLightLogicVector *logics);
    /// saves all known logics
    static void writeXML(std::ostream &into);
    /// destroys all stored logics
    static void clear();
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NBTrafficLightLogicCont.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


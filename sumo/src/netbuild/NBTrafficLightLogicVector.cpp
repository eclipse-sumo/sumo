//---------------------------------------------------------------------------//
//                        NBTrafficLightLogicVector.cpp -
//  A vector of traffic lights logics
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
#include <vector>
#include <iostream>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicVector.h"

NBTrafficLightLogicVector::NBTrafficLightLogicVector()
{
}

NBTrafficLightLogicVector::~NBTrafficLightLogicVector()
{
    for(LogicVector::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        delete (*i);
    }
}


void
NBTrafficLightLogicVector::add(NBTrafficLightLogic *logic)
{
    if(!contains(logic)) {
        _cont.push_back(logic);
    } else {
        delete logic;
    }
}


void
NBTrafficLightLogicVector::add(const NBTrafficLightLogicVector &cont)
{
    for(LogicVector::const_iterator i=cont._cont.begin(); i!=cont._cont.end(); i++) {
        if(!contains(*i)) {
            NBTrafficLightLogic *logic = *i;
            add(new NBTrafficLightLogic(*logic));
        }
    }
}


void
NBTrafficLightLogicVector::writeXML(std::ostream &os) const
{
    size_t pos = 0;
    for(LogicVector::const_iterator i=_cont.begin(); i!=_cont.end(); i++) {
        (*i)->writeXML(os, pos++);
    }
}

bool
NBTrafficLightLogicVector::contains(NBTrafficLightLogic *logic) const
{
    for(LogicVector::const_iterator i=_cont.begin(); i!=_cont.end(); i++) {
        if((*i)->equals(*logic)) {
            return true;
        }
    }
    return false;
}


int
NBTrafficLightLogicVector::size() const
{
    return _cont.size();
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBTrafficLightLogicVector.icc"
//#endif

// Local Variables:
// mode:C++
// End:



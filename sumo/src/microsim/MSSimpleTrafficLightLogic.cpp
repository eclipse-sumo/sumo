//---------------------------------------------------------------------------//
//                        MSSimpleTrafficLightLogic.cpp -
//  The basic traffic light logic
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
// Revision 1.3  2003/04/04 07:13:20  dkrajzew
// Yellow phases must be now explicetely given
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#include <utility>
#include <vector>
#include <bitset>
#include "MSEventControl.h"
#include "MSNet.h"
#include "MSTrafficLightLogic.h"
#include "MSSimpleTrafficLightLogic.h"


template< size_t N >
std::bitset<64> MSSimpleTrafficLightLogic<N>::_allClear;

template< size_t N >
MSSimpleTrafficLightLogic<N>::MSSimpleTrafficLightLogic<N>(
    const std::string &id, const Phases &phases, size_t step)
    : MSTrafficLightLogic(id), _phases(phases),
    _step(step)
{
}


template< size_t N >
MSSimpleTrafficLightLogic<N>::~MSSimpleTrafficLightLogic<N>()
{
}


template< size_t N > void
MSSimpleTrafficLightLogic<N>::applyPhase(MSLogicJunction::Request &request) const
{
    assert(_phases.size()>_step);
    std::bitset<64> allowed = _phases[_step].driveMask;
    for(size_t i=0; i<request.size(); i++) {
        request[i] = request[i] & allowed.test(i);
    }
}


template< size_t N > const std::bitset<64> &
MSSimpleTrafficLightLogic<N>::linkPriorities() const
{
    assert(_phases.size()>_step);
    return _phases[_step].breakMask;
}


template< size_t N > size_t
MSSimpleTrafficLightLogic<N>::nextStep()
{
    // increment the index to the current phase
    _step++;
    if(_step==_phases.size()) {
        _step = 0;
    }
    return _step;
}


template< size_t N > MSNet::Time
MSSimpleTrafficLightLogic<N>::duration() const
{
    assert(_phases.size()>_step);
    return _phases[_step].duration;
}




template< size_t N > MSNet::Time
MSSimpleTrafficLightLogic<N>::nextPhase(MSLogicJunction::InLaneCont &inLanes)
{
    // increment the index to the current phase
    nextStep();
    // reset the link priorities
    setLinkPriorities(inLanes);
    // set the next event
    return duration();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSSimpleTrafficLightLogic.icc"
//#endif

// Local Variables:
// mode:C++
// End:



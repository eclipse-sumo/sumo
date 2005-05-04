#ifndef MSExtendedTrafficLightLogic_h
#define MSExtendedTrafficLightLogic_h
//---------------------------------------------------------------------------//
//                        MSExtendedTrafficLightLogic.h -
//  The base of a traffic light logic that knows the incoming lanes
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : May 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.3  2005/05/04 08:22:19  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2005/01/27 14:22:45  dkrajzew
// ability to open the complete phase definition added; code style adapted
//
// Revision 1.2  2005/01/06 10:48:07  dksumo
// 0.8.2.1 patches
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSSimpleTrafficLightLogic.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NLDetectorBuilder;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSExtendedTrafficLightLogic
 * This is a base class for extended traffic lights logic which need
 *  information about the incoming lanes.
 * It adds the function "init" to the API in order to make an initialisation
 *  of the tls-logic with incoming lanes possible.
 */
class MSExtendedTrafficLightLogic
    : public MSSimpleTrafficLightLogic
{
public:
    /// Constructor
    MSExtendedTrafficLightLogic(const std::string &id, const Phases &phases,
        size_t step, size_t delay)
        : MSSimpleTrafficLightLogic(id, phases, step, delay) { }

    /// Destructor
    ~MSExtendedTrafficLightLogic() { }

    /// Initialises the tls with information about incoming lanes
    virtual void init(NLDetectorBuilder &nb,
        const std::vector<MSLane*> &lanes,
        std::map<std::string, std::vector<std::string> > &edgeContinuations,
        double det_offset) = 0;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
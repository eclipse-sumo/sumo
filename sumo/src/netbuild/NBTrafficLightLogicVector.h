#ifndef NBTrafficLightLogicVector_h
#define NBTrafficLightLogicVector_h
//---------------------------------------------------------------------------//
//                        NBTrafficLightLogicVector.h -
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
// $Log$
// Revision 1.8  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/04/27 11:48:26  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.6  2003/12/04 13:03:58  dkrajzew
// possibility to pass the tl-type from the netgenerator added
//
// Revision 1.5  2003/06/05 11:43:36  dkrajzew
// class templates applied; documentation added
//
// Revision 1.4  2003/03/20 16:23:10  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.3  2003/03/03 14:59:22  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <vector>
#include <iostream>
#include "NBConnectionDefs.h"

/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBTrafficLightLogic;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NBTrafficLightLogicVector
 * This class holds all computed traffic light phases (with changing cliques)
 * valid for the given set of connections
 */
class NBTrafficLightLogicVector {
public:
    /// Constructor
    NBTrafficLightLogicVector(const NBConnectionVector &inLanes,
        std::string type);

    /// Destructor
    ~NBTrafficLightLogicVector();

    /// Adds a further traffic light phaselists to the list
    void add(NBTrafficLightLogic *logic);

    /// Adds all phaselists stored within the given list to the list of logics of this type
    void add(const NBTrafficLightLogicVector &cont);

    /// Writes the logics for this junction
    void writeXML(std::ostream &os) const;

    /// returns the information whether the given phaselists is already within this container
    bool contains(NBTrafficLightLogic *logic) const;

    /// Returns the number of phaselists within this container
    int size() const;

    /// Returns the type of this traffic light logic
    const std::string &getType() const;

private:
    /// The links participating in this junction
    NBConnectionVector myInLinks;

    /// Definition of the container type for theknown phaselist
    typedef std::vector<NBTrafficLightLogic*> LogicVector;

    /// Container type for the phaselist
    LogicVector _cont;

    /// The type of the logic (traffic-light, actuated, agentbased)
    std::string myType;

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


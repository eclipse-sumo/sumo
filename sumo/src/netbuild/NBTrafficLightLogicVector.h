/****************************************************************************/
/// @file    NBTrafficLightLogicVector.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A vector of traffic lights logics
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
#ifndef NBTrafficLightLogicVector_h
#define NBTrafficLightLogicVector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include "NBConnectionDefs.h"

// ===========================================================================
// class declarations
// ===========================================================================
class NBTrafficLightLogic;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
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
    void writeXML(OutputDevice &os) const;

    /// returns the information whether the given phaselists is already within this container
    bool contains(NBTrafficLightLogic *logic) const;


    /** @brief Returns the number of phaselists within this container
     * @return The number of programs stored
     */
    unsigned int size() const throw() {
        return (unsigned int) myCont.size();
    }


    /** @brief Returns the type of this traffic light logic
     * @return The type of this tls
     */
    const std::string &getType() const throw() {
        return myType;
    }


    void setOffsetsToHalf();
    void setOffsetsToQuarter();

private:
    /// The links participating in this junction
    NBConnectionVector myInLinks;

    /// Definition of the container type for theknown phaselist
    typedef std::vector<NBTrafficLightLogic*> LogicVector;

    /// Container type for the phaselist
    LogicVector myCont;

    /// The type of the logic (traffic-light, actuated, agentbased)
    std::string myType;

};


#endif

/****************************************************************************/


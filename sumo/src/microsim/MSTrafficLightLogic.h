#ifndef MSTrafficLightLogic_h
#define MSTrafficLightLogic_h
//---------------------------------------------------------------------------//
//                        MSTrafficLightLogic.h -
//  The parent class for traffic light logics
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
// Revision 1.3  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <map>
#include <string>
#include <bitset>
#include "MSNet.h"
#include "MSLogicJunction.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSTrafficLightLogic
 */
class MSTrafficLightLogic {
public:
    /// Constructor
    MSTrafficLightLogic(const std::string &id);

    /// Destructor
    virtual ~MSTrafficLightLogic();

    /** @brief Inserts MSTrafficLightLogic into the static dictionary
        Returns true if the key id isn't already in the dictionary.
        Otherwise returns false (the logic is not inserted then). */
    static bool dictionary(const std::string &name,
        MSTrafficLightLogic *logic);

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    virtual MSNet::Time nextPhase(MSLogicJunction::InLaneCont &inLanes) = 0;

    /** @brief Returns the MSEdgeControl associated to the key id if exists,
        Otherwise returns 0. */
    static MSTrafficLightLogic *dictionary(const std::string &name);

    /** @brief Applies the right-of-way rules of the phase specified by the second argument to the first argument
        Requests of vehicles which are not allowed to drive as they
        have red light are masked out from the given request */
    virtual void applyPhase(MSLogicJunction::Request &request) const = 0;

    /** Returns the link priorities for the given phase */
    virtual const std::bitset<64> &linkPriorities() const = 0;

    /// Returns the index of the phase next to the given phase
    virtual size_t nextStep() = 0;

    /// Returns the duration of the numbered phase
    virtual MSNet::Time duration() const = 0;

    /** @brief Sets the priorities of incoming lanes
        This must be done as they change when the light changes */
    void setLinkPriorities(MSLogicJunction::InLaneCont &inLanes);

	/// Returns the current step
	virtual size_t step() const = 0;

protected:
    /// Definition of the dictionary type for traffic light logics
    typedef std::map<std::string, MSTrafficLightLogic*> DictType;

    /// Traffic light logic dictionary
    static DictType _dict;

    /// The id of the logic
    std::string _id;

private:
    /// invalidated copy constructor
    MSTrafficLightLogic(const MSTrafficLightLogic &s);

    /// invalidated assignment operator
    MSTrafficLightLogic& operator=(const MSTrafficLightLogic &s);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSTrafficLightLogic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


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
// Revision 1.6  2003/06/06 10:39:17  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.5  2003/06/05 16:11:03  dkrajzew
// new usage of traffic lights implemented
//
// Revision 1.4  2003/05/21 15:15:42  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
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
#include <helpers/Command.h>
#include "MSNet.h"
#include "MSLogicJunction.h"


class MSLink;
class MSEventControl;

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSTrafficLightLogic
 */
class MSTrafficLightLogic {
public:
    /// Constructor
    MSTrafficLightLogic(const std::string &id, size_t delay);

    /// Destructor
    virtual ~MSTrafficLightLogic();

    /** @brief Inserts MSTrafficLightLogic into the static dictionary
        Returns true if the key id isn't already in the dictionary.
        Otherwise returns false (the logic is not inserted then). */
    static bool dictionary(const std::string &name,
        MSTrafficLightLogic *logic);

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    virtual MSNet::Time nextPhase() = 0;

    /** @brief Returns the MSEdgeControl associated to the key id if exists,
        Otherwise returns 0. */
    static MSTrafficLightLogic *dictionary(const std::string &name);

    /** Returns the link priorities for the given phase */
    virtual const std::bitset<64> &linkPriorities() const = 0;

    virtual const std::bitset<64> &yellowMask() const = 0;

    virtual const std::bitset<64> &allowed() const = 0;

    /// Returns the index of the phase next to the given phase
    virtual size_t nextStep() = 0;

    /// Returns the duration of the numbered phase
    virtual MSNet::Time duration() const = 0;

    /** @brief Sets the priorities of incoming lanes
        This must be done as they change when the light changes */
    void setLinkPriorities();

	/// Returns the current step
	virtual size_t step() const = 0;

    void maskRedLinks();

    friend class NLSucceedingLaneBuilder;

protected:
    /// Adds a link on building
    void addLink(MSLink *link, size_t pos);

protected:
    /// Definition of the dictionary type for traffic light logics
    typedef std::map<std::string, MSTrafficLightLogic*> DictType;

    /// Traffic light logic dictionary
    static DictType _dict;

    /// The id of the logic
    std::string _id;

    /// Definition of the list of links which participate in this tl-light
    typedef std::vector<MSLink*> LinkVector;

    /// Definition of a list which holds lists of links which do have the same attribute
    typedef std::vector<LinkVector> LinkVectorVector;

    /// The list of links which do participate in this traffic light
    LinkVectorVector myLinks;

private:
    /**
     * Class realising the switch between the traffic light states (phases
     */
    class SwitchCommand : public Command {
    private:
        /// The logic to be executed on a switch
        MSTrafficLightLogic *_tlLogic;

    public:
        /// Constructor
        SwitchCommand(MSTrafficLightLogic *tlLogic)
            : _tlLogic(tlLogic) { }

        /// Destructor
        ~SwitchCommand() { }

        /** @brief Executes this event
            Executes the regarded junction's "nextPhase"- method */
        MSNet::Time execute() {
            return _tlLogic->nextPhase();
        }

    private:
        /// invalidated default constructor
        SwitchCommand();
    };

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


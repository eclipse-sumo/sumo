#ifndef MSTrafficLightJunction_h
#define MSTrafficLightJunction_h
//---------------------------------------------------------------------------//
//                        MSTrafficLightJunction.h -
//  A traffic light junction
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
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <helpers/Command.h>
#include <microsim/MSNet.h>
#include "MSTrafficLightLogic.h"
#include "MSRightOfWayJunction.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEventControl;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * MSTrafficLightJunction
 * A junction which - beside right-of-way rules - also has a traffic light.
 * The traffic light masks incoming requests away and so only vehicles which
 * have green light are moved. The movement is done as in MSRightOfWayJunction
 * which is the parent class of this one.
 */
class MSTrafficLightJunction : public MSRightOfWayJunction {
public:
    /// constructor
    MSTrafficLightJunction(const std::string &id, double x, double y,
        InLaneCont in, MSJunctionLogic* logic, MSTrafficLightLogic *tlLogic,
        size_t delay, size_t initStep, MSEventControl &ec);

    /// destructor
    virtual ~MSTrafficLightJunction();

    /** Sets the repond-fields */
    virtual bool setAllowed();

    /// Initialises the junction after the network was completely loaded
    virtual void postloadInit();

protected:
    /// The used logic
    MSTrafficLightLogic *_tlLogic;

private:

    /**
     * Class realising the switch between the traffic light states (phases
     */
    class SwitchCommand : public Command {
    private:
        /// The lanes incoming to the junction
        InLaneCont &_inLanes;

        /// The logic to be executed on a switch
        MSTrafficLightLogic *_tlLogic;

    public:
        /// Constructor
        SwitchCommand(InLaneCont &junctionInLanes,
            MSTrafficLightLogic *tlLogic)
            : _inLanes(junctionInLanes), _tlLogic(tlLogic) { }

        /// Destructor
        ~SwitchCommand() { }

        /** @brief Executes this event
            Executes the regarded junction's "nextPhase"- method */
        MSNet::Time execute() {
            return _tlLogic->nextPhase(_inLanes);
        }

    private:
        /// invalidated default constructor
        SwitchCommand();
    };
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSTrafficLightJunction.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


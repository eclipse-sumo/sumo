/****************************************************************************/
/// @file    MSJunctionLogic.h
/// @author  Christian Roessel
/// @date    Wed, 12 Dez 2001
/// @version $Id: $
///
// kinds of logic-implementations.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSJunctionLogic_h
#define MSJunctionLogic_h
// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLogicJunction.h"
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 */
class MSJunctionLogic
{
public:
    /// Destructor.
    virtual ~MSJunctionLogic();

    /// Modifies the passed respond according to the request.
    virtual void respond(const MSLogicJunction::Request& request,
                         const MSLogicJunction::InnerState& innerState,
                         MSLogicJunction::Respond& respond) const = 0;

    /// Returns the logic's number of links.
    unsigned int nLinks();

    /// Returns the logic's number of inLanes.
    unsigned int nInLanes();

    /** @brief Inserts MSJunctionLogic into the static dictionary
        Returns true if the key id isn't already in the dictionary.
        Otherwise returns false. */
    static bool dictionary(std::string id, MSJunctionLogic* junction);

    /** Returns the MSJunctionLogic associated to the key id if exists,
        otherwise returns 0. */
    static MSJunctionLogic* dictionary(std::string id);

    /** Clears the dictionary */
    static void clear();

    static void replace(std::string id, MSJunctionLogic* junction);

protected:
    /// Constructor.
    MSJunctionLogic(unsigned int nLinks, unsigned int nInLanes);

    /// The logic's number of links.
    unsigned int myNLinks;

    /// The logic's number of inLanes.
    unsigned int myNInLanes;

private:
    /** definition of the static dictionary type. */
    typedef std::map< std::string, MSJunctionLogic* > DictType;

    /** Static dictionary to associate string-ids with objects. */
    static DictType myDict;

    /// Default constructor.
    MSJunctionLogic();

    /// Copy constructor.
    MSJunctionLogic(const MSJunctionLogic&);

    /// Assignment operator.
    MSJunctionLogic& operator=(const MSJunctionLogic&);
};


#endif

/****************************************************************************/


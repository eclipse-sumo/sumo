#ifndef MSJunctionLogic_H
#define MSJunctionLogic_H
/***************************************************************************
                          MSJunctionLogic.h  -  Base class for different
                          kinds of logic-implementations.
                             -------------------
    begin                : Wed, 12 Dez 2001
    copyright            : (C) 2001 by Christian Roessel
    email                : roessel@zpr.uni-koeln.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.4  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.3  2003/02/07 10:41:51  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:39:02  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:16  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.2  2002/02/13 16:29:45  croessel
// Added dictionary.
//
// Revision 1.1  2001/12/13 15:43:13  croessel
// Initial commit.
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MSLogicJunction.h"
#include <string>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 */
class MSJunctionLogic
{
public:
    /// Destructor.
    virtual ~MSJunctionLogic();

    /// Modifies the passed respond according to the request.
    virtual void respond( const MSLogicJunction::Request& request,
        const MSLogicJunction::InnerState& innerState,
        MSLogicJunction::Respond& respond ) const = 0;

    /// Returns the logic's number of links.
    unsigned int nLinks();

    /// Returns the logic's number of inLanes.
    unsigned int nInLanes();

    /** @brief Inserts MSJunctionLogic into the static dictionary
        Returns true if the key id isn't already in the dictionary.
        Otherwise returns false. */
    static bool dictionary( std::string id, MSJunctionLogic* junction );

    /** Returns the MSJunctionLogic associated to the key id if exists,
        otherwise returns 0. */
    static MSJunctionLogic* dictionary( std::string id);

    /** Clears the dictionary */
    static void clear();

protected:
    /// Constructor.
    MSJunctionLogic( unsigned int nLinks, unsigned int nInLanes );

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

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "MSJunctionLogic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

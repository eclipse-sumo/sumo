#ifndef MSInternalJunction_H
#define MSInternalJunction_H
/***************************************************************************
                          MSInternalJunction.h  -  Usual right-of-way
                          junction.
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
// Revision 1.2  2006/10/06 07:13:40  dkrajzew
// debugging internal lanes
//
// Revision 1.1  2006/09/18 10:06:29  dkrajzew
// patching junction-internal state simulation
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSLogicJunction.h"
#include <bitset>
#include <vector>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;
class MSJunctionLogic;
class findCompetitor;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSInternalJunction
 * A class which realises junctions that do regard any kind of a right-of-way.
 * The rules for the right-of-way themselves are stored within the associated
 * "MSJunctionLogic" - structure.
 */
#ifdef HAVE_INTERNAL_LANES
class MSInternalJunction : public MSLogicJunction
{
public:
    /** Use this constructor only. */
    MSInternalJunction( std::string id, const Position2D &position,
        LaneCont incoming,
        LaneCont internal);

    /// Destructor.
    virtual ~MSInternalJunction();

    /** Clears junction's and lane's requests to prepare for the next
        iteration. */
    bool clearRequests();
    void postloadInit();

    /** Sets the information which vehicles may drive */
    virtual bool setAllowed();

private:
    /// Invalidated copy constructor.
    MSInternalJunction( const MSInternalJunction& );

    /// Invalidated assignment operator.
    MSInternalJunction& operator=( const MSInternalJunction& );

};
#endif


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

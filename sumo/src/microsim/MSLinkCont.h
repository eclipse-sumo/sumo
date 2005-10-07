#ifndef MSLinkCont_h
#define MSLinkCont_h
//---------------------------------------------------------------------------//
//                        MSLinkCont.h -
//  A vector of links
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
// Revision 1.5  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.4  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/05/04 08:28:15  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2004/02/16 14:24:13  dkrajzew
// some helper methods added
//
// Revision 1.1  2003/02/07 10:41:51  dkrajzew
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include "MSLink.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSEdge;
class MSLane;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSLinkCont
 * A simple contanier of definitions about how a lane may be left
 */
typedef std::vector<MSLink*> MSLinkCont;


/**
 * @class MSLinkContHelper
 * Some helping functions for dealing with links.
 */
class MSLinkContHelper {
public:
    /** @brief Returns the internal lane that must be passed in order to get to the desired edge
        Returns 0 if no such edge exists */
    static const MSEdge *getInternalFollowingEdge(MSLane *fromLane,
        MSEdge *followerAfterInternal);

    /** @brief Returns the link connecting both lanes
        Both lanes have to be non-internal; 0 may be returned if no connection
        exists */
    static MSLink *getConnectingLink(const MSLane &from
        , const MSLane &to);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

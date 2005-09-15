/***************************************************************************
                          MSJunction.cpp  -  Base class for all kinds of
                                             junctions.
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
namespace
{
    const char rcsid[] =
    "$Id$";
}

// $Log$
// Revision 1.9  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2004/08/02 12:09:39  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.7  2003/11/11 08:38:51  dkrajzew
// consequent position2D instead of two doubles added
//
// Revision 1.6  2003/07/22 15:05:22  dkrajzew
// warnings removed; formatted the code
//
// Revision 1.5  2003/07/16 15:31:14  dkrajzew
// junctions are now able to build gui-wrappers (throws an exception within the gui-version, but the interface is needed)
//
// Revision 1.4  2003/07/07 08:16:28  dkrajzew
// a junction is now able to return its id
//
// Revision 1.3  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:42:28  dkrajzew
// complete deletion within destructors implemented; clear-operator added for container; global file include; junction extended by position information (should be revalidated later)
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:15  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.9  2002/01/30 15:48:30  croessel
// Constuctor and Destructor defined.
//
// Revision 1.8  2001/12/13 15:56:46  croessel
// Initial commit.
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSJunction.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLink;


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
MSJunction::DictType MSJunction::myDict;


/* =========================================================================
 * member method definition
 * ======================================================================= */
MSJunction::MSJunction( std::string id, const Position2D &position)
    : myID( id ), myPosition(position)
{
}

//-------------------------------------------------------------------------//

MSJunction::~MSJunction()
{
}

//-------------------------------------------------------------------------//

bool
MSJunction::dictionary( string id, MSJunction* ptr )
{
    DictType::iterator it = myDict.find( id );
    if ( it == myDict.end() ) {
        // id not in myDict.
        myDict.insert( DictType::value_type( id, ptr ) );
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------//

MSJunction*
MSJunction::dictionary( string id )
{
    DictType::iterator it = myDict.find( id );
    if ( it == myDict.end() ) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}

//-------------------------------------------------------------------------//

void
MSJunction::clear()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}

//-------------------------------------------------------------------------//

std::vector<std::string>
MSJunction::getNames()
{
    std::vector<std::string> ret;
    ret.reserve(MSJunction::myDict.size());
    for(MSJunction::DictType::iterator i=MSJunction::myDict.begin();
        i!=MSJunction::myDict.end(); i++) {
        ret.push_back((*i).first);
    }
    return ret;
}

//-------------------------------------------------------------------------//

const Position2D &
MSJunction::getPosition() const
{
    return myPosition;
}

//-------------------------------------------------------------------------//

void
MSJunction::postloadInit()
{
}

//-------------------------------------------------------------------------//

void
MSJunction::postloadInitContainer()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        (*i).second->postloadInit();
    }
}

std::string
MSJunction::id() const
{
    return myID;
}


GUIJunctionWrapper *
MSJunction::buildJunctionWrapper(GUIGlObjectStorage &)
{
    throw "Only within the gui-version";
}

size_t
MSJunction::dictSize()
{
    return myDict.size();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

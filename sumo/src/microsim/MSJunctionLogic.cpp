/***************************************************************************
                          MSJunctionLogic.cpp  -  Base class for different
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

namespace
{
    const char rcsid[] =
    "$Id$";
}

// $Log$
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
// Revision 1.3  2002/02/13 16:29:45  croessel
// Added dictionary.
//
// Revision 1.2  2002/01/30 15:51:33  croessel
// Destructor defined.
//
// Revision 1.1  2001/12/13 15:44:26  croessel
// Initial commit.
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSJunctionLogic.h"

using namespace std;

//-------------------------------------------------------------------------//

// Init static member.
MSJunctionLogic::DictType MSJunctionLogic::myDict;

//-------------------------------------------------------------------------//

unsigned int
MSJunctionLogic::nLinks()
{
    return myNLinks;
}

//-------------------------------------------------------------------------//

unsigned int
MSJunctionLogic::nInLanes()
{
    return myNInLanes;
}

//-------------------------------------------------------------------------//

MSJunctionLogic::MSJunctionLogic( unsigned int nLinks,
                                  unsigned int nInLanes ) :
    myNLinks( nLinks ),
    myNInLanes( nInLanes )
{
}

//-------------------------------------------------------------------------//

MSJunctionLogic::~MSJunctionLogic()
{
}

//-------------------------------------------------------------------------//

bool
MSJunctionLogic::dictionary( string id, MSJunctionLogic* ptr )
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

MSJunctionLogic*
MSJunctionLogic::dictionary( string id )
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
MSJunctionLogic::clear()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSJunctionLogic.icc"
//#endif

// Local Variables:
// mode:C++
// End:

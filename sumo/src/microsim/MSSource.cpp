//---------------------------------------------------------------------------//
//                        MSSource.cpp  -  Abstract Base Class for all
//                        types of source-like emitters.
//                           -------------------
//  begin                : Wed, 12 Jun 2002
//  copyright            : (C) 2002 by Christian Roessel
//  organisation         : DLR/IVF http://ivf.dlr.de
//  email                : roessel@zpr.uni-koeln.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.3  2005/07/12 12:26:44  dkrajzew
// code style adapted
//
// Revision 1.2  2005/07/04 12:04:54  dksumo
// code beautifying
//
// Revision 1.1  2004/10/22 12:49:26  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.3  2002/07/31 17:32:47  roessel
// Initial sourceforge commit.
//
// Revision 1.8  2002/07/23 15:26:06  croessel
// Added member pos and method setPos.
//
// Revision 1.7  2002/07/16 13:09:53  croessel
// Added using namespace std;
//
// Revision 1.6  2002/07/11 09:10:21  croessel
// Readding file to repository.
//
// Revision 1.4  2002/07/09 18:27:37  croessel
// Constructor parameter changes and new set-methods.
//
// Revision 1.3  2002/07/05 14:49:29  croessel
// Changed member MSLane* to MSLane&.
//
// Revision 1.2  2002/06/18 10:14:44  croessel
// Not needed for release 0.7
//
// Revision 1.1  2002/06/12 19:15:11  croessel
// Initial commit.
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSSource.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSSource::MSSource( void )
    : myID( "uninitialized" ),
      myLane( 0 )
{}


//---------------------------------------------------------------------------//


MSSource::MSSource( string aId, MSLane* aLane )
     : myID( aId ),
       myLane( aLane )
{
}


//---------------------------------------------------------------------------//


MSSource::~MSSource( void )
{

}


//---------------------------------------------------------------------------//


void
MSSource::setId( std::string aID )
{
    myID = aID;
}


//---------------------------------------------------------------------------//


void
MSSource::setLane( MSLane* aLane )
{
    myLane = aLane;
}


//---------------------------------------------------------------------------//


void
MSSource::setPos( double aPos )
{
    myPos = aPos;
}


//--------------- DO NOT DEFINE ANYTHING AFTER THIS POINT -------------------//
#ifdef DISABLE_INLINE
#include "MSSource.icc"
#endif

// Local Variables:
// mode:C++
// End:

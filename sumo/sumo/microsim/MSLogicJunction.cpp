/***************************************************************************
                          MSLogicJunction.cpp  -  Base class for junctions 
                          with one ore more logics.
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
// Revision 1.2  2002/04/18 10:51:22  croessel
// Introduced new method "bool driveRequest()" in class DriveBrakeRequest
// to let findCompetitor check, if a first car set a request.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:17  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.5  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.4  2002/01/31 13:51:40  croessel
// Revision 1.3 Log-message corrected.
//
// Revision 1.3  2002/01/30 16:16:43  croessel
// Destructor defined.
//
// Revision 1.2  2001/12/20 14:30:23  croessel
// using namespace std added.
//
// Revision 1.1  2001/12/13 15:56:47  croessel
// Initial commit.
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSLogicJunction.h"

using namespace std;

//-------------------------------------------------------------------------//

MSLogicJunction::DriveBrakeRequest::DriveBrakeRequest( Request request,
                                                       bool driveReq,
                                                       bool brakeReq) :
    myRequest( request ),
    myDriveRequest( driveReq ),
    myBrakeRequest( brakeReq )
{
}

//-------------------------------------------------------------------------//

bool
MSLogicJunction::DriveBrakeRequest::driveRequest() const
{
    return myDriveRequest;
}

//-------------------------------------------------------------------------//

MSLogicJunction::MSLogicJunction( string id ) : MSJunction( id )
{
}

//-------------------------------------------------------------------------//

MSLogicJunction::~MSLogicJunction() 
{
}

//-------------------------------------------------------------------------//

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSLogicJunction.icc"
//#endif

// Local Variables:
// mode:C++
// End:

//---------------------------------------------------------------------------//
//                        MSTriggeredSource.cpp  -  Concrete Source
//                        that reads emit-times and -speeds from a
//                        file and chooses the routes according to a
//                        distribution. 
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include "MSTriggeredSource.h"


//---------------------------------------------------------------------------//


MSTriggeredSource::~MSTriggeredSource( void )
{

}


//--------------- DO NOT DEFINE ANYTHING AFTER THIS POINT -------------------//
#ifdef DISABLE_INLINE
#include "MSTriggeredSource.icc"
#endif


// $Log$
// Revision 1.1  2002/06/12 19:15:12  croessel
// Initial commit.
//


// Local Variables:
// mode:C++
// End:








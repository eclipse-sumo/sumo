/***************************************************************************
                          Counter.cpp  -  Counter-class.
                             -------------------
    begin                : Tue, 18 Jun 2002
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


#ifdef EXTERNAL_TEMPLATE_DEFINITION
namespace
{
    const char rcsid[] =
    "$Id$";
}
#endif // EXTERNAL_TEMPLATE_DEFINITION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include "Counter.h"


template< typename T > 
size_t Counter< T >::count = 0;


//--------------- DO NOT DEFINE ANYTHING AFTER THIS POINT -------------------//
#ifdef DISABLE_INLINE
#include "Counter.icc"
#endif


// $Log$
// Revision 1.2  2002/06/18 17:20:03  croessel
// Added #ifdef EXTERNAL_TEMPLATE_DEFINITION
//
// Revision 1.1  2002/06/18 16:34:27  croessel
// Initial commit. Made files cfront compliant.
//


// Local Variables:
// mode:C++
// End:




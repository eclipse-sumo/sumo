/***************************************************************************
                          Counter.h  -  Counter-class.
                             -------------------
    begin                : Thu, 20 Dec 2001
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
// Revision 1.2  2002/10/16 16:53:12  dkrajzew
// global inclusion
//
// Revision 1.1  2002/10/16 14:44:45  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.2  2002/06/18 16:35:37  croessel
// Moved deinition to cpp or icc in order to make files cfront compliant.
//
// Revision 1.1.1.1  2002/04/08 07:21:22  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:13  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/01/24 17:22:50  croessel
// Initialization of static member "count" added.
//
// Revision 1.2  2002/01/10 09:15:42  croessel
// Documentation-bug fixed.
//
// Revision 1.1  2002/01/09 15:05:30  croessel
// Initial commit.
//

#ifndef Counter_H
#define Counter_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cstddef>

/**
   Source:
   This is a slightly-modified version of an article that appeared in the
   April 1998 issue of the C/C++ Users Journal.

   Counting Objects in C++ by Scott Meyers.

   Usage: Derive the CLASS that shall be counted privately from Counter (e.g.
   class CLASS : public Counter< CLASS > { ... } and make the howMany()-method
   available by "using Counter< CLASS >::howMany;".
*/
template< typename T  >
class Counter
{
public:
    Counter();

    Counter( const Counter& );

    ~Counter();

    static size_t howMany();

private:
    static size_t count;
};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//
#ifndef DISABLE_INLINE
#include "Counter.icc"
#endif // DISABLE_INLINE

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#include "Counter.cpp"
#endif // EXTERNAL_TEMPLATE_DEFINITION

#endif // Counter_H

// Local Variables:
// mode:C++
// End:











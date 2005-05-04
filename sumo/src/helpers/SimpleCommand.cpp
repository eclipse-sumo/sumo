/***************************************************************************
                          SimpleCommand.cpp  -  Command-pattern-class for
                          simple commands, that need no parameters and no
                          undo.
                             -------------------
    begin                : Tue, 28 Jun 2002
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include "SimpleCommand.h"

#endif // EXTERNAL_TEMPLATE_DEFINITION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


//--------------- DO NOT DEFINE ANYTHING AFTER THIS POINT -------------------//
#ifdef DISABLE_INLINE
#include "SimpleCommand.icc"
#endif


// $Log$
// Revision 1.4  2005/05/04 08:06:19  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.3  2003/02/07 10:40:13  dkrajzew
// updated
//
// Revision 1.2  2002/10/16 16:53:12  dkrajzew
// global inclusion
//
// Revision 1.1  2002/10/16 14:44:45  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.3  2002/06/18 18:29:07  croessel
// Added #ifdef EXTERNAL_TEMPLATE_DEFINITION to prevent multiple inclusions.
//
// Revision 1.2  2002/06/18 17:20:04  croessel
// Added #ifdef EXTERNAL_TEMPLATE_DEFINITION
//
// Revision 1.1  2002/06/18 16:34:27  croessel
// Initial commit. Made files cfront compliant.
//


// Local Variables:
// mode:C++
// End:

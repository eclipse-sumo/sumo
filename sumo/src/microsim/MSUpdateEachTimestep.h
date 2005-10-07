#ifndef MSUPDATEEACHTIMESTEP_H
#define MSUPDATEEACHTIMESTEP_H

/**
 * @file   MSUpdateEachTimestep.h
 * @author Christian Roessel
 * @date   Started Thu Oct 23 16:22:53 2003
 * @version $Id$
 * @brief
 *
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSUpdateEachTimestepContainer.h"

template < class ToUpdate >
class MSUpdateEachTimestep
{
public:
    MSUpdateEachTimestep( void )
        {
            MSUpdateEachTimestepContainer< MSUpdateEachTimestep<
                ToUpdate > >::getInstance()->addItemToUpdate( this );
        }

    virtual ~MSUpdateEachTimestep( void )
        {
            MSUpdateEachTimestepContainer< MSUpdateEachTimestep<
                ToUpdate > >::getInstance()->removeItemToUpdate( this );
        }

    virtual bool updateEachTimestep( void ) = 0;
};

#endif // MSUPDATEEACHTIMESTEP_H

// Local Variables:
// mode:C++
// End:

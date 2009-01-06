/****************************************************************************/
/// @file    MSUpdateEachTimestep.h
/// @author  Christian Roessel
/// @date    Thu Oct 23 16:22:53 2003
/// @version $Id$
///
//	»missingDescription«
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSUpdateEachTimestep_h
#define MSUpdateEachTimestep_h


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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSUpdateEachTimestepContainer.h"

template < class ToUpdate >
class MSUpdateEachTimestep
{
public:
    MSUpdateEachTimestep(void) {
        MSUpdateEachTimestepContainer< MSUpdateEachTimestep<
        ToUpdate > >::getInstance()->addItemToUpdate(this);
    }

    virtual ~MSUpdateEachTimestep(void) {
        MSUpdateEachTimestepContainer< MSUpdateEachTimestep<
        ToUpdate > >::getInstance()->removeItemToUpdate(this);
    }

    virtual bool updateEachTimestep(void) = 0;
};


#endif

/****************************************************************************/


/****************************************************************************/
/// @file    MSUpdateEachTimestepContainer.h
/// @author  Christian Roessel
/// @date    Thu Oct 23 16:38:35 2003
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
#ifndef MSUpdateEachTimestepContainer_h
#define MSUpdateEachTimestepContainer_h


/**
 * @file   MSUpdateEachTimestepContainer.h
 * @author Christian Roessel
 * @date   Started Thu Oct 23 16:38:35 2003
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

#include <vector>
#include <algorithm>

template< class UpdateEachTimestep >
class MSUpdateEachTimestepContainer
{
public:
    static MSUpdateEachTimestepContainer* getInstance(void) {
        if (instanceM == 0) {
            instanceM = new MSUpdateEachTimestepContainer();
        }
        return instanceM;
    }

    void addItemToUpdate(UpdateEachTimestep* item) {
        containerM.push_back(item);
    }

    void removeItemToUpdate(UpdateEachTimestep* item) {
        typename std::vector< UpdateEachTimestep* >::iterator i =
            std::find(containerM.begin(), containerM.end(), item);
        if (i!=containerM.end()) {
            containerM.erase(i);
        }
    }

    void updateAll(void) {
        std::for_each(containerM.begin(), containerM.end(),
                      std::mem_fun(&UpdateEachTimestep::updateEachTimestep));
    }

    ~MSUpdateEachTimestepContainer(void) {
        containerM.clear();
        instanceM = 0;
    }

    void clear() {
        for (typename std::vector< UpdateEachTimestep* >::iterator i=containerM.begin(); i!=containerM.end(); ++i) {
            delete(*i);
        }
        containerM.clear();
    }

private:
    MSUpdateEachTimestepContainer(void)
            : containerM() {}

    std::vector< UpdateEachTimestep* > containerM;

    static MSUpdateEachTimestepContainer* instanceM;
};

// initialize static member
template< class UpdateEachTimestep >
MSUpdateEachTimestepContainer< UpdateEachTimestep >*
MSUpdateEachTimestepContainer< UpdateEachTimestep >::instanceM = 0;


#endif

/****************************************************************************/


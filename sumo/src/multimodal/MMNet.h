/****************************************************************************/
/// @file    MMNet.h
/// @author  unknown_author
/// @date    :vector<MMLayer*>::iterator i=myLayers.begin(); i!=myLayers.end(); ++i) {
/// @version $Id: $
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MMNet_h
#define MMNet_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/StdDefs.h>
#include "MMLayer.h"

class MMNet
{
public:
    MMNet()
    { }

    ~MMNet()
    { }

    void addLayer(MMLayer *l)
    {
        myLayers.push_back(l);
    }

    /// performs a single simulation step
    void simulationStep(SUMOTime start, SUMOTime step)
    {
        for (std::vector<MMLayer*>::iterator i=myLayers.begin(); i!=myLayers.end(); ++i) {
            (*i)->simulationStep(start, step);
        }
    }

protected:
    std::vector<MMLayer*> myLayers;

};


#endif

/****************************************************************************/


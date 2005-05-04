#ifndef GUINetBuilder_h
#define GUINetBuilder_h
//---------------------------------------------------------------------------//
//                        GUINetBuilder.h -
//  Builds the gui-network
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2005/05/04 07:56:43  dkrajzew
// level 3 warnings removed
//
// Revision 1.6  2005/01/27 14:19:35  dkrajzew
// ability to load from a string added
//
// Revision 1.5  2004/07/02 08:38:51  dkrajzew
// changes needed to implement the online-router (class derivation)
//
// Revision 1.4  2003/12/11 06:18:35  dkrajzew
// network loading and initialisation improved
//
// Revision 1.3  2003/09/05 14:56:11  dkrajzew
// first tries for an implementation of aggregated views
//
// Revision 1.2  2003/02/07 10:38:19  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <netload/NLNetBuilder.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class OptionsCont;
class GUINet;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * This is mostly the same as a NLNetBuilder, but allows to build a GUINet
 * instead of a MSNet
 */
class GUINetBuilder : public NLNetBuilder
{
public:
    /// constructor
    GUINetBuilder(const OptionsCont &oc,
        NLEdgeControlBuilder &eb, NLJunctionControlBuilder &jb,
        bool allowAggregatedViews);

    /// destructor
    virtual ~GUINetBuilder();

    /// builds the gui network
    virtual MSNet *buildNetworkFromDescription(MSVehicleControl *vc,
        const std::string &description);

    /// builds the gui network
    virtual MSNet *buildNet(MSVehicleControl *vc);

private:
    /// information whether aggregated views may be used
    bool myAgregatedViewsAllowed;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


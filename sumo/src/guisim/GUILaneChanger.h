#ifndef GUILaneChanger_h
#define GUILaneChanger_h
//---------------------------------------------------------------------------//
//                        GUILaneChanger.h -
//  The gui-version of the lane changer
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
// Revision 1.3  2005/07/12 11:37:41  dkrajzew
// level 3 warnings removed; code style adapted
//
// Revision 1.2  2003/06/05 11:40:28  dkrajzew
// class templates applied; documentation added
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <microsim/MSEdge.h>
#include <microsim/MSLaneChanger.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class GUILaneChanger : public MSLaneChanger {
public:
    /// Destructor.
    ~GUILaneChanger();

    /// Constructor
    GUILaneChanger( MSEdge::LaneCont* lanes );

protected:
    /** @brief sets the lanes aftr changing was done
        This methods makes the same as the methods from MSLaneChanger but locks
        the lanes first */
    void updateLanes();

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


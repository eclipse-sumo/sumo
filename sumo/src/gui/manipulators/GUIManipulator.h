#ifndef GUIManipulator_h
#define GUIManipulator_h
//---------------------------------------------------------------------------//
//                        GUILaneDrawer_FGnT.h -
//  Abstract GUI manipulation class
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.3  2005/04/27 09:44:27  dkrajzew
// level3 warnings removed
//
// Revision 1.2  2004/11/23 10:06:50  dkrajzew
// adapted the new class hierarchy
//
// Revision 1.1  2004/07/02 08:24:33  dkrajzew
// possibility to manipulate vss in the gui added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <fx.h>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIMainWindow;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIManipulator : public FXDialogBox {
    FXDECLARE(GUIManipulator)
public:
    /// Constructor
    GUIManipulator(GUIMainWindow &app, const std::string &name,
        int xpos, int ypos);

    /// Destructor
    virtual ~GUIManipulator();

protected:
    /// FOX needs this
    GUIManipulator() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


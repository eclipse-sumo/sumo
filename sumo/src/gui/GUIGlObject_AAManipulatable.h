#ifndef GUIGlObject_AAManipulatable_h
#define GUIGlObject_AAManipulatable_h
//---------------------------------------------------------------------------//
//                        GUIGlObject_AAManipulatable.h -
//  Base class for all objects that may be displayed within the openGL-gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 12 May 2004
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
// Revision 1.1  2004/07/02 08:27:09  dkrajzew
// possibility to manipulate objects added (preliminary, subject to change)
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "GUIGlObject_AbstractAdd.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIManipulator;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIGlObject_AAManipulatable : public GUIGlObject_AbstractAdd {
public:
    /// Constructor
    GUIGlObject_AAManipulatable(GUIGlObjectStorage &idStorage,
        std::string fullName, GUIGlObjectType type)
        : GUIGlObject_AbstractAdd(idStorage, fullName, type) { }

    /// Constructor for objects joining gl-objects
    GUIGlObject_AAManipulatable(GUIGlObjectStorage &idStorage,
        std::string fullName, size_t glID, GUIGlObjectType type)
         : GUIGlObject_AbstractAdd(idStorage, fullName, glID, type) { }

    /// Destructor
    virtual ~GUIGlObject_AAManipulatable() { }

    virtual GUIManipulator *openManipulator(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent) = 0;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


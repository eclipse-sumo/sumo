#ifndef GUIGlObject_h
#define GUIGlObject_h
//---------------------------------------------------------------------------//
//                        GUIGlObject.h -
//  A class for handling of displayed objects via their numerical id
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Oct 2002
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
// Revision 1.2  2003/04/14 08:24:55  dkrajzew
// unneeded display switch and zooming option removed; new glo-objct concept implemented; comments added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>


 /* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObjectStorage;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class GUIGlObject {
public:
    /// Constructor
    GUIGlObject(GUIGlObjectStorage &idStorage,
        std::string fullName);

    /// Destructor
    virtual ~GUIGlObject();

    /// Returns the full name apperaing in the tool tip
    const std::string &getFullName() const;

    /// Returns the numerical id of the object
    size_t getGlID() const;

    /// Needed to set the id
    friend class GUIGlObjectStorage;

private:
    /// Sets the id of the object
    void setGlID(size_t id);

private:
    /// The numerical id of the object
    size_t myGlID;

    /// The name of the object
    std::string myFullName;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIGlObject.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


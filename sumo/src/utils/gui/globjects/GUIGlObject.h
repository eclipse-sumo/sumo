/****************************************************************************/
/// @file    GUIGlObject.h
/// @author  Daniel Krajzewicz
/// @date    Oct 2002
/// @version $Id: $
///
// Base class for all objects that may be displayed within the openGL-gui
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
#ifndef GUIGlObject_h
#define GUIGlObject_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "GUIGlObjectTypes.h"
#include <utils/geom/Boundary.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObjectStorage;
class GUISUMOAbstractView;
class GUIParameterTableWindow;
class GUIMainWindow;
class GUIGLObjectPopupMenu;


// ===========================================================================
// class definitions
// ===========================================================================
class GUIGlObject
{
public:
    /// Constructor
    GUIGlObject(GUIGlObjectStorage &idStorage,
                std::string fullName);

    /// Constructor for objects joining gl-objects
    GUIGlObject(GUIGlObjectStorage &idStorage,
                std::string fullName, size_t glID);

    /// Destructor
    virtual ~GUIGlObject();

    /// Returns the full name apperaing in the tool tip
    const std::string &getFullName() const;

    /// Returns the numerical id of the object
    size_t getGlID() const;

    /// Needed to set the id
    friend class GUIGlObjectStorage;


    //@{ The following methods needs to be implemented by derived classes
    /// Returns the popup-menu
    virtual GUIGLObjectPopupMenu *getPopUpMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent) = 0;

    /// Returns the parameter window
    virtual GUIParameterTableWindow *getParameterWindow(
        GUIMainWindow &app, GUISUMOAbstractView &parent) = 0;

    /// Returns the type of the object as coded in GUIGlObjectType
    virtual GUIGlObjectType getType() const = 0;

    /// returns the id of the object as known to microsim
    virtual const std::string &microsimID() const = 0;

    /// Returns the information whether this object is still active
    virtual bool active() const = 0;

    /// Returns the boundary to which the object shall be centered
    virtual Boundary getCenteringBoundary() const = 0;
    //@}

protected:
    //@{ Helper methods for building popup-menus
    /// Builds the header
    void buildPopupHeader(GUIGLObjectPopupMenu *ret,
                          GUIMainWindow &app, bool addSeparator=true);

    /// Builds an entry which allows to center to the object
    void buildCenterPopupEntry(GUIGLObjectPopupMenu *ret, bool addSeparator=true);

    /// Builds entries which allow to copy the name / typed name into the clipboard
    void buildNameCopyPopupEntry(GUIGLObjectPopupMenu *ret,
                                 bool addSeparator=true);

    /// Builds an entry which allows to (de)select the object
    void buildSelectionPopupEntry(GUIGLObjectPopupMenu *ret,
                                  bool addSeparator=true);

    /// Builds an entry which allows to open the parameter window
    void buildShowParamsPopupEntry(GUIGLObjectPopupMenu *ret,
                                   bool addSeparator=true);

    /// Builds an entry which allows to open the manipulator window
    void buildShowManipulatorPopupEntry(GUIGLObjectPopupMenu *ret,
                                        bool addSeparator=true);
    //@}

private:
    /// Sets the id of the object
    void setGlID(size_t id);

private:
    /// The numerical id of the object
    size_t myGlID;

    /// The name of the object
    std::string myFullName;

};


#endif

/****************************************************************************/


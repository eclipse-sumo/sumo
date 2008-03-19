/****************************************************************************/
/// @file    GUIGlObject.h
/// @author  Daniel Krajzewicz
/// @date    Oct 2002
/// @version $Id$
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "GUIGlObjectTypes.h"
#include <utils/geom/Boundary.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


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
                std::string fullName) throw();

    /// Constructor for objects joining gl-objects
    GUIGlObject(GUIGlObjectStorage &idStorage,
                std::string fullName, GLuint glID) throw();

    /// Destructor
    virtual ~GUIGlObject() throw();

    /// Returns the full name apperaing in the tool tip
    const std::string &getFullName() const throw();

    /// Returns the numerical id of the object
    GLuint getGlID() const throw();

    /// Needed to set the id
    friend class GUIGlObjectStorage;


    /// @name interfaces to be implemented by derived classes
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     */
    virtual GUIGLObjectPopupMenu *getPopUpMenu(
        GUIMainWindow &app, GUISUMOAbstractView &parent) throw() = 0;


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     */
    virtual GUIParameterTableWindow *getParameterWindow(
        GUIMainWindow &app, GUISUMOAbstractView &parent) throw() = 0;


    /** @brief Returns the id of the object as known to microsim
     *
     * @return The id of the object
     */
    virtual const std::string &microsimID() const throw() = 0;


    /// Returns the type of the object as coded in GUIGlObjectType
    virtual GUIGlObjectType getType() const throw() = 0;


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     */
    virtual Boundary getCenteringBoundary() const throw() = 0;
    //@}


    /** @brief Returns the information whether this object is still active
     *
     * @return Whether this object is active (always true in this case)
     * @see GUIGlObject::active
     */
    virtual bool active() const throw() { return true; }


protected:
    /// @name helper methods for building popup-menus
    //@{

    /// Builds the header
    void buildPopupHeader(GUIGLObjectPopupMenu *ret,
                          GUIMainWindow &app, bool addSeparator=true) throw();

    /// Builds an entry which allows to center to the object
    void buildCenterPopupEntry(GUIGLObjectPopupMenu *ret, bool addSeparator=true) throw();

    /// Builds entries which allow to copy the name / typed name into the clipboard
    void buildNameCopyPopupEntry(GUIGLObjectPopupMenu *ret,
                                 bool addSeparator=true) throw();

    /// Builds an entry which allows to (de)select the object
    void buildSelectionPopupEntry(GUIGLObjectPopupMenu *ret,
                                  bool addSeparator=true) throw();

    /// Builds an entry which allows to open the parameter window
    void buildShowParamsPopupEntry(GUIGLObjectPopupMenu *ret,
                                   bool addSeparator=true) throw();

    /// Builds an entry which allows to open the manipulator window
    void buildShowManipulatorPopupEntry(GUIGLObjectPopupMenu *ret,
                                        bool addSeparator=true) throw();
    //@}

private:
    /// Sets the id of the object
    void setGlID(GLuint id) throw();

private:
    /// The numerical id of the object
    GLuint myGlID;

    /// The name of the object
    std::string myFullName;

};


#endif

/****************************************************************************/


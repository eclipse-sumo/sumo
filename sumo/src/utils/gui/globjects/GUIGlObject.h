/****************************************************************************/
/// @file    GUIGlObject.h
/// @author  Daniel Krajzewicz
/// @date    Oct 2002
/// @version $Id$
///
// Base class for all objects that may be displayed within the openGL-gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
#include <set>
#include "GUIGlObjectTypes.h"
#include <utils/geom/Boundary.h>
#include <utils/common/StdDefs.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObjectStorage;
class GUIParameterTableWindow;
class GUIMainWindow;
class GUIGLObjectPopupMenu;
class GUISUMOAbstractView;
class GUIVisualizationSettings;


// ===========================================================================
// class definitions
// ===========================================================================
class GUIGlObject {
public:
    /** @brief Constructor
     *
     * This is the standard constructor that assures that the object is known
     *  and its id is unique. Use it always :-)
     *
     * @param[in] idStorage The global storage of gl-ids, used to give a unique id
     * @param[in] fullName The complete name, including a type-prefix
     * @see GUIGlObjectStorage
     */
    GUIGlObject(GUIGlObjectStorage &idStorage, std::string fullName) throw();


    /** @brief Constructor
     *
     * This constructor should be used only for compound objects, that share
     *  visualization. Use it only if you know what you are doing.
     *
     * @param[in] fullName The complete name, including a type-prefix
     * @param[in] glID The id of this object
     * @see GUIGlObjectStorage
     */
    GUIGlObject(std::string fullName, GLuint glID) throw();


    /// @brief Destructor
    virtual ~GUIGlObject() throw();



    /// @name Atomar getter methods
    /// @{

    /** @brief Returns the full name appearing in the tool tip
     * @return This object's typed id
     */
    const std::string &getFullName() const throw() {
        return myFullName;
    }


    /** @brief Returns the numerical id of the object
     * @return This object's gl-id
     */
    GLuint getGlID() const throw() {
        return myGlID;
    }
    /// @}



    /// @brief Needed to set the id
    friend class GUIGlObjectStorage;



    /// @name interfaces to be implemented by derived classes
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     */
    virtual GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app, GUISUMOAbstractView &parent) throw() = 0;


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     */
    virtual GUIParameterTableWindow *getParameterWindow(GUIMainWindow &app, GUISUMOAbstractView &parent) throw() = 0;


    /** @brief Returns the id of the object as known to microsim
     *
     * @return The id of the object
     */
    virtual const std::string &getMicrosimID() const throw() = 0;


    /** @brief Returns the type of the object as coded in GUIGlObjectType
     * @return The type of the object
     * @see GUIGlObjectType
     */
    virtual GUIGlObjectType getType() const throw() = 0;


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     */
    virtual Boundary getCenteringBoundary() const throw() = 0;


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     */
    virtual void drawGL(const GUIVisualizationSettings &s) const throw() = 0;
    //@}


    /** @brief Draws additional, user-triggered visualisations
     * @param[in] parent The view
     * @param[in] s The settings for the current view (may influence drawing)
     */
    virtual void drawGLAdditional(GUISUMOAbstractView * const parent, const GUIVisualizationSettings &s) const throw() {
        UNUSED_PARAMETER(&s);
        UNUSED_PARAMETER(parent);
    };



    /// @name Parameter table window I/O
    /// @{

    /** @brief Lets this object know a parameter window showing the object's values was opened
     * @param[in] w The opened parameter window
     */
    void addParameterTable(GUIParameterTableWindow *w) throw();


    /** @brief Lets this object know a parameter window showing the object's values was closed
     * @param[in] w The closed parameter window
     */
    void removeParameterTable(GUIParameterTableWindow *w) throw();
    /// @}



protected:
    /// @name helper methods for building popup-menus
    //@{

    /** @brief Builds the header
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildPopupHeader(GUIGLObjectPopupMenu *ret, GUIMainWindow &app, bool addSeparator=true) throw();


    /** @brief Builds an entry which allows to center to the object
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildCenterPopupEntry(GUIGLObjectPopupMenu *ret, bool addSeparator=true) throw();


    /** @brief Builds entries which allow to copy the name / typed name into the clipboard
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildNameCopyPopupEntry(GUIGLObjectPopupMenu *ret, bool addSeparator=true) throw();


    /** @brief Builds an entry which allows to (de)select the object
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildSelectionPopupEntry(GUIGLObjectPopupMenu *ret, bool addSeparator=true) throw();


    /** @brief Builds an entry which allows to open the parameter window
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildShowParamsPopupEntry(GUIGLObjectPopupMenu *ret, bool addSeparator=true) throw();


    /** @brief Builds an entry which allows to copy the cursor position
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildPositionCopyEntry(GUIGLObjectPopupMenu *ret, bool addSeparator=true) throw();


    /** @brief Builds an entry which allows to open the manipulator window
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildShowManipulatorPopupEntry(GUIGLObjectPopupMenu *ret, bool addSeparator=true) throw();
    //@}


private:
    /** @brief Sets the id of the object
     *
     * May not be done within the constructor in the case of compund objects
     * @param[in] id The (new) id of the object
     */
    void setGlID(GLuint id) throw();


private:
    /// @brief The numerical id of the object
    GLuint myGlID;

    /// @brief The name of the object
    std::string myFullName;

    /// @brief Parameter table windows which refer to this object
    std::set<GUIParameterTableWindow*> myParamWindows;


};


#endif

/****************************************************************************/


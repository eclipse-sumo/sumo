/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIGlObject.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Oct 2002
///
// Base class for all objects that may be displayed within the openGL-gui
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <set>

#include <utils/geom/Boundary.h>
#include <utils/common/StdDefs.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringBijection.h>
#include <utils/common/RGBColor.h>
#include <utils/foxtools/fxheader.h>

#include "GUIGlObjectTypes.h"


// ===========================================================================
// definitions
// ===========================================================================

typedef unsigned int GUIGlID;

// ===========================================================================
// class declarations
// ===========================================================================

class GUIGlObjectStorage;
class GUIParameterTableWindow;
class GUIMainWindow;
class GUIGLObjectPopupMenu;
class GUISUMOAbstractView;
class GUIVisualizationSettings;
struct GUIVisualizationTextSettings;

#ifdef HAVE_OSG
namespace osg {
class Node;
}
#endif

// ===========================================================================
// class definitions
// ===========================================================================

class GUIGlObject {

public:
    /// @brief associates object types with strings
    static StringBijection<GUIGlObjectType> TypeNames;
    static const GUIGlID INVALID_ID;
    static const double INVALID_PRIORITY;

    /** @brief Constructor
     *
     * This is the standard constructor that assures that the object is known
     *  and its id is unique. Use it always :-)
     *
     * @param[in] type The GUIGlObjectType type
     * @param[in] microsimID unique ID
     * @param[in] icon optional icon associated with this GUIGLObject
     * @see GUIGlObjectStorage
     */
    GUIGlObject(GUIGlObjectType type, const std::string& microsimID, FXIcon* icon);

    /// @brief Destructor
    virtual ~GUIGlObject();

    /// @name getter methods
    /// @{
    /// @brief Returns the full name appearing in the tool tip
    /// @return This object's typed id
    inline const std::string& getFullName() const {
        return myFullName;
    }

    /// @brief Returns the name of the parent object (if any)
    /// @return This object's parent id
    virtual std::string getParentName() const;

    /// @brief Returns the numerical id of the object
    /// @return This object's gl-id
    inline GUIGlID getGlID() const {
        return myGlID;
    }

    /// @brief get icon associated with this GL Object
    FXIcon* getGLIcon() const;

    /// @}

    /// @name interfaces to be implemented by derived classes
    /// @{
    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     */
    virtual GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) = 0;

    /// @brief notify object about popup menu removal
    virtual void removedPopupMenu() {}

    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     */
    virtual GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) = 0;

    /** @brief Returns an own type parameter window (optional)
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     */
    virtual GUIParameterTableWindow* getTypeParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /// @brief Returns the id of the object as known to microsim
    inline const std::string& getMicrosimID() const {
        return myMicrosimID;
    }

    /// @brief Returns the name of the object (default "")
    virtual const std::string getOptionalName() const;

    /// @brief Changes the microsimID of the object
    /// @note happens in netedit
    virtual void setMicrosimID(const std::string& newID);

    /// @brief Returns the type of the object as coded in GUIGlObjectType
    /// @see GUIGlObjectType
    inline GUIGlObjectType getType() const {
        return myGLObjectType;
    }

    /// @brief Returns the priority of receiving mouse clicks
    virtual double getClickPriority() const {
        return (double)myGLObjectType;
    }

    /// @brief get blocking status
    inline bool isBlocked() const {
        return myAmBlocked;
    }

    /// @brief set blocking status
    inline void setBlocked(const bool state = true) {
        myAmBlocked = state;
    }

    /// @brief return exaggeration associated with this GLObject
    virtual double getExaggeration(const GUIVisualizationSettings& s) const {
        UNUSED_PARAMETER(s);
        return 1.;
    }

    //// @brief Returns the boundary to which the view shall be centered in order to show the object
    virtual Boundary getCenteringBoundary() const = 0;

    /// @brief Draws the object
    /// @param[in] s The settings for the current view (may influence drawing)
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;

    /// @brief check if element is locked (Currently used only in netedit)
    virtual bool isGLObjectLocked() const;

    /// @brief mark element as front element (Currently used only in netedit)
    virtual void markAsFrontElement();

    /// @brief delete GLObject (Currently used only in netedit)
    virtual void deleteGLObject();

    /// @brief select GLObject (Currently used only in netedit)
    virtual void selectGLObject();

    /// @brief update GLObject (geometry, ID, etc.) (optional)
    virtual void updateGLObject();

    virtual double getColorValue(const GUIVisualizationSettings& /*s*/, int /*activeScheme*/) const {
        return 0;
    }
    /// @}

    /** @brief Draws additional, user-triggered visualisations
     * @param[in] parent The view
     * @param[in] s The settings for the current view (may influence drawing)
     */
    virtual void drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const;

    /// @brief remove additional user-griggered visualisations
    virtual void removeActiveAddVisualisation(GUISUMOAbstractView* const /*parent*/, int /*which*/) {}

    /// @brief notify object about left click
    virtual void onLeftBtnPress(void* /*data*/) {}

#ifdef HAVE_OSG
    /// @brief get OSG Node
    osg::Node* getNode() const;

    /// @brief set OSG Node
    void setNode(osg::Node* node);
#endif

    /// @name Parameter table window I/O
    /// @{
    /// @brief Lets this object know a parameter window showing the object's values was opened
    /// @param[in] w The opened parameter window
    void addParameterTable(GUIParameterTableWindow* w);

    /// @brief Lets this object know a parameter window showing the object's values was closed
    /// @param[in] w The closed parameter window
    void removeParameterTable(GUIParameterTableWindow* w);
    /// @}

    /// @brief draw name of item
    void drawName(const Position& pos, const double scale, const GUIVisualizationTextSettings& settings, const double angle = 0, bool forceShow = false) const;

protected:
    /// @name helper methods for building popup-menus
    /// @{
    /** @brief Builds the header
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildPopupHeader(GUIGLObjectPopupMenu* ret, GUIMainWindow& app, bool addSeparator = true);

    /** @brief Builds an entry which allows to center to the object
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildCenterPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator = true);

    /** @brief Builds entries which allow to copy the name / typed name into the clipboard
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildNameCopyPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator = true);

    /** @brief Builds an entry which allows to (de)select the object
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildSelectionPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator = true);

    /** @brief Builds an entry which allows to open the parameter window
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildShowParamsPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator = true);

    /** @brief Builds an entry which allows to open the type parameter window
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildShowTypeParamsPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator = true);

    /** @brief Builds an entry which allows to copy the cursor position
     *   if geo projection is used, also builds an entry for copying the geo-position
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildPositionCopyEntry(GUIGLObjectPopupMenu* ret, const GUIMainWindow& app) const;

    /** @brief Builds an entry which allows to open the manipulator window
     * @param[in, filled] ret The popup menu to add the entry to
     * @param[in] addSeparator Whether a separator shall be added, too
     */
    void buildShowManipulatorPopupEntry(GUIGLObjectPopupMenu* ret, bool addSeparator = true);
    /// @}

    /// @brief build basic shape popup options. Used to unify pop-ups menu in netedit and SUMO-GUI
    void buildShapePopupOptions(GUIMainWindow& app, GUIGLObjectPopupMenu* ret, const std::string& type);

    /// @brief build basic additional popup options. Used to unify pop-ups menu in netedit and SUMO-GUI
    void buildAdditionalsPopupOptions(GUIMainWindow& app, GUIGLObjectPopupMenu* ret, const std::string& type);

private:
    /// @brief The numerical id of the object
    const GUIGlID myGlID;

    /// @brief The type of the object
    const GUIGlObjectType myGLObjectType;

    /// @brief ID of GL object
    std::string myMicrosimID;

    /// @brief full name of GL Object
    std::string myFullName;

    /// @brief icon associatd with this GL Object
    FXIcon* myIcon;

    /// @brief whether the object can be deleted
    bool myAmBlocked = false;

    /// @brief Parameter table windows which refer to this object
    std::set<GUIParameterTableWindow*> myParamWindows;

#ifdef HAVE_OSG
    /// @brief OSG Node of this GL object
    osg::Node* myOSGNode = nullptr;
#endif

    /// @brief create full name
    std::string createFullName() const;

    /// @brief vector for TypeNames Initializer
    static StringBijection<GUIGlObjectType>::Entry GUIGlObjectTypeNamesInitializer[];

    /// @brief Invalidated copy constructor.
    GUIGlObject(const GUIGlObject&) = delete;

    /// @brief Invalidated assignment operator.
    GUIGlObject& operator=(const GUIGlObject&) = delete;
};

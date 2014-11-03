/****************************************************************************/
/// @file    GUIPerson.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIPerson_h
#define GUIPerson_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <set>
#include <string>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/common/RGBColor.h>
#include <microsim/MSPerson.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/foxtools/MFXMutex.h>
#include <utils/gui/settings/GUIColorer.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;
class GUIGLObjectPopupMenu;
class MSDevice_Vehroutes;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIPerson
 */
class GUIPerson : public MSPerson, public GUIGlObject {
public:
    /** @brief Constructor
     */
    GUIPerson(const SUMOVehicleParameter* pars, const MSVehicleType* vtype, MSPerson::MSPersonPlan* plan);


    /// @brief destructor
    ~GUIPerson();


    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;



    /** @brief Draws additionally triggered visualisations
     * @param[in] parent The view
     * @param[in] s The settings for the current view (may influence drawing)
     */
    virtual void drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const;
    //@}

    /* @brief set the position of a person while riding in a vehicle
     * @note This must be called by the vehicle before the call to drawGl */
    void setPositionInVehicle(const Position& pos) {
        myPositionInVehicle = pos;
    }

    /// @name inherited from MSPerson with added locking
    //@{

    /// @brief return the offset from the start of the current edge
    SUMOReal getEdgePos() const;

    /// @brief return the Network coordinate of the person
    // @note overrides the base method and returns myPositionInVehicle while in driving stage
    Position getPosition() const;

    /// @brief return the current angle of the person
    SUMOReal getAngle() const;

    /// @brief the time this person spent waiting in seconds
    SUMOReal getWaitingSeconds() const;

    /// @brief the current speed of the person
    SUMOReal getSpeed() const;

    //@}


    /**
     * @class GUIPersonPopupMenu
     *
     * A popup-menu for vehicles. In comparison to the normal popup-menu, this one
     *  also allows to trigger further visualisations and to track the vehicle.
     */
    class GUIPersonPopupMenu : public GUIGLObjectPopupMenu {
        //FXDECLARE(GUIPersonPopupMenu)
    public:
        /** @brief Constructor
         * @param[in] app The main window for instantiation of other windows
         * @param[in] parent The parent view for changing it
         * @param[in] o The object of interest
         * @param[in, out] additionalVisualizations Information which additional visualisations are enabled (per view)
         */
        GUIPersonPopupMenu(GUIMainWindow& app,
                           GUISUMOAbstractView& parent, GUIGlObject& o, std::map<GUISUMOAbstractView*, int>& additionalVisualizations);

        /// @brief Destructor
        ~GUIPersonPopupMenu();


    protected:
        /// @brief Information which additional visualisations are enabled (per view)
        std::map<GUISUMOAbstractView*, int>& myVehiclesAdditionalVisualizations;
        /// @brief Needed for parameterless instantiation
        std::map<GUISUMOAbstractView*, int> dummy;

    protected:
        /// @brief default constructor needed by FOX
        GUIPersonPopupMenu() : myVehiclesAdditionalVisualizations(dummy) { }

    };



    /// @brief Enabled visualisations, per view
    std::map<GUISUMOAbstractView*, int> myAdditionalVisualizations;




private:
    /// The mutex used to avoid concurrent updates of the vehicle buffer
    mutable MFXMutex myLock;

    /// The position of a person while riding a vehicle
    Position myPositionInVehicle;

    /// @brief sets the color according to the currente settings
    void setColor(const GUIVisualizationSettings& s) const;

    /// @brief gets the color value according to the current scheme index
    SUMOReal getColorValue(size_t activeScheme) const;

    /// @brief sets the color according to the current scheme index and some vehicle function
    bool setFunctionalColor(size_t activeScheme) const;

    /// @name drawing helper methods
    /// @{
    void drawAction_drawAsTriangle(const GUIVisualizationSettings& s) const;
    void drawAction_drawAsPoly(const GUIVisualizationSettings& s) const;
    void drawAction_drawAsImage(const GUIVisualizationSettings& s) const;
    /// @}
};


#endif

/****************************************************************************/


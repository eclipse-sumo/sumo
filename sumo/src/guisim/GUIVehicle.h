/****************************************************************************/
/// @file    GUIVehicle.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIVehicle_h
#define GUIVehicle_h


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
#include <utils/geom/PositionVector.h>
#include <microsim/MSVehicle.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/foxtools/MFXMutex.h>
#include <utils/gui/settings/GUIColorer.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;
class GUIGLObjectPopupMenu;
class MSDevice_Vehroutes;
#ifdef HAVE_OSG
class GUIOSGView;
namespace osg {
class ShapeDrawable;
}
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIVehicle
 * @brief A MSVehicle extended by some values for usage within the gui
 *
 * A visualisable MSVehicle. Extended by the possibility to retrieve names
 * of all available vehicles (static) and the possibility to retrieve the
 * color of the vehicle which is available in different forms allowing an
 * easier recognition of done actions such as lane changing.
 */
class GUIVehicle : public MSVehicle, public GUIGlObject {
public:
    /** @brief Constructor
     * @param[in] pars The vehicle description
     * @param[in] route The vehicle's route
     * @param[in] type The vehicle's type
     * @param[in] speedFactor The factor for driven lane's speed limits
     * @param[in] vehicleIndex The vehicle's running index
     * @exception ProcessError If a value is wrong
     */
    GUIVehicle(SUMOVehicleParameter* pars, const MSRoute* route,
               const MSVehicleType* type, SUMOReal speedFactor, int vehicleIndex);


    /// @brief destructor
    ~GUIVehicle();


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



    /// @name Additional visualisations
    /// @{

    /** @brief Returns whether the named feature is enabled in the given view
     * @param[in] parent The view for which the feature may be enabled
     * @param[in] which The visualisation feature
     * @return see comment
     */
    bool hasActiveAddVisualisation(GUISUMOAbstractView* const parent, int which) const;


    /** @brief Adds the named visualisation feature to the given view
     * @param[in] parent The view for which the feature shall be enabled
     * @param[in] which The visualisation feature to enable
     * @see GUISUMOAbstractView::addAdditionalGLVisualisation
     */
    void addActiveAddVisualisation(GUISUMOAbstractView* const parent, int which);


    /** @brief Adds the named visualisation feature to the given view
     * @param[in] parent The view for which the feature shall be enabled
     * @param[in] which The visualisation feature to enable
     * @see GUISUMOAbstractView::removeAdditionalGLVisualisation
     */
    void removeActiveAddVisualisation(GUISUMOAbstractView* const parent, int which);
    /// @}



    /** @brief Returns the time since the last lane change in seconds
     * @see MSVehicle::myLastLaneChangeOffset
     * @return The time since the last lane change in seconds
     */
    SUMOReal getLastLaneChangeOffset() const;


    /** @brief Returns the description of best lanes to use in order to continue the route
     *
     * Prevents parallel reading and generation of the information by locking
     *  "myLock" before calling MSVehicle::getBestLanes.
     * @return The best lanes structure holding matching the current vehicle position and state ahead
     * @see MSVehicle::getBestLanes
     */
    const std::vector<LaneQ>& getBestLanes() const;


    /**
     * @class GUIVehiclePopupMenu
     *
     * A popup-menu for vehicles. In comparison to the normal popup-menu, this one
     *  also allows to trigger further visualisations and to track the vehicle.
     */
    class GUIVehiclePopupMenu : public GUIGLObjectPopupMenu {
        FXDECLARE(GUIVehiclePopupMenu)
    public:
        /** @brief Constructor
         * @param[in] app The main window for instantiation of other windows
         * @param[in] parent The parent view for changing it
         * @param[in] o The object of interest
         * @param[in, out] additionalVisualizations Information which additional visualisations are enabled (per view)
         */
        GUIVehiclePopupMenu(GUIMainWindow& app,
                            GUISUMOAbstractView& parent, GUIGlObject& o, std::map<GUISUMOAbstractView*, int>& additionalVisualizations);

        /// @brief Destructor
        ~GUIVehiclePopupMenu();

        /// @brief Called if all routes of the vehicle shall be shown
        long onCmdShowAllRoutes(FXObject*, FXSelector, void*);
        /// @brief Called if all routes of the vehicle shall be hidden
        long onCmdHideAllRoutes(FXObject*, FXSelector, void*);
        /// @brief Called if the current route of the vehicle shall be shown
        long onCmdShowCurrentRoute(FXObject*, FXSelector, void*);
        /// @brief Called if the current route of the vehicle shall be hidden
        long onCmdHideCurrentRoute(FXObject*, FXSelector, void*);
        /// @brief Called if the vehicle's best lanes shall be shown
        long onCmdShowBestLanes(FXObject*, FXSelector, void*);
        /// @brief Called if the vehicle's best lanes shall be hidden
        long onCmdHideBestLanes(FXObject*, FXSelector, void*);
        /// @brief Called if the vehicle shall be tracked
        long onCmdStartTrack(FXObject*, FXSelector, void*);
        /// @brief Called if the current shall not be tracked any longer
        long onCmdStopTrack(FXObject*, FXSelector, void*);
        /// @brief Called if all routes of the vehicle shall be shown
        long onCmdShowLFLinkItems(FXObject*, FXSelector, void*);
        /// @brief Called if all routes of the vehicle shall be hidden
        long onCmdHideLFLinkItems(FXObject*, FXSelector, void*);
        /// @brief Called when show a vehicles foes
        long onCmdShowFoes(FXObject*, FXSelector, void*);

    protected:
        /// @brief Information which additional visualisations are enabled (per view)
        std::map<GUISUMOAbstractView*, int>& myVehiclesAdditionalVisualizations;
        /// @brief Needed for parameterless instantiation
        std::map<GUISUMOAbstractView*, int> dummy;

    protected:
        /// @brief default constructor needed by FOX
        GUIVehiclePopupMenu() : myVehiclesAdditionalVisualizations(dummy) { }

    };


    /// @name Additional visualisations
    /// @{

    /** @brief Additional visualisation feature ids
     */
    enum VisualisationFeatures {
        /// @brief show vehicle's best lanes
        VO_SHOW_BEST_LANES = 1,
        /// @brief show vehicle's current route
        VO_SHOW_ROUTE = 2,
        /// @brief show all vehicle's routes
        VO_SHOW_ALL_ROUTES = 4,
        /// @brief track vehicle
        VO_TRACKED = 8,
        /// @brief LFLinkItems
        VO_SHOW_LFLINKITEMS = 16
    };

    /// @brief Enabled visualisations, per view
    std::map<GUISUMOAbstractView*, int> myAdditionalVisualizations;


    /** @brief Draws the route
     * @param[in] r The route to draw
     */
    void drawRouteHelper(const MSRoute& r, SUMOReal exaggeration) const;


    /** @brief Chooses the route to draw and draws it, darkening it as given
     * @param[in] s The visualisation settings, needed to determine the vehicle's color
     * @param[in] routeNo The route to show (0: the current, >0: prior)
     * @param[in] darken The amount to darken the route by
     */
    void drawRoute(const GUIVisualizationSettings& s, int routeNo, SUMOReal darken) const;


    /** @brief Draws the vehicle's best lanes
     */
    void drawBestLanes() const;
    /// @}

    /// @brief adds the blocking foes to the current selection
    void selectBlockingFoes() const;

#ifdef HAVE_OSG
    void setGeometry(GUIOSGView* view, osg::ShapeDrawable* geom) {
        myGeom[view] = geom;
    }

    void updateColor(GUIOSGView* view);
#endif


private:
    /// @brief sets the color according to the currente settings
    void setColor(const GUIVisualizationSettings& s) const;

    /// @brief gets the color value according to the current scheme index
    SUMOReal getColorValue(size_t activeScheme) const;

    /// @brief sets the color according to the current scheme index and some vehicle function
    bool setFunctionalColor(size_t activeScheme) const;

    /// @name drawing helper methods
    /// @{
    static void drawPoly(double* poses, SUMOReal offset);

    void drawAction_drawVehicleAsBoxPlus() const;
    void drawAction_drawVehicleAsTrianglePlus() const;
    void drawAction_drawVehicleAsPoly(const GUIVisualizationSettings& s) const;

    /* @brief try to draw vehicle as raster image and return true if sucessful
     * @param[in] length The custom length of the vehicle
     *   (defaults to the * length specified in the vehicle type if -1 is passed)
    */
    bool drawAction_drawVehicleAsImage(const GUIVisualizationSettings& s, SUMOReal length = -1) const;

    /* @brief draw train with individual carriages. The number of carriages is
     * determined from defaultLength of carriages and vehicle length
     * passengerSeats are computed beginning at firstPassengerCarriage */
    void drawAction_drawRailCarriages(const GUIVisualizationSettings& s, SUMOReal defaultLength, int firstPassengerCarriage = 0, bool asImage = false) const;
    /// @}

    /* @brief return the previous lane in this vehicles route including internal lanes
     * @param[in] current The lane of which the predecessor should be returned
     * @param[in,out] routeIndex The index of the current or previous non-internal edge in the route
     */
    MSLane* getPreviousLane(MSLane* current, int& routeIndex) const;

    /// @brief returns the seat position for the person with the given index
    const Position& getSeatPosition(size_t personIndex) const;

    /// @brief return the number of passengers
    int getNumPassengers() const;

    /// @brief add seats to mySeatPositions and update requiredSeats
    void computeSeats(const Position& front, const Position& back, int& requiredSeats) const;

    /// @brief retrieve information about the current stop state
    std::string getStopInfo() const;

    static void drawLinkItem(const Position& pos, SUMOTime arrivalTime, SUMOTime leaveTime, SUMOReal exagerate);

private:
    /// The mutex used to avoid concurrent updates of the vehicle buffer
    mutable MFXMutex myLock;

    MSDevice_Vehroutes* myRoutes;

    /// @brief positions of seats in the vehicle (updated at every drawing step)
    mutable PositionVector mySeatPositions;

#ifdef HAVE_OSG
    std::map<GUIOSGView*, osg::ShapeDrawable*> myGeom;
#endif

};


#endif

/****************************************************************************/


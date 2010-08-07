/****************************************************************************/
/// @file    GUIVehicle.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A MSVehicle extended by some values for usage within the gui
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

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <vector>
#include <set>
#include <string>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/common/RGBColor.h>
#include <microsim/MSVehicle.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/foxtools/MFXMutex.h>
#include <utils/gui/settings/GUIColorer.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;
class GUIGLObjectPopupMenu;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIVehicle
 *
 * A visualisable MSVehicle. Extended by the possibility to retrieve names
 * of all available vehicles (static) and the possibility to retrieve the
 * color of the vehicle which is available in different forms allowing an
 * easier recognition of done actions such as lane changing.
 */
class GUIVehicle : public MSVehicle, public GUIGlObject {
public:
    /// Use this constructor only.
    GUIVehicle(GUIGlObjectStorage &idStorage,
               SUMOVehicleParameter* pars, const MSRoute* route,
               const MSVehicleType* type,
               int vehicleIndex) throw(ProcessError);

    /// destructor
    ~GUIVehicle() throw();

    inline void setOwnDefinedColor() const {
        const RGBColor &col = myParameter->color;
        glColor3d(col.red(), col.green(), col.blue());
    }

    inline void setOwnTypeColor() const {
        const RGBColor &col = myType->getColor();
        glColor3d(col.red(), col.green(), col.blue());
    }

    inline void setOwnRouteColor() const {
        const RGBColor &col = myRoute->getColor();
        glColor3d(col.red(), col.green(), col.blue());
    }




    /// @name inherited from GUIGlObject
    //@{

    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu *getPopUpMenu(GUIMainWindow &app,
                                       GUISUMOAbstractView &parent) throw();


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow *getParameterWindow(
        GUIMainWindow &app, GUISUMOAbstractView &parent) throw();


    /** @brief Returns the id of the object as known to microsim
     *
     * @return The id of the vehicle
     * @see GUIGlObject::getMicrosimID
     */
    const std::string &getMicrosimID() const throw();


    /** @brief Returns the type of the object as coded in GUIGlObjectType
     *
     * @return GLO_EDGE (is an edge)
     * @see GUIGlObject::getType
     * @see GUIGlObjectType
     */
    GUIGlObjectType getType() const throw() {
        return GLO_VEHICLE;
    }


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const throw();


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings &s) const throw();


    /** @brief Returns the information whether this object is still active
     *
     * @return Whether this vehicle is active (has not left the simulation)
     * @see GUIGlObject::active
     */
    bool active() const throw();


    /** @brief Draws additionally triggered visualisations
     * @param[in] parent The view
     * @param[in] s The settings for the current view (may influence drawing)
     */
	virtual void drawGLAdditional(GUISUMOAbstractView * const parent, const GUIVisualizationSettings &s) const throw();
    //@}


    void setRemoved();



	/// @name Additional visualisations
	/// @{
	
	/** @brief Returns whether the named feature is enabled in the given view
	 * @param[in] parent The view for which the feature may be enabled
	 * @param[in] which The visualisation feature
	 * @return see comment
	 */
	bool hasActiveAddVisualisation(GUISUMOAbstractView * const parent, int which) const throw();


	/** @brief Adds the named visualisation feature to the given view
	 * @param[in] parent The view for which the feature shall be enabled
	 * @param[in] which The visualisation feature to enable
	 * @return Always true
	 * @see GUISUMOAbstractView::addAdditionalGLVisualisation
	 */
	bool addActiveAddVisualisation(GUISUMOAbstractView * const parent, int which) throw();


	/** @brief Adds the named visualisation feature to the given view
	 * @param[in] parent The view for which the feature shall be enabled
	 * @param[in] which The visualisation feature to enable
	 * @return Whether the vehicle was known to the view
	 * @see GUISUMOAbstractView::removeAdditionalGLVisualisation
	 */
	bool removeActiveAddVisualisation(GUISUMOAbstractView * const parent, int which) throw();
	/// @}



    /** @brief Returns the time since the last lane change
     * @see MSVehicle::myLastLaneChangeOffset
     * @return The time since the last lane change
     */
    SUMOReal getLastLaneChangeOffset() const throw() {
        return myLastLaneChangeOffset;
    }


    /** @brief Returns the description of best lanes to use in order to continue the route
     *
     * Prevents parallel reading and generation of the information by locking
     *  "myLock" before calling MSVehicle::getBestLanes.
     * @return The best lanes structure holding matching the current vehicle position and state ahead
     * @see MSVehicle::getBestLanes
     */
    const std::vector<LaneQ> &getBestLanes() const throw();

    class Colorer : public GUIColorer<GUIVehicle> {
    public:
        Colorer();
        SUMOReal getColorValue(const GUIVehicle& vehicle) const;
        bool setFunctionalColor(const GUIVehicle& vehicle) const;
    };

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
        GUIVehiclePopupMenu(GUIMainWindow &app,
                            GUISUMOAbstractView &parent, GUIGlObject &o, std::map<GUISUMOAbstractView*, int> &additionalVisualizations);

        /// @brief Destructor
        ~GUIVehiclePopupMenu() throw();

        /// @brief Called if all routes of the vehicle shall be shown
        long onCmdShowAllRoutes(FXObject*,FXSelector,void*);
        /// @brief Called if all routes of the vehicle shall be hidden
        long onCmdHideAllRoutes(FXObject*,FXSelector,void*);
        /// @brief Called if the current route of the vehicle shall be shown
        long onCmdShowCurrentRoute(FXObject*,FXSelector,void*);
        /// @brief Called if the current route of the vehicle shall be hidden
        long onCmdHideCurrentRoute(FXObject*,FXSelector,void*);
        /// @brief Called if the vehicle's best lanes shall be shown
        long onCmdShowBestLanes(FXObject*,FXSelector,void*);
        /// @brief Called if the vehicle's best lanes shall be hidden
        long onCmdHideBestLanes(FXObject*,FXSelector,void*);
        /// @brief Called if the vehicle shall be tracked
        long onCmdStartTrack(FXObject*,FXSelector,void*);
        /// @brief Called if the current shall not be tracked any longer
        long onCmdStopTrack(FXObject*,FXSelector,void*);

	protected:
		/// @brief Information which additional visualisations are enabled (per view)
		std::map<GUISUMOAbstractView*, int> &myVehiclesAdditionalVisualizations;
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
        VO_SHOW_ALL_ROUTES = 4
    };

	/// @brief Enabled visualisations, per view
	std::map<GUISUMOAbstractView*, int> myAdditionalVisualizations;


	/** @brief Draws the route
	 * @param[in] r The route to draw
	 */
	void draw(const MSRoute &r) const throw();


	/** @brief Chooses the route to draw and draws it, darkening it as given
	 * @param[in] s The visualisation settings, needed to determine the vehicle's color
	 * @param[in] routeNo The route to show (0: the current, >0: prior)
	 * @param[in] darken The amount to darken the route by
	 */
	void drawRoute(const GUIVisualizationSettings &s, int routeNo, SUMOReal darken) const throw();


	/** @brief Draws the vehicle's best lanes
	 */
	void drawBestLanes() const throw();
	/// @}


private:
    /// The mutex used to avoid concurrent updates of the vehicle buffer
    mutable MFXMutex myLock;

};


#endif

/****************************************************************************/


/****************************************************************************/
/// @file    GUIOSGView.h
/// @author  Daniel Krajzewicz
/// @date    19.01.2012
/// @version $Id$
///
// An OSG-based 3D view on the simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIOSGView_h
#define GUIOSGView_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_OSG

#include <string>
#include <osgGA/TerrainManipulator>
#include <osgViewer/Viewer>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>
#include <utils/shapes/Polygon.h>
#include <gui/GUISUMOViewParent.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUINet;
class GUISUMOViewParent;
class GUIVehicle;
class GUILaneWrapper;
class MSRoute;
namespace osgGA {
class CameraManipulator;
class NodeTrackerManipulator;
}


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIOSGView
 * @brief An OSG-based 3D view on the simulation
 */
class GUIOSGView : public GUISUMOAbstractView {
    FXDECLARE(GUIOSGView)
public:
    /**
     * @class Command_TLSChange
     * @brief Updates scene on each tls switch
     */
    class Command_TLSChange : public MSTLLogicControl::OnSwitchAction {
    public:
        /** @brief Constructor
         *
         * @param[in] link   The link to observe
         * @param[in] root   the root of the scene
         * @param[in] green  the green light
         * @param[in] yellow the yellow light
         * @param[in] red    the red light
         */
        Command_TLSChange(const MSLink* const link, osg::Switch* switchNode);


        /// @brief Destructor
        virtual ~Command_TLSChange();


        /** @brief Executes the command
         *
         * Called when an active tls program switches.
         *  If the state of the observed linkchanged, this method removes
         *  the old traffic light and adds a new one.
         */
        void execute();


    private:
        /// @brief The link to observe
        const MSLink* const myLink;

        /// @brief The switch for the traffic light models
        osg::ref_ptr<osg::Switch> mySwitch;

        /// @brief The previous link state
        LinkState myLastState;


    private:
        /// @brief Invalidated copy constructor.
        Command_TLSChange(const Command_TLSChange&);

        /// @brief Invalidated assignment operator.
        Command_TLSChange& operator=(const Command_TLSChange&);

    };


    struct OSGMovable {
        osg::ref_ptr<osg::PositionAttitudeTransform> pos;
        osg::ref_ptr<osg::ShapeDrawable> geom;
        osg::ref_ptr<osg::Switch> lights;
    };


    /// constructor
    GUIOSGView(FXComposite* p, GUIMainWindow& app,
               GUISUMOViewParent* parent, GUINet& net, FXGLVisual* glVis,
               FXGLCanvas* share);

    virtual ~GUIOSGView();

    /// builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow&);

    /// recenters the view
    void recenterView();

    /** @brief centers to the chosen artifact
     * @param[in] id The id of the artifact to center to
     * @param[in] applyZoom Whether to zoom in
     * @param[in] zoomDist The distance in m to use for the zoom, values < 0 means: use the centeringBoundary
     * @note caller is responsible for calling update
     */
    void centerTo(GUIGlID id, bool applyZoom, SUMOReal zoomDist = 20);

    void showViewportEditor();

    /// applies the given viewport settings
    void setViewportFromTo(const Position& lookFrom, const Position& lookAt);

    ///@brief copy the viewport to the given view
    void copyViewportTo(GUISUMOAbstractView* view);

    /** @brief Starts vehicle tracking
     * @param[in] id The glID of the vehicle to track
     */
    void startTrack(int id);


    /** @brief Stops vehicle tracking
     */
    void stopTrack();


    /** @brief Returns the id of the tracked vehicle (-1 if none)
     * @return The glID of the vehicle to track
     */
    GUIGlID getTrackedID() const;

    bool setColorScheme(const std::string& name);

    /// @brief handle mouse click in gaming mode
    void onGamingClick(Position pos);

    /// @brief get the current simulation time
    SUMOTime getCurrentTimeStep() const;

    void remove(GUIVehicle* veh);

    // callback
    long onConfigure(FXObject*, FXSelector, void*);
    long onKeyPress(FXObject*, FXSelector, void*);
    long onKeyRelease(FXObject*, FXSelector, void*);
    long onLeftBtnPress(FXObject*, FXSelector, void*);
    long onLeftBtnRelease(FXObject*, FXSelector, void*);
    long onMiddleBtnPress(FXObject*, FXSelector, void*);
    long onMiddleBtnRelease(FXObject*, FXSelector, void*);
    long onRightBtnPress(FXObject*, FXSelector, void*);
    long onRightBtnRelease(FXObject*, FXSelector, void*);
    //long onMotion(FXObject*, FXSelector, void*);
    long onMouseMove(FXObject*, FXSelector, void*);
    long onPaint(FXObject*, FXSelector, void*);
    long OnIdle(FXObject* sender, FXSelector sel, void* ptr);

private:
    class SUMOTerrainManipulator : public osgGA::TerrainManipulator {
    public:
        SUMOTerrainManipulator() {
            setAllowThrow(false);
        }
        bool performMovementLeftMouseButton(const double eventTimeDelta, const double dx, const double dy) {
            return osgGA::TerrainManipulator::performMovementMiddleMouseButton(eventTimeDelta, dx, dy);
        }
        bool performMovementMiddleMouseButton(const double eventTimeDelta, const double dx, const double dy) {
            return osgGA::TerrainManipulator::performMovementLeftMouseButton(eventTimeDelta, dx, dy);
        }
        bool performMovementRightMouseButton(const double eventTimeDelta, const double dx, const double dy) {
            return osgGA::TerrainManipulator::performMovementRightMouseButton(eventTimeDelta, dx, -dy);
        }
    };

    class FXOSGAdapter : public osgViewer::GraphicsWindow {
    public:
        FXOSGAdapter(GUISUMOAbstractView* parent, FXCursor* cursor);
        void grabFocus();
        void grabFocusIfPointerInWindow() {}
        void useCursor(bool cursorOn);

        bool makeCurrentImplementation();
        bool releaseContext();
        void swapBuffersImplementation();

        // not implemented yet...just use dummy implementation to get working.
        bool valid() const {
            return true;
        }
        bool realizeImplementation() {
            return true;
        }
        bool isRealizedImplementation() const  {
            return true;
        }
        void closeImplementation() {}
        bool releaseContextImplementation() {
            return true;
        }

    protected:
        ~FXOSGAdapter();
    private:
        GUISUMOAbstractView* const myParent;
        FXCursor* const myOldCursor;
    };

protected:

    osg::ref_ptr<FXOSGAdapter> myAdapter;
    osg::ref_ptr<osgViewer::Viewer> myViewer;
    osg::ref_ptr<osg::Group> myRoot;

private:
    GUIVehicle* myTracked;
    osg::ref_ptr<osgGA::CameraManipulator> myCameraManipulator;

    SUMOTime myLastUpdate;

    std::map<MSVehicle*, OSGMovable > myVehicles;
    std::map<MSTransportable*, OSGMovable > myPersons;

    osg::ref_ptr<osg::Node> myGreenLight;
    osg::ref_ptr<osg::Node> myYellowLight;
    osg::ref_ptr<osg::Node> myRedLight;
    osg::ref_ptr<osg::Node> myRedYellowLight;

protected:
    GUIOSGView() { }

};

#endif

#endif

/****************************************************************************/



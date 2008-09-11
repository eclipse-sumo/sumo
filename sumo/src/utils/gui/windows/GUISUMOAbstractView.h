/****************************************************************************/
/// @file    GUISUMOAbstractView.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for a view
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
#ifndef GUISUMOAbstractView_h
#define GUISUMOAbstractView_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <fx.h>
#include <fx3d.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position2D.h>
#include <utils/common/RGBColor.h>
#include <utils/shapes/Polygon2D.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include "GUIGrid.h"
#include <utils/gui/drawer/GUIColoringSchemesMap.h>
#include <utils/foxtools/MFXMutex.h>

#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGLObjectToolTip;
class MSVehicle;
class GUINet;
class GUIGlChildWindow;
class GUIVehicle;
class GUILaneWrapper;
class GUIPerspectiveChanger;
class GUIMainWindow;
class GUIJunctionWrapper;
class GUIDetectorWrapper;
class GUIGLObjectPopupMenu;
class GUIGlObject;
class GUIGlObjectStorage;
class PointOfInterest;
class ShapeContainer;
class GUIDialog_EditViewport;
class GUIDialog_ViewSettings;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUISUMOAbstractView
 * This class is meant to be pure virtual later;
 * It shall be the main class to inherit views of the simulation (micro-
 * or macroscopic ones) from it.
 */
class GUISUMOAbstractView : public FXGLCanvas
{
    FXDECLARE(GUISUMOAbstractView)
public:
    /// constructor
    GUISUMOAbstractView(FXComposite *p, GUIMainWindow &app,
                        GUIGlChildWindow *parent, const GUIGrid &grid,
                        FXGLVisual *glVis);

    /// constructor
    GUISUMOAbstractView(FXComposite *p, GUIMainWindow &app,
                        GUIGlChildWindow *parent, const GUIGrid &grid,
                        FXGLVisual *glVis, FXGLCanvas *share);

    /// destructor
    virtual ~GUISUMOAbstractView();

    /// builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow &) { }

    /// recenters the view
    void recenterView();

    /// centers to the chosen artifact
    virtual void centerTo(GUIGlObject *o);

    /// rename the chosen artifact
    virtual void rename(GUIGlObject *o);

    /// move the chosen artifact
    virtual void moveTo(GUIGlObject *o);

    /// change the color of the chosen artifact
    virtual void changeCol(GUIGlObject *o);

    /// change typ of the chosen artifact
    virtual void changeTyp(GUIGlObject *o);

    /// delete the chosen artifact
    virtual void deleteObj(GUIGlObject *o);

    /// applies the given viewport settings
    virtual void setViewport(SUMOReal zoom, SUMOReal xPos, SUMOReal yPos);

    /// meter-to-pixels conversion method
    SUMOReal m2p(SUMOReal meter);

    /// pixels-to-meters conversion method
    SUMOReal p2m(SUMOReal pixel);

    /// Returns the information whether rotation is allowd
//    bool allowRotation() const;

    /// Returns the gl-id of the object under the given coordinates
    void setTooltipPosition(FXint x, FXint y, FXint mouseX, FXint mouseY);

    /// A reimplementation due to some internal reasons
    FXbool makeCurrent();

    /// returns true, if the edit button was pressed
    bool isInEditMode();

    //GUINet &getNet() const;


    long onConfigure(FXObject*,FXSelector,void*);
    long onPaint(FXObject*,FXSelector,void*);
    virtual long onLeftBtnPress(FXObject*,FXSelector,void*);
    virtual long onLeftBtnRelease(FXObject*,FXSelector,void*);
    virtual long onRightBtnPress(FXObject*,FXSelector,void*);
    virtual long onRightBtnRelease(FXObject*,FXSelector,void*);
    virtual long onMouseMove(FXObject*,FXSelector,void*);
    virtual long onMouseLeft(FXObject*,FXSelector,void*);

//    long onCmdShowGrid(FXObject*,FXSelector,void*);
    long onSimStep(FXObject*sender,FXSelector,void*);

    long onKeyPress(FXObject *o,FXSelector sel,void *data);
    long onKeyRelease(FXObject *o,FXSelector sel,void *data);


    virtual void openObjectDialog();

    /// A method that updates the tooltip
    void updateToolTip();

    /// Returns the maximum width of gl-windows
    int getMaxGLWidth() const;

    /// Returns the maximum height of gl-windows
    int getMaxGLHeight() const;

    /// paints the area to a buffer
    FXColor *getSnapshot();

    void showViewportEditor();
    virtual void showViewschemeEditor() = 0;
    void showToolTips(bool val);
    virtual void setColorScheme(char*) { }

    void drawShapes(const ShapeContainer &sc, int maxLayer, SUMOReal width);

    void remove(GUIDialog_EditViewport *) {
        myViewportChooser = 0;
    }

    void remove(GUIDialog_ViewSettings *) {
        myVisualizationChanger = 0;
    }


    SUMOReal getGridWidth() const;
    SUMOReal getGridHeight() const;

    /// shows a vehicle route
    virtual void showRoute(GUIVehicle * /*v*/, int /*index=-1*/) { }

    /// shows
    virtual void showBestLanes(GUIVehicle * /*v*/) { }

    /// hides a vehicle route
    virtual void hideRoute(GUIVehicle * /*v*/, int /*index=-1*/) { }

    /// hides
    virtual void hideBestLanes(GUIVehicle * /*v*/) { }

    virtual void startTrack(int /*id*/) { }
    virtual void stopTrack() { }
    virtual int getTrackedID() const {
        return -1;
    }
    virtual bool amShowingRouteFor(GUIVehicle * /*v*/, int /*index=-1*/) {
        return false;
    }

    virtual bool amShowingBestLanesFor(GUIVehicle * /*v*/) {
        return false;
    }

public:
    /**
     * JunctionColoringScheme
     * This enumeration holds the possible vehicle colouring schemes
     */
    enum JunctionColoringScheme {
        /// colouring by vehicle speed
        VCS_BY_TYPE = 0
    };

    struct Decal {
        std::string filename;
        SUMOReal left;
        SUMOReal top;
        SUMOReal right;
        SUMOReal bottom;
        SUMOReal rot;
        bool initialised;
        int glID;
    };

public:
    /**
     * @class ViewSettings
     * This class stores the viewport information for an easier checking whether
     *  it has changed.
     */
    class ViewportSettings
    {
    public:
        /// Constructor
        ViewportSettings();

        /// Parametrised Constructor
        ViewportSettings(SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax);

        /// Destructor
        ~ViewportSettings();

        /// Returns the information whether the stored setting differs from the given
        bool differ(SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax);

        /// Sets the setting information to the given values
        void set(SUMOReal xmin, SUMOReal ymin, SUMOReal xmax, SUMOReal ymax);

    private:
        /// Position and size information to describe the viewport
        SUMOReal myXMin, myYMin, myXMax, myYMax;

    };

    /**
     * @struct VisualizationSettings
     * @brief This class stores the information about how to visualize the structures
     */
    struct VisualizationSettings {
        /// The name of this setting
        std::string name;

        /// Information whether antialiase shall be enabled
        bool antialiase;
        /// Information whether dithering shall be enabled
        bool dither;

        /// @name background visualization settings
        //@{

        /// The background color to use
        RGBColor backgroundColor;
        /// Information whether background decals (textures) shall be used
        bool showBackgroundDecals;
        /// information whether a grid shall be shown
        bool showGrid;
        /// Information about the grid spacings
        SUMOReal gridXSize, gridYSize;
        //@}


        /// @name lane visualization settings
        //@{

        /// The lane visualization scheme
        int laneEdgeMode;
        /// The map if used colors (scheme->used colors)
        std::map<int, std::vector<RGBColor> > laneColorings;
#ifdef HAVE_MESOSIM
        /// The map if used colors (scheme->used colors)
        std::map<int, std::vector<RGBColor> > edgeColorings;
#endif
        /// Information whether lane borders shall be drawn
        bool laneShowBorders;
        /// Information whether link textures (arrows) shall be drawn
        bool showLinkDecals;
        int laneEdgeExaggMode; // !!! unused
        SUMOReal minExagg; // !!! unused
        SUMOReal maxExagg; // !!! unused
        /// Information whether rails shall be drawn
        bool showRails;
        /// Information whether the edge's name shall be drawn
        bool drawEdgeName;
        /// The size of the edge name
        float edgeNameSize;
        /// The color of edge names
        RGBColor edgeNameColor;
        //@}


        /// @name vehicle visualization settings
        //@{

        /// The vehicle visualization scheme
        int vehicleMode;
        /// The minimum size of vehicles to let them be drawn
        float minVehicleSize;
        /// The vehicle exaggeration (upscale)
        float vehicleExaggeration;
        /// The map if used colors (scheme->used colors)
        std::map<int, std::vector<RGBColor> > vehicleColorings;
        /// Information whether vehicle blinkers shall be drawn
        bool showBlinker;
        /// Information whether the c2c radius shall be drawn
        bool drawcC2CRadius;
        /// Information whether the lane change preference shall be drawn
        bool drawLaneChangePreference;
        /// Information whether the vehicle's name shall be drawn
        bool drawVehicleName;
        /// The size of the vehicle name
        float vehicleNameSize;
        /// The color of vehicle names
        RGBColor vehicleNameColor;
        //@}


        /// @name junction visualization settings
        //@{

        /// The junction visualization scheme
        int junctionMode;
        /// Information whether a link's tls index shall be drawn
        bool drawLinkTLIndex;
        /// Information whether a link's junction index shall be drawn
        bool drawLinkJunctionIndex;
        /// Information whether the junction's name shall be drawn
        bool drawJunctionName;
        /// The size of the junction name
        float junctionNameSize;
        /// The color of junction names
        RGBColor junctionNameColor;
        //@}


        /// Information whether lane-to-lane arrows shall be drawn
        bool showLane2Lane;


        /// @name additional structures visualization settings
        //@{

        /// The additional structures visualization scheme
        int addMode;
        /// The minimum size of additional structures to let them be drawn
        float minAddSize;
        /// The additional structures exaggeration (upscale)
        float addExaggeration;
        /// Information whether the additional's name shall be drawn
        bool drawAddName;
        /// The size of the additionals' name
        float addNameSize;
        // The color of additionals' names
        //RGBColor addNameColor;
        //@}


        /// @name shapes visualization settings
        //@{

        /// The minimum size of shapes to let them be drawn
        float minPOISize;
        /// The additional shapes (upscale)
        float poiExaggeration;
        /// Information whether the poi's name shall be drawn
        bool drawPOIName;
        /// The size of the poi name
        float poiNameSize;
        /// The color of poi names
        RGBColor poiNameColor;
        //@}

        /// Information whether the size legend shall be drawn
        bool showSizeLegend;

        bool operator==(const VisualizationSettings &vs2);

    };

    FXComboBox &getColoringSchemesCombo();

    Position2D getPositionInformation() const;


protected:

    /// performs the painting of the simulation
    void paintGL();

    /// Draws the given polygon
    void drawPolygon2D(const Polygon2D &polygon) const;

    /// Draws the given poi
    void drawPOI2D(const PointOfInterest &p, SUMOReal width) const;

    void updatePositionInformation() const;

    Position2D getPositionInformation(int x, int y) const;

    void drawShapesLayer(const ShapeContainer &sc, int layer, SUMOReal width);

    virtual void doPaintGL(int /*mode*/, SUMOReal /*scale*/) { }

    virtual void doInit() { }

    /// paints a grid
    void paintGLGrid();

    /** applies the changes arised from window resize or movement */
    void applyChanges(SUMOReal scale, size_t xoff, size_t yoff);

    /// draws the legend
    void displayLegend(bool flip=false);

    /// centers the given boundary
    void centerTo(Boundary bound);

    /// returns the id of the object under the cursor using GL_SELECT
    unsigned int getObjectUnderCursor();

    /// invokes the tooltip for the given object
    void showToolTipFor(unsigned int id);

    /// Clears the usetable, filling it with false
    void clearUsetable(size_t *myEdges2Show, size_t myEdges2ShowSize);

protected:
    /// The application
    GUIMainWindow *myApp;

    /// the parent window
    GUIGlChildWindow *myParent;

    /// the network used (stored here for a faster access)
    GUIGrid *myGrid;

    /// the sizes of the window
    int myWidthInPixels, myHeightInPixels;

    /// the scale of the net (the maximum size, either width or height)
    SUMOReal myNetScale;

    /// The perspective changer
    GUIPerspectiveChanger *myChanger;

    /// Information whether too-tip informations shall be generated
    bool myInEditMode;

    /// The used tooltip-class
    GUIGLObjectToolTip *myToolTip;

    /// position to display the tooltip at
    FXint myToolTipX, myToolTipY;

    /// The current mouse position (if the mouse is over this canvas)
    FXint myMouseX, myMouseY;

    /// Offset to the mouse-hotspot from the mouse position
    int myMouseHotspotX, myMouseHotspotY;

    /// myWidthInPixels / myHeightInPixels
    SUMOReal myRatio;

    /// Additional scaling factor for meters-to-pixels conversion
    SUMOReal myAddScl;

    /// The current popup-menu
    GUIGLObjectPopupMenu *myPopup;

    /// the description of the viewport
    ViewportSettings myViewportSettings;

    VisualizationSettings *myVisualizationSettings;

    bool myUseToolTips;

    /// Internal information whether doInit() was called
    bool myAmInitialised;


    GUIDialog_EditViewport *myViewportChooser;
    GUIDialog_ViewSettings *myVisualizationChanger;

    std::vector<Decal> myDecals;
    MFXMutex myDecalsLock;

    mutable MFXMutex myPolyDrawLock;

    enum VehicleOperationType {
        VO_TRACK,
        VO_SHOW_BEST_LANES,
        VO_SHOW_ROUTE
    };

    struct VehicleOps {
        VehicleOperationType type;
        GUIVehicle *vehicle;
        int routeNo;
    };

    /// List of vehicles for which something has to be done with
    std::vector<VehicleOps> myVehicleOps;

    /**
     * A class to find the matching lane wrapper
     */
    class vehicle_in_ops_finder
    {
    public:
        /** constructor */
        explicit vehicle_in_ops_finder(const GUIVehicle * const v)
                : myVehicle(v) { }

        /** the comparing function */
        bool operator()(const VehicleOps &vo) {
            return vo.vehicle == myVehicle;
        }

    private:
        /// The vehicle to search for
        const GUIVehicle * const myVehicle;

    };

protected:
    GUISUMOAbstractView() { }

};


#endif

/****************************************************************************/


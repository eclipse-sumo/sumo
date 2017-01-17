/****************************************************************************/
///@brief @file    GUISUMOAbstractView.h
///@brief @author  Daniel Krajzewicz
///@brief @author  Jakob Erdmann
///@brief @author  Michael Behrisch
///@brief @author  Andreas Gaubatz
///@brief @date    Sept 2002
///@brief @version $Id$
///
// The base class for a view
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
#include <map>
#include <fx.h>
// fx3d includes windows.h so we need to guard against macro pollution
#ifdef WIN32
#define NOMINMAX
#endif
#include <fx3d.h>
#ifdef WIN32
#undef NOMINMAX
#endif

#include <utils/foxtools/MFXMutex.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOTime.h>
#include <utils/shapes/Polygon.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <foreign/rtree/SUMORTree.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlChildWindow;
class GUIVehicle;
class GUIPerspectiveChanger;
class GUIMainWindow;
class GUIGLObjectPopupMenu;
class GUIGlObject;
class GUIDialog_EditViewport;
class GUIDialog_ViewSettings;
class GUIVisualizationSettings;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUISUMOAbstractView
 * This class is meant to be pure virtual later;
 * It shall be the main class to inherit views of the simulation (micro-
 * or macroscopic ones) from it.
 */
class GUISUMOAbstractView : public FXGLCanvas {
    FXDECLARE(GUISUMOAbstractView)
public:
    ///@brief constructor
    GUISUMOAbstractView(FXComposite* p, GUIMainWindow& app, GUIGlChildWindow* parent, const SUMORTree& grid, FXGLVisual* glVis, FXGLCanvas* share);

    ///@brief destructor
    virtual ~GUISUMOAbstractView();

    ///@brief builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow&) { }

    ///@brief recenters the view
    virtual void recenterView();

    /** @brief centers to the chosen artifact
     * @param[in] id The id of the artifact to center to
     * @param[in] applyZoom Whether to zoom in
     * @param[in] zoomDist The distance in m to use for the zoom, values < 0 means: use the centeringBoundary
     * @note caller is responsible for calling update
     */
    virtual void centerTo(GUIGlID id, bool applyZoom, SUMOReal zoomDist = 20);

    ///@brief centers to the chosen artifact
    void centerTo(const Boundary& bound);

    ///@brief applies the given viewport settings
    virtual void setViewportFromTo(const Position& lookFrom, const Position& lookAt);

    ///@brief copy the viewport to the given view
    virtual void copyViewportTo(GUISUMOAbstractView* view);

    ///@brief meter-to-pixels conversion method
    SUMOReal m2p(SUMOReal meter) const;

    ///@brief pixels-to-meters conversion method
    SUMOReal p2m(SUMOReal pixel) const;

    ///@brief Returns the information whether rotation is allowd
    ///@note disabled
    //bool allowRotation() const;

    ///@brief Returns the gl-id of the object under the given coordinates
    void setWindowCursorPosition(FXint x, FXint y);

    ///@brief A reimplementation due to some internal reasons
    FXbool makeCurrent();

    ///@brief returns true, if the edit button was pressed
    bool isInEditMode();

    ///@brief get changer
    GUIPerspectiveChanger& getChanger() const;

    ///@brief get visible boundary
    Boundary getVisibleBoundary() const;

    ///@brief mouse functions
    //@{
    virtual long onConfigure(FXObject*, FXSelector, void*);
    virtual long onPaint(FXObject*, FXSelector, void*);
    virtual long onLeftBtnPress(FXObject*, FXSelector, void*);
    virtual long onLeftBtnRelease(FXObject*, FXSelector, void*);
    virtual long onMiddleBtnPress(FXObject*, FXSelector, void*);
    virtual long onMiddleBtnRelease(FXObject*, FXSelector, void*);
    virtual long onRightBtnPress(FXObject*, FXSelector, void*);
    virtual long onRightBtnRelease(FXObject*, FXSelector, void*);
    virtual long onDoubleClicked(FXObject*, FXSelector, void*);
    virtual long onMouseWheel(FXObject*, FXSelector, void*);
    virtual long onMouseMove(FXObject*, FXSelector, void*);
    virtual long onMouseLeft(FXObject*, FXSelector, void*);
    //@}

    ///@brief keyboard functions
    //@{
    virtual long onKeyPress(FXObject* o, FXSelector sel, void* data);
    virtual long onKeyRelease(FXObject* o, FXSelector sel, void* data);
    //@}

    //@brief open object dialog
    virtual void openObjectDialog();

    ///@brief A method that updates the tooltip
    void updateToolTip();

    ///@brief @name Dealing with snapshots
    ///@{
    /** @brief Sets the snapshot time to file map
     * @param[in] snaps The snapshots to take at certain times
     */
    void setSnapshots(std::map<SUMOTime, std::string> snaps);

    /** @brief Takes a snapshots and writes it into the given file
     *
     * The format to use is determined from the extension.
     * If compiled with ffmpeg and a video format is requested it will instantiate a video encoder.
     * @param[in] destFile The name of the file to write the snapshot into
     * @return The error message, if an error occcured; "" otherwise
     */
    std::string makeSnapshot(const std::string& destFile);

    ///@brief Adds a frame to a video snapshot which will be initialized if neccessary
    virtual void saveFrame(const std::string& destFile, FXColor* buf);

    ///@brief Ends a video snapshot
    virtual void endSnapshot() {}

    ///@brief Checks whether it is time for a snapshot
    virtual void checkSnapshots();

    ///@brief get the current simulation time
    virtual SUMOTime getCurrentTimeStep() const;
    ///@}

    ///@brief get the viewport and create it on first access
    GUIDialog_EditViewport* getViewportEditor();

    ///@brief show viewport editor
    virtual void showViewportEditor();

    ///@brief show viewsscheme editor
    void showViewschemeEditor();

    ///@brief show tool tips
    void showToolTips(bool val);

    ///@brief set color scheme
    virtual bool setColorScheme(const std::string&);

    ///@brief get visualitation settings
    GUIVisualizationSettings* getVisualisationSettings() const;

    ///@brief remove viewport
    void remove(GUIDialog_EditViewport*);

    ///@brief remove view settings
    void remove(GUIDialog_ViewSettings*);

    ///@brief get grid width
    // @todo: check why this is here
    SUMOReal getGridWidth() const;

    ///@brief get grid Height
    // @todo: check why this is here
    SUMOReal getGridHeight() const;

    ///@brief star track
    virtual void startTrack(int /*id*/);

    ///@brief stop track
    virtual void stopTrack();

    ///@brief get tracked id
    virtual GUIGlID getTrackedID() const;

    ///@brief on gaming click
    virtual void onGamingClick(Position /*pos*/);

    ///@brief @name Additional visualisations
    ///@{
    /** @brief Adds an object to call its additional visualisation method
     * @param[in] which The object to add
     * @return Always true
     * @see GUIGlObject::drawGLAdditional
     */
    bool addAdditionalGLVisualisation(const GUIGlObject* const which);

    /** @brief Removes an object from the list of objects that show additional things
     * @param[in] which The object to remoe
     * @return True if the object was known, false otherwise
     * @see GUIGlObject::drawGLAdditional
     */
    bool removeAdditionalGLVisualisation(const GUIGlObject* const which);

    /** @brief Check if an object is added in the additional GL visualitation
     * @param[in] which The object to check
     * @see GUIGlObject::drawGLAdditional
     */
    bool isAdditionalGLVisualisationEnabled(GUIGlObject* const which) const;
    ///@}

    ///@brief destoys the popup
    void destroyPopup();

public:
    ///@struct Decal
    ///@brief A decal (an image) that can be shown
    struct Decal {
        ///@brief Constructor
        Decal();

        ///@brief The path to the file the image is located at
        std::string filename;
        ///@brief The center of the image in x-direction (net coordinates, in m)
        SUMOReal centerX;
        ///@brief The center of the image in y-direction (net coordinates, in m)
        SUMOReal centerY;
        ///@brief The center of the image in z-direction (net coordinates, in m)
        SUMOReal centerZ;
        ///@brief The width of the image (net coordinates in x-direction, in m)
        SUMOReal width;
        ///@brief The height of the image (net coordinates in y-direction, in m)
        SUMOReal height;
        ///@brief The altitude of the image (net coordinates in z-direction, in m)
        SUMOReal altitude;
        ///@brief The rotation of the image in the ground plane (in degrees)
        SUMOReal rot;
        ///@brief The tilt of the image to the ground plane (in degrees)
        SUMOReal tilt;
        ///@brief The roll of the image to the ground plane (in degrees)
        SUMOReal roll;
        ///@brief The layer of the image
        SUMOReal layer;
        ///@brief Whether this image was initialised (inserted as a texture)
        bool initialised;
        ///@brief Whether this image should be skipped in 2D-views
        bool skip2D;
        ///@brief Whether this image should be skipped in 2D-views
        bool screenRelative;
        ///@brief whether the decal shall be drawn in screen coordinates, rather than network coordinates
        int glID;
        ///@brief The image pointer for later cleanup
        FXImage* image;
    };

public:
    ///@brief get coloring schemes combo
    FXComboBox& getColoringSchemesCombo();

    ///@brief Returns the cursor's x/y position within the network
    Position getPositionInformation() const;

    ///@brief Translate screen position to network position
    Position screenPos2NetPos(int x, int y) const;

    ///@brief add decals
    void addDecals(const std::vector<Decal>& decals);

    ///@brief get visualisation settings
    GUIVisualizationSettings* getVisualisationSettings();

    ///@brief Returns the delay of the parent application
    SUMOReal getDelay() const;

    /// @brief Sets the delay of the parent application
    void setDelay(SUMOReal delay);

protected:
    ///@brief performs the painting of the simulation
    void paintGL();

    ///@brief update position information
    void updatePositionInformation() const;

    ///@brief paint GL
    virtual int doPaintGL(int /*mode*/, const Boundary& /*boundary*/);

    ///@brief doInit
    virtual void doInit();

    ///@brief paints a grid
    void paintGLGrid();

    ///@briefDraws a line with ticks, and the length information.
    void displayLegend();

    ///@brief returns the id of the object under the cursor using GL_SELECT
    GUIGlID getObjectUnderCursor();

    ///@brief returns the id of the object at position using GL_SELECT
    GUIGlID getObjectAtPosition(Position pos);

    ///@brief returns the ids of the object at position within the given (rectangular) radius using GL_SELECT
    std::vector<GUIGlID> getObjectsAtPosition(Position pos, SUMOReal radius);

    ///@brief returns the ids of all objects in the given boundary
    std::vector<GUIGlID> getObjectsInBoundary(const Boundary& bound);

    ///@brief invokes the tooltip for the given object
    void showToolTipFor(const GUIGlID id);

protected:
    ///@brief check whether we can read image data or position with gdal
    FXImage* checkGDALImage(Decal& d);

    ///@brief Draws the stored decals
    void drawDecals();

    ///@brief applies gl-transformations to fit the Boundary given by myChanger onto the canvas.
    /// If fixRatio is true, this boundary will be enlarged to prevent anisotropic stretching.
    /// (this should be set to false when doing selections)
    void applyGLTransform(bool fixRatio = true);

protected:
    ///@brief The application
    GUIMainWindow* myApp;

    ///@brief The parent window
    GUIGlChildWindow* myParent;

    ///@brief The visualization speed-up
    SUMORTree* myGrid;

    ///@brief The perspective changer
    GUIPerspectiveChanger* myChanger;

    ///@brief Information whether too-tip informations shall be generated
    bool myInEditMode;

    ///@brief Offset to the mouse-hotspot from the mouse position
    int myMouseHotspotX, myMouseHotspotY;

    ///@brief The current popup-menu
    GUIGLObjectPopupMenu* myPopup;

    ///@brief visualization settings
    GUIVisualizationSettings* myVisualizationSettings;

    ///@brief use tool tips
    bool myUseToolTips;

    ///@brief Internal information whether doInit() was called
    bool myAmInitialised;

    ///@brief viewport chooser
    GUIDialog_EditViewport* myViewportChooser;

    ///@brief Position of the cursor relative to the window
    FXint myWindowCursorPositionX, myWindowCursorPositionY;

    ///@brief Visualization changer
    GUIDialog_ViewSettings* myVisualizationChanger;

    ///@brief @name Optionally shown decals
    ///@{
    ///@brief The list of decals to show
    std::vector<Decal> myDecals;

    ///@brief The mutex to use before accessing the decals list in order to avoid thread conficts
    MFXMutex myDecalsLock;
    ///@}

    ///@brief Snapshots
    std::map<SUMOTime, std::string> mySnapshots;

    ///@brief poly draw lock
    mutable MFXMutex myPolyDrawLock;

    ///@brief List of objects for which GUIGlObject::drawGLAdditional is called
    std::map<const GUIGlObject*, int> myAdditionallyDrawn;

protected:
    ///@brief empty constructor
    GUISUMOAbstractView() { }
};


#endif

/****************************************************************************/


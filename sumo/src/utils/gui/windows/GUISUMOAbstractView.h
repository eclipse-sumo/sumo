/****************************************************************************/
/// @file    GUISUMOAbstractView.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Andreas Gaubatz
/// @date    Sept 2002
/// @version $Id$
///
// The base class for a view
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include <fx3d.h>
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
    /// constructor
    GUISUMOAbstractView(FXComposite* p, GUIMainWindow& app,
                        GUIGlChildWindow* parent, const SUMORTree& grid,
                        FXGLVisual* glVis, FXGLCanvas* share);

    /// destructor
    virtual ~GUISUMOAbstractView();

    /// builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow&) { }

    /// recenters the view
    void recenterView();

    /** @brief centers to the chosen artifact
     * @param[in] id The id of the artifact to center to
     * @param[in] applyZoom Whether to zoom in
     * @param[in] zoomDist The distance in m to use for the zoom, values < 0 means: use the centeringBoundary
     * @note caller is responsible for calling update
     */
    void centerTo(GUIGlID id, bool applyZoom, SUMOReal zoomDist = 20);

    /// centers to the chosen artifact
    void centerTo(const Boundary& bound);

    /// applies the given viewport settings
    virtual void setViewport(SUMOReal zoom, SUMOReal xPos, SUMOReal yPos);

    /// meter-to-pixels conversion method
    SUMOReal m2p(SUMOReal meter) const;

    /// pixels-to-meters conversion method
    SUMOReal p2m(SUMOReal pixel) const;

    /// Returns the information whether rotation is allowd
//    bool allowRotation() const;

    /// Returns the gl-id of the object under the given coordinates
    void setWindowCursorPosition(FXint x, FXint y);

    /// A reimplementation due to some internal reasons
    FXbool makeCurrent();

    /// returns true, if the edit button was pressed
    bool isInEditMode();

    GUIPerspectiveChanger& getChanger() const {
        return *myChanger;
    }

    Boundary getVisibleBoundary() const;

    long onConfigure(FXObject*, FXSelector, void*);
    long onPaint(FXObject*, FXSelector, void*);
    virtual long onLeftBtnPress(FXObject*, FXSelector, void*);
    virtual long onLeftBtnRelease(FXObject*, FXSelector, void*);
    virtual long onRightBtnPress(FXObject*, FXSelector, void*);
    virtual long onRightBtnRelease(FXObject*, FXSelector, void*);
    virtual long onMouseWheel(FXObject*, FXSelector, void*);
    virtual long onMouseMove(FXObject*, FXSelector, void*);
    virtual long onMouseLeft(FXObject*, FXSelector, void*);

    long onKeyPress(FXObject* o, FXSelector sel, void* data);
    long onKeyRelease(FXObject* o, FXSelector sel, void* data);


    virtual void openObjectDialog();

    /// A method that updates the tooltip
    void updateToolTip();


    /// @name Dealing with snapshots
    /// @{

    /** @brief Sets the snapshot time to file map
     * @param[in] snaps The snapshots to take at certain times
     */
    void setSnapshots(std::map<SUMOTime, std::string> snaps);


    /** @brief Takes a snapshots and writes it into the given file
     *
     * The format to use is determined from the extension.
     * @param[in] destFile The name of the file to write the snapshot into
     * @return The error message, if an error occcured; "" otherwise
     */
    std::string makeSnapshot(const std::string& destFile);


    /** @brief Checks whether it is time for a snapshot
     */
    void checkSnapshots();

    /// @brief get the current simulation time
    virtual SUMOTime getCurrentTimeStep() const {
        return 0;
    }
    /// @}



    void showViewportEditor();
    virtual void showViewschemeEditor() = 0;
    void showToolTips(bool val);
    virtual bool setColorScheme(const std::string&) {
        return true;
    }

    void remove(GUIDialog_EditViewport*) {
        myViewportChooser = 0;
    }

    void remove(GUIDialog_ViewSettings*) {
        myVisualizationChanger = 0;
    }


    // @todo: check why this is here
    SUMOReal getGridWidth() const;
    // @todo: check why this is here
    SUMOReal getGridHeight() const;

    virtual void startTrack(int /*id*/) { }
    virtual void stopTrack() { }
    virtual int getTrackedID() const {
        return -1;
    }

    virtual void onGamingClick(Position /*pos*/) { }

    /// @name Additional visualisations
    /// @{

    /** @brief Adds an object to call its additional visualisation method
     * @param[in] which The object to add
     * @return Always true
     * @see GUIGlObject::drawGLAdditional
     */
    bool addAdditionalGLVisualisation(GUIGlObject* const which);


    /** @brief Removes an object from the list of objects that show additional things
     * @param[in] which The object to remoe
     * @return True if the object was known, false otherwise
     * @see GUIGlObject::drawGLAdditional
     */
    bool removeAdditionalGLVisualisation(GUIGlObject* const which);
    /// @}


    /// @brief destoys the popup
    void destroyPopup();


public:
    /**
     * JunctionColoringScheme
     * This enumeration holds the possible vehicle colouring schemes
     */
    enum JunctionColoringScheme {
        /// colouring by vehicle speed
        VCS_BY_TYPE = 0
    };


    /** @struct Decal
     * @brief A decal (an image) that can be shown
     */
    struct Decal {
        /// @brief Constructor
        Decal()
            : filename(), centerX(0), centerY(0),
              width(1000), height(1000), rot(0),
              initialised(false), glID(-1) { }

        /// @brief The path to the file the image is located at
        std::string filename;
        /// @brief The center of the image in x-direction (net coordinates, in m)
        SUMOReal centerX;
        /// @brief The center of the image in y-direction (net coordinates, in m)
        SUMOReal centerY;
        /// @brief The width of the image (net coordinates, in m)
        SUMOReal width;
        /// @brief The height of the image (net coordinates, in m)
        SUMOReal height;
        /// @brief The rotation of the image (in degrees)
        SUMOReal rot;
        /// @brief Whether this image was initialised (inserted as a texture)
        bool initialised;
        /// @brief The gl-id of the texture that holds this image
        int glID;
    };


public:
    FXComboBox& getColoringSchemesCombo();


    /** @brief Returns the cursor's x/y position within the network
     * @return The cursor's x/y position within the network
     */
    Position getPositionInformation() const;

    void addDecals(const std::vector<Decal> &decals) {
        myDecals.insert(myDecals.end(), decals.begin(), decals.end());
    }


    const GUIVisualizationSettings* getVisualisationSettings() {
        return myVisualizationSettings;
    }


protected:
    /// performs the painting of the simulation
    void paintGL();

    void updatePositionInformation() const;


    virtual int doPaintGL(int /*mode*/, const Boundary& /*boundary*/) {
        return 0;
    }

    virtual void doInit() { }

    /// paints a grid
    void paintGLGrid();

    /** brief Draws the size legend
     *
     * Draws a line with ticks, and the length information.
     */
    void displayLegend();


    /// returns the id of the object under the cursor using GL_SELECT
    GUIGlID getObjectUnderCursor();

    /// returns the id of the object at position using GL_SELECT
    GUIGlID getObjectAtPosition(Position pos);

    /// returns the ids of all objects in the given boundary
    std::vector<GUIGlID> getObjectsInBoundary(const Boundary& bound);

    /// invokes the tooltip for the given object
    void showToolTipFor(unsigned int id);


protected:
    /** @brief Draws the stored decals
     */
    void drawDecals();

    // applies gl-transformations to fit the Boundary given by myChanger onto
    // the canvas. If fixRatio is true, this boundary will be enlarged to
    // prevent anisotropic stretching. (this should be set to false when doing
    // selections)
    void applyGLTransform(bool fixRatio = true);

protected:
    /// @brief The application
    GUIMainWindow* myApp;

    /// @brief The parent window
    GUIGlChildWindow* myParent;

    /// @brief The visualization speed-up
    SUMORTree* myGrid;

    /// @brief The perspective changer
    GUIPerspectiveChanger* myChanger;

    /// @brief Information whether too-tip informations shall be generated
    bool myInEditMode;

    /// @brief Position of the cursor relative to the window
    FXint myWindowCursorPositionX, myWindowCursorPositionY;

    /// @brief Offset to the mouse-hotspot from the mouse position
    int myMouseHotspotX, myMouseHotspotY;

    /// @brief The current popup-menu
    GUIGLObjectPopupMenu* myPopup;

    GUIVisualizationSettings* myVisualizationSettings;

    bool myUseToolTips;

    /// Internal information whether doInit() was called
    bool myAmInitialised;


    GUIDialog_EditViewport* myViewportChooser;
    GUIDialog_ViewSettings* myVisualizationChanger;


    /// @name Optionally shown decals
    /// @{

    /// @brief The list of decals to show
    std::vector<Decal> myDecals;

    /// @brief The mutex to use before accessing the decals list in order to avoid thread conficts
    MFXMutex myDecalsLock;
    /// @}


    std::map<SUMOTime, std::string> mySnapshots;

    mutable MFXMutex myPolyDrawLock;

    /// @brief List of objects for which GUIGlObject::drawGLAdditional is called
    std::map<GUIGlObject*, int> myAdditionallyDrawn;


protected:
    GUISUMOAbstractView() { }

};


#endif

/****************************************************************************/


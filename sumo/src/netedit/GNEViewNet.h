/****************************************************************************/
/// @file    GNEViewNet.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A view on the network being edited (adapted from GUIViewTraffic)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEViewNet_h
#define GNEViewNet_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/Position.h>
#include <utils/common/RGBColor.h>
#include <utils/geom/PositionVector.h>
#include <utils/shapes/Polygon.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/common/StringBijection.h>

// ===========================================================================
// enum
// ===========================================================================
enum EditMode {
    /** placeholder mode */
    GNE_MODE_DUMMY,
    /** mode for creating new edges */
    GNE_MODE_CREATE_EDGE,
    /** mode for moving things */
    GNE_MODE_MOVE,
    /** mode for deleting things */
    GNE_MODE_DELETE,
    /** mode for inspecting object attributes */
    GNE_MODE_INSPECT,
    /** mode for selecting objects */
    GNE_MODE_SELECT,
    /** mode for connecting lanes */
    GNE_MODE_CONNECT,
    /** mode for editing tls */
    GNE_MODE_TLS
};

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEJunction;
class GNEEdge;
class GNEViewParent;
class GNEUndoList;
class GNEInspector;
class GNESelector;
class GNEConnector;
class GNETLSEditor;
class GNEPoly;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEViewNet
 * Microsocopic view at the simulation
 */
class GNEViewNet : public GUISUMOAbstractView {
    // FOX-declarations
    FXDECLARE(GNEViewNet)

public:
    /* @brief constructor
     * @param[in] toolbar A reference to the parents toolbar
     */
    GNEViewNet(FXComposite* tmpParent, FXComposite* actualParent,
               GUIMainWindow& app, GNEViewParent* viewParent, GNENet* net, FXGLVisual* glVis,
               FXGLCanvas* share, FXToolBar* toolBar);

    /// destructor
    virtual ~GNEViewNet();

    /// builds the view toolbars
    virtual void buildViewToolBars(GUIGlChildWindow&);


    bool setColorScheme(const std::string& name);


    /// overloaded handlers
    /// @{
    long onLeftBtnPress(FXObject*, FXSelector, void*);
    long onLeftBtnRelease(FXObject*, FXSelector, void*);
    long onMouseMove(FXObject*, FXSelector, void*);
    /// @}

    /// @brief sets edit mode via combo box
    long onCmdChangeMode(FXObject*, FXSelector, void*);

    /// @brief split edge at cursor position
    long onCmdSplitEdge(FXObject*, FXSelector, void*);

    /// @brief split edge at cursor position
    long onCmdSplitEdgeBidi(FXObject*, FXSelector, void*);

    /// @brief reverse edge
    long onCmdReverseEdge(FXObject*, FXSelector, void*);

    /// @brief add reversed edge
    long onCmdAddReversedEdge(FXObject*, FXSelector, void*);

    /// @brief set non-default geometry endpoint
    long onCmdSetEdgeEndpoint(FXObject*, FXSelector, void*);

    /// @brief restore geometry endpoint to node position
    long onCmdResetEdgeEndpoint(FXObject*, FXSelector, void*);

    /// @brief input custom node shape
    long onCmdNodeShape(FXObject*, FXSelector, void*);

    /// @brief replace node by geometry
    long onCmdNodeReplace(FXObject*, FXSelector, void*);

    /// @brief switch color scheme
    long onCmdVisualizeHeight(FXObject*, FXSelector, void*);

    /** @brief sets edit mode (from hotkey)
     * @param[in] selid An id MID_GNE_MODE_<foo> as defined in GUIAppEnum
     */
    void setEditModeFromHotkey(FXushort selid);

    // abort current edition operation
    void abortOperation(bool clearSelection = true);

    // handle del keypress
    void hotkeyDel();

    // handle enter keypress
    void hotkeyEnter();

    // store the position where a popup-menu was requested
    void markPopupPosition();

    // @brief get the net object
    GNENet* getNet() {
        return myNet;
    };

    void setStatusBarText(const std::string& text);

    /* @brief whether inspection, selection and inversion should apply to edges
     * or to lanes */
    bool selectEdges() {
        return mySelectEdges->getCheck() != 0;
    }

    /* @brief whether to autoselect nodes
     * or to lanes */
    bool autoSelectNodes() {
        return myExtendToEdgeNodes->getCheck() != 0;
    }

    GNESelector* getSelector() {
        return mySelector;
    }


    void setSelectionScaling(SUMOReal selectionScale) {
        myVisualizationSettings->selectionScale = selectionScale;
    }

    /// @brief update control contents after undo/redo or recompute
    void updateControls();

protected:
    int doPaintGL(int mode, const Boundary& bound);

    /// called after some features are already initialized
    void doInit();

    /// @brief FOX needs this
    GNEViewNet() {}

private:
    // we are not responsible for deletion
    GNENet* myNet;

    // the current edit mode
    EditMode myEditMode;

    // the previous edit mode used for toggling
    EditMode myPreviousEditMode;

    FXMenuCheck* mySelectEdges;
    FXMenuCheck* myExtendToEdgeNodes;

    /// @name the state-variables of the create-edge state-machine
    // @{
    /* @brief source junction for new edge
    * 0 if no edge source is selected
    * an existing (or newly created) junction otherwise
    */
    GNEJunction* myCreateEdgeSource;

    /* @brief whether the endpoint for a created edge should be set as the new
     * source
     */
    FXMenuCheck* myChainCreateEdge;
    FXMenuCheck* myAutoCreateOppositeEdge;
    // @}

    /// @name the state-variables of the move state-machine
    // @{
    /* @brief the Junction to be moved
     * 0 if nothing is grabbed
     * otherwise the junction being moved
     */
    GNEJunction* myJunctionToMove;

    /// @brief the edge of which geometry is being moved
    GNEEdge* myEdgeToMove;
    /// @brief position from which to move edge geometry
    Position myMoveSrc;

    /// @brief whether a selection is being moved
    bool myMoveSelection;

    /// @brief whether we should warn about merging junctions
    FXMenuCheck* myWarnAboutMerge;
    // @}

    /// @name state-variables of inspect-mode
    // @{
    FXMenuCheck* myVisualizeHeight;
    // @}
    //
    /// @name state-variables of select-mode
    // @{
    // @brief whether we have started rectangle-selection
    bool myAmInRectSelect;
    // @brief corner of the rectangle-selection
    Position mySelCorner1;
    Position mySelCorner2;
    // @}

    //@name toolbar related stuff
    //@{
    /// @brief a reference to the toolbar in myParent
    FXToolBar* myToolbar;
    /// @brief combo box for selecting the  edit mode
    FXComboBox* myEditModesCombo;

    /** @brief since we cannot switch on strings we map the mode names to an enum
     */
    StringBijection<EditMode> myEditModeNames;

    //@}

    /// @brief a reference to the undolist maintained in the application
    GNEUndoList* myUndoList;

    // @brief the panel for GNE_MODE_INSPECT
    GNEInspector* myInspector;

    // @brief the panel for GNE_MODE_SELECT
    GNESelector* mySelector;

    // @brief the panel for GNE_MODE_CONNECT
    GNEConnector* myConnector;

    // @brief the panel for GNE_MODE_TLS
    GNETLSEditor* myTLSEditor;

    Position myPopupSpot;

    GNEPoly* myCurrentPoly;

private:
    // set edit mode
    void setEditMode(EditMode mode);

    /// @brief adds controls for setting the edit mode
    void buildEditModeControls();

    /// @brief updates mode specific controls
    void updateModeSpecificControls();

    /// @brief delete all currently selected junctions
    void deleteSelectedJunctions();

    /// @brief delete all currently selected edges
    void deleteSelectedEdges();

    /** @brief try to merge moved junction with another junction in that spot
     * return true if merging did take place */
    bool mergeJunctions(GNEJunction* moved);

    /// @brief try to retrieve an edge at the given position
    GNEEdge* getEdgeAtCursorPosition(Position& pos);

    /// @brief try to retrieve a junction at the given position
    GNEJunction* getJunctionAtCursorPosition(Position& pos);

    /// @brief try to retrieve multiple edges at the given position
    std::set<GNEEdge*> getEdgesAtCursorPosition(Position& pos);


};


#endif

/****************************************************************************/


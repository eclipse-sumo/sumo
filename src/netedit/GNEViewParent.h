/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
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
/// @file    GNEViewParent.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// A single child window which contains a view of the edited network (adapted
// from GUISUMOViewParent)
// While we don't actually need MDI for netedit it is easier to adapt existing
// structures than to write everything from scratch.
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/gui/windows/GUIGlChildWindow.h>


// ===========================================================================
// class declarations
// ===========================================================================

class GNEApplicationWindow;
class GNEDialogACChooser;
class GNEFrame;
class GNENet;
class GNEUndoList;
// common frames
class GNEDeleteFrame;
class GNEInspectorFrame;
class GNESelectorFrame;
// network frames
class GNEAdditionalFrame;
class GNEConnectorFrame;
class GNECreateEdgeFrame;
class GNECrossingFrame;
class GNEPolygonFrame;
class GNEProhibitionFrame;
class GNETAZFrame;
class GNETLSEditorFrame;
// demand frames
class GNEPersonFrame;
class GNEPersonPlanFrame;
class GNEPersonTypeFrame;
class GNERouteFrame;
class GNEStopFrame;
class GNEVehicleFrame;
class GNEVehicleTypeFrame;
// data frames
class GNEEdgeDataFrame;
class GNEEdgeRelDataFrame;


// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class GNEViewParent
 * @brief A single child window which contains a view of the simulation area
 *
 * It is made of a tool-bar containing a field to change the type of display,
 *  buttons that allow to choose an artifact and some other view controlling
 *  options.
 *
 * The rest of the window is a canvas that contains the display itself
 */
class GNEViewParent : public GUIGlChildWindow {
    /// @brief FOX-declaration
    FXDECLARE(GNEViewParent)

public:
    /**@brief Constructor
     * also builds the view and calls create()
     *
     * @param[in] p The MDI-pane this window is shown within
     * @param[in] mdimenu The MDI-menu for alignment
     * @param[in] name The name of the window
     * @param[in] parentWindow The main window
     * @param[in] ic The icon of this window
     * @param[in] opts Window options
     * @param[in] x Initial x-position
     * @param[in] y Initial x-position
     * @param[in] w Initial width
     * @param[in] h Initial height
     * @param[in] share A canvas tor get the shared context from
     * @param[in] net The network to show
     */
    GNEViewParent(FXMDIClient* p, FXMDIMenu* mdimenu,
                  const FXString& name, GNEApplicationWindow* parentWindow,
                  FXGLCanvas* share, GNENet* net, GNEUndoList* undoList,
                  FXIcon* ic = nullptr, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    /// @brief Destructor
    ~GNEViewParent();

    /// @brief hide all frames
    void hideAllFrames();

    /// @brief get current frame (note: it can be null)
    GNEFrame* getCurrentShownFrame() const;

    /// @brief get frame for NETWORK_INSPECT
    GNEInspectorFrame* getInspectorFrame() const;

    /// @brief get frame for NETWORK_SELECT
    GNESelectorFrame* getSelectorFrame() const;

    /// @brief get frame for NETWORK_CONNECT
    GNEConnectorFrame* getConnectorFrame() const;

    /// @brief get frame for NETWORK_TLS
    GNETLSEditorFrame* getTLSEditorFrame() const;

    /// @brief get frame for NETWORK_ADDITIONAL
    GNEAdditionalFrame* getAdditionalFrame() const;

    /// @brief get frame for NETWORK_CROSSING
    GNECrossingFrame* getCrossingFrame() const;

    /// @brief get frame for NETWORK_TAZ
    GNETAZFrame* getTAZFrame() const;

    /// @brief get frame for NETWORK_DELETE
    GNEDeleteFrame* getDeleteFrame() const;

    /// @brief get frame for NETWORK_POLYGON
    GNEPolygonFrame* getPolygonFrame() const;

    /// @brief get frame for NETWORK_PROHIBITION
    GNEProhibitionFrame* getProhibitionFrame() const;

    /// @brief get frame for NETWORK_CREATEEDGE
    GNECreateEdgeFrame* getCreateEdgeFrame() const;

    /// @brief get frame for DEMAND_ROUTE
    GNERouteFrame* getRouteFrame() const;

    /// @brief get frame for DEMAND_VEHICLE
    GNEVehicleFrame* getVehicleFrame() const;

    /// @brief get frame for DEMAND_VEHICLETYPE
    GNEVehicleTypeFrame* getVehicleTypeFrame() const;

    /// @brief get frame for DEMAND_STOP
    GNEStopFrame* getStopFrame() const;

    /// @brief get frame for DEMAND_PERSONTYPE
    GNEPersonTypeFrame* getPersonTypeFrame() const;

    /// @brief get frame for DEMAND_PERSON
    GNEPersonFrame* getPersonFrame() const;

    /// @brief get frame for DEMAND_PERSONFRAME
    GNEPersonPlanFrame* getPersonPlanFrame() const;

    /// @brief get frame for DATA_EDGEDATA
    GNEEdgeDataFrame* getEdgeDataFrame() const;

    /// @brief get frame for DATA_EDGERELDATA
    GNEEdgeRelDataFrame* getEdgeRelDataFrame() const;

    /// @brief show frames area if at least a GNEFrame is showed
    /// @note this function is called in GNEFrame::Show();
    void showFramesArea();

    /// @brief hide frames area if all GNEFrames are hidden
    /// @note this function is called in GNEFrame::Show();
    void hideFramesArea();

    /// @brief get GUIMainWindow App
    GUIMainWindow* getGUIMainWindow() const;

    /// @brief get GNE Application Windows
    GNEApplicationWindow* getGNEAppWindows() const;

    /// @brief remove created chooser dialog
    void eraseACChooserDialog(GNEDialogACChooser* chooserDialog);

    /// @brief update toolbar undo/redo buttons (called when user press Ctrl+Z/Y)
    void updateUndoRedoButtons();

    /// @name FOX-callbacks
    /// @{
    /// @brief Called if the user wants to make a snapshot (screenshot)
    long onCmdMakeSnapshot(FXObject* sender, FXSelector, void*);

    /// @brief Called when the user hits the close button (x)
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief locator-callback
    long onCmdLocate(FXObject*, FXSelector, void*);

    /// @brief Called when user press a key
    long onKeyPress(FXObject* o, FXSelector sel, void* data);

    /// @brief Called when user releases a key
    long onKeyRelease(FXObject* o, FXSelector sel, void* data);

    /// @brief Called when user change the splitter between FrameArea and ViewNet
    long onCmdUpdateFrameAreaWidth(FXObject*, FXSelector, void*);
    /// @}

protected:
    FOX_CONSTRUCTOR(GNEViewParent)

private:
    /// @brief struct for Frames
    struct Frames {
        /// @brief constructor
        Frames();

        /// @brief hide frames
        void hideFrames();

        /// @brief set new width in all frames
        void setWidth(int frameWidth);

        /// @brief return true if at least there is a frame shown
        bool isFrameShown() const;

        /// @brief get current frame show
        GNEFrame* getCurrentShownFrame() const;

        /// @brief frame for NETWORK_INSPECT
        GNEInspectorFrame* inspectorFrame;

        /// @brief frame for NETWORK_SELECT
        GNESelectorFrame* selectorFrame;

        /// @brief frame for NETWORK_CONNECT
        GNEConnectorFrame* connectorFrame;

        /// @brief frame for NETWORK_TLS
        GNETLSEditorFrame* TLSEditorFrame;

        /// @brief frame for NETWORK_ADDITIONAL
        GNEAdditionalFrame* additionalFrame;

        /// @brief frame for NETWORK_CROSSING
        GNECrossingFrame* crossingFrame;

        /// @brief frame for NETWORK_TAZ
        GNETAZFrame* TAZFrame;

        /// @brief frame for NETWORK_DELETE
        GNEDeleteFrame* deleteFrame;

        /// @brief frame for NETWORK_POLYGON
        GNEPolygonFrame* polygonFrame;

        /// @brief frame for NETWORK_PROHIBITION
        GNEProhibitionFrame* prohibitionFrame;

        /// @brief frame for NETWORK_CREATEDGE
        GNECreateEdgeFrame* createEdgeFrame;

        /// @brief frame for DEMAND_ROUTE
        GNERouteFrame* routeFrame;

        /// @brief frame for DEMAND_VEHICLE
        GNEVehicleFrame* vehicleFrame;

        /// @brief frame for DEMAND_VEHICLETYPE
        GNEVehicleTypeFrame* vehicleTypeFrame;

        /// @brief frame for DEMAND_STOP
        GNEStopFrame* stopFrame;

        /// @brief frame for DEMAND_PERSON
        GNEPersonFrame* personFrame;

        /// @brief frame for DEMAND_PERSONTYPE
        GNEPersonTypeFrame* personTypeFrame;

        /// @brief frame for DEMAND_PERSONPLAN
        GNEPersonPlanFrame* personPlanFrame;

        /// @brief frame for DATA_EDGEDATA
        GNEEdgeDataFrame* edgeDataFrame;

        /// @brief frame for DATA_EDGERELDATA
        GNEEdgeRelDataFrame* edgeRelDataFrame;
    };

    /// @brief struct for ACChoosers dialog
    struct ACChoosers {
        /// @brief constructor
        ACChoosers();

        /// @brief destructor
        ~ACChoosers();

        /// @brief pointer to ACChooser dialog used for locate junctions
        GNEDialogACChooser* ACChooserJunction;

        /// @brief pointer to ACChooser dialog used for locate edges
        GNEDialogACChooser* ACChooserEdges;

        /// @brief pointer to ACChooser dialog used for locate vehicles
        GNEDialogACChooser* ACChooserVehicles;

        /// @brief pointer to ACChooser dialog used for locate persons
        GNEDialogACChooser* ACChooserPersons;

        /// @brief pointer to ACChooser dialog used for locate routes
        GNEDialogACChooser* ACChooserRoutes;

        /// @brief pointer to ACChooser dialog used for locate stops
        GNEDialogACChooser* ACChooserStops;

        /// @brief pointer to ACChooser dialog used for locate TLSs
        GNEDialogACChooser* ACChooserTLS;

        /// @brief pointer to ACChooser dialog used for locate additional
        GNEDialogACChooser* ACChooserAdditional;

        /// @brief pointer to ACChooser dialog used for locate POIs
        GNEDialogACChooser* ACChooserPOI;

        /// @brief pointer to ACChooser dialog used for locate Polygons
        GNEDialogACChooser* ACChooserPolygon;

        /// @brief pointer to ACChooser dialog used for locate Prohibitions
        GNEDialogACChooser* ACChooserProhibition;
    };

    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEAppWindows;

    /// @brief frame to hold myView and myAttributePanel
    FXHorizontalFrame* myViewArea;

    /// @brief frame to hold GNEFrames
    FXHorizontalFrame* myFramesArea;

    /// @brief toolbar undo button
    FXButton* myUndoButton;

    /// @brief toolbar redo button
    FXButton* myRedoButton;

    /// @brief Splitter to divide ViewNet und GNEFrames
    FXSplitter* myFramesSplitter;

    /// @brief struct for frames
    Frames myFrames;

    /// @brief struct for ACChoosers
    ACChoosers myACChoosers;
};

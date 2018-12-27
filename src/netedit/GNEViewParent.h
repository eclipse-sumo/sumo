/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEViewParent.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A single child window which contains a view of the edited network (adapted
// from GUISUMOViewParent)
// While we don't actually need MDI for netedit it is easier to adapt existing
// structures than to write everything from scratch.
/****************************************************************************/
#ifndef GNEViewParent_h
#define GNEViewParent_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/gui/windows/GUIGlChildWindow.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GNEAdditionalFrame;
class GNEApplicationWindow;
class GNEConnectorFrame;
class GNECreateEdgeFrame;
class GNECrossingFrame;
class GNEDeleteFrame;
class GNEDialogACChooser;
class GNEInspectorFrame;
class GNERouteFrame;
class GNENet;
class GNEPolygonFrame;
class GNEProhibitionFrame;
class GNESelectorFrame;
class GNETAZFrame;
class GNETLSEditorFrame;
class GNEUndoList;

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

    /// @brief get frame for GNE_NMODE_INSPECT
    GNEInspectorFrame* getInspectorFrame() const;

    /// @brief get frame for GNE_NMODE_SELECT
    GNESelectorFrame* getSelectorFrame() const;

    /// @brief get frame for GNE_NMODE_CONNECT
    GNEConnectorFrame* getConnectorFrame() const;

    /// @brief get frame for GNE_NMODE_TLS
    GNETLSEditorFrame* getTLSEditorFrame() const;

    /// @brief get frame for GNE_NMODE_ADDITIONAL
    GNEAdditionalFrame* getAdditionalFrame() const;

    /// @brief get frame for GNE_NMODE_CROSSING
    GNECrossingFrame* getCrossingFrame() const;

    /// @brief get frame for GNE_NMODE_TAZ
    GNETAZFrame* getTAZFrame() const;

    /// @brief get frame for GNE_NMODE_DELETE
    GNEDeleteFrame* getDeleteFrame() const;

    /// @brief get frame for GNE_NMODE_POLYGON
    GNEPolygonFrame* getPolygonFrame() const;

    /// @brief get frame for GNE_NMODE_PROHIBITION
    GNEProhibitionFrame* getProhibitionFrame() const;
    
    /// @brief get frame for GNE_NMODE_CREATEEDGE
    GNECreateEdgeFrame* getCreateEdgeFrame() const;

    /// @brief get frame for GNE_RMODE_ROUTE
    GNERouteFrame* getRouteFrame() const;

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
    /// @brief FOX needs this
    GNEViewParent() {}

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

        /// @brief frame for GNE_NMODE_INSPECT
        GNEInspectorFrame* inspectorFrame;

        /// @brief frame for GNE_NMODE_SELECT
        GNESelectorFrame* selectorFrame;

        /// @brief frame for GNE_NMODE_CONNECT
        GNEConnectorFrame* connectorFrame;

        /// @brief frame for GNE_NMODE_TLS
        GNETLSEditorFrame* TLSEditorFrame;

        /// @brief frame for GNE_NMODE_ADDITIONAL
        GNEAdditionalFrame* additionalFrame;

        /// @brief frame for GNE_NMODE_CROSSING
        GNECrossingFrame* crossingFrame;

        /// @brief frame for GNE_NMODE_TAZ
        GNETAZFrame* TAZFrame;

        /// @brief frame for GNE_NMODE_DELETE
        GNEDeleteFrame* deleteFrame;

        /// @brief frame for GNE_NMODE_POLYGON
        GNEPolygonFrame* polygonFrame;

        /// @brief frame for GNE_NMODE_PROHIBITION
        GNEProhibitionFrame* prohibitionFrame;

        /// @brief frame for GNE_NMODE_CREATEDGE
        GNECreateEdgeFrame* createEdgeFrame;

        /// @brief frame for GNE_RMODE_ROUTE
        GNERouteFrame* routeFrame;
    };

    /// @brief struct for ACChoosers dialog
    struct ACChoosers {
        /// @brief constructor
        ACChoosers();

        /// @brief destructor
        ~ACChoosers();

        /// @brief pointer to ACChooser dialog
        GNEDialogACChooser* ACChooserJunction;

        /// @brief pointer to ACChooser dialog
        GNEDialogACChooser* ACChooserEdges;

        /// @brief pointer to ACChooser dialog
        GNEDialogACChooser* ACChooserTLS;

        /// @brief pointer to ACChooser dialog
        GNEDialogACChooser* ACChooserAdditional;

        /// @brief pointer to ACChooser dialog
        GNEDialogACChooser* ACChooserPOI;

        /// @brief pointer to ACChooser dialog
        GNEDialogACChooser* ACChooserPolygon;

        /// @brief pointer to ACChooser dialog
        GNEDialogACChooser* ACChooserProhibition;
    };

    /// @brief pointer to GNEApplicationWindow
    GNEApplicationWindow* myGNEAppWindows;

    /// @brief frame to hold myView and myAttributePanel
    FXHorizontalFrame* myViewArea;

    /// @brief frame to hold GNEFrames
    FXHorizontalFrame* myFramesArea;

    /// @brief Splitter to divide ViewNet und GNEFrames
    FXSplitter* myFramesSplitter;

    /// @brief struct for frames
    Frames myFrames;

    /// @brief struct for ACChoosers
    ACChoosers myACChoosers;
};


#endif

/****************************************************************************/

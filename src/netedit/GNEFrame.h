/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// Abstract class for lateral frames in NetEdit
/****************************************************************************/
#ifndef GNEFrame_h
#define GNEFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBTrafficLightLogic.h>
#include <utils/gui/div/GUISelectedStorage.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEViewNet;
class GNEUndoList;
class GNEAttributeCarrier;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEFrame
 * Abstract class for lateral frames in NetEdit
 */
class GNEFrame : public FXVerticalFrame {

public:

    // ===========================================================================
    // class NeteditAttributes
    // ===========================================================================

    class NeteditAttributes : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrame::NeteditAttributes)

    public:
        /// @brief constructor
        NeteditAttributes(GNEFrame* frameParent);

        /// @brief destructor
        ~NeteditAttributes();

        /// @brief show NeteditAttributes
        void showNeteditAttributes(bool shapeEditing);

        /// @brief hide NeteditAttributes
        void hideNeteditAttributes();

        /// @brief check if block movement is enabled
        bool isBlockMovementEnabled() const;

        /// @brief check if block shape is enabled
        bool isBlockShapeEnabled() const;

        /// @brief check if clse shape is enabled
        bool isCloseShapeEnabled() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user changes the checkbox "set blocking movement"
        long onCmdSetBlockMovement(FXObject*, FXSelector, void*);

        /// @brief Called when user changes the checkbox "set blocking shape"
        long onCmdSetBlockShape(FXObject*, FXSelector, void*);

        /// @brief Called when the user change checkbox for open/closed polygon
        long onCmdsetClosingShape(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        NeteditAttributes() {}

    private:
        /// @brief Label for block movement
        FXLabel* myBlockMovementLabel;

        /// @brief checkBox for block movement
        FXCheckButton* myBlockMovementCheckButton;

        /// @brief frame for Block shape
        FXHorizontalFrame* myBlockShapeFrame;

        /// @brief Label for block shape
        FXLabel* myBlockShapeLabel;

        /// @brief checkBox for block shape
        FXCheckButton* myBlockShapeCheckButton;

        /// @brief Frame for open/close polygon
        FXHorizontalFrame* myClosePolygonFrame;

        /// @brief Label for open/close polygon
        FXLabel* myClosePolygonLabel;

        /// @brief checkbox to enable/disable closing polygon
        FXCheckButton* myClosePolygonCheckButton;
    };

    // ===========================================================================
    // class GEOAttributes
    // ===========================================================================

    class GEOAttributes : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrame::GEOAttributes)

    public:
        /// @brief constructor
        GEOAttributes(GNEFrame* frameParent);

        /// @brief destructor
        ~GEOAttributes();

        /// @brief show GEOAttribute for the current AttributeCarriers
        void showGEOAttributes(const std::vector<GNEAttributeCarrier*>& ACs);

        /// @brief hide GEOAttributes
        void hideGEOAttributes();

        /// @brief refresh TextFields with the new GEO Attributes
        void refreshGEOAttributes();

        /// @brief get GEO Attributes
        std::map<SumoXMLAttr, std::string> getGEOAttributes() const;

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when user change the current GEO Attribute
        long onCmdSetGEOAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when user enters a new length
        long onCmdUseGEOParameters(FXObject*, FXSelector, void*);

        /// @brief Called when user press the help button
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        GEOAttributes() {}

    private:
        /// @brief current GNEFrame parent
        GNEFrame* myFrameParent;

        /// @brief type of GEO Attribute
        SumoXMLAttr myGEOAttribute;

        /// @brief current edited ACs
        std::vector<GNEAttributeCarrier*> myACs;

        /// @brief horizontal frame for GEOAttribute
        FXHorizontalFrame* myGEOAttributeFrame;

        /// @brief Label for GEOAttribute
        FXLabel* myGEOAttributeLabel;

        /// @brief textField for GEOAttribute
        FXTextField* myGEOAttributeTextField;

        /// @brief horizontal frame for use GEO
        FXHorizontalFrame* myUseGEOFrame;

        /// @brief Label for use GEO
        FXLabel* myUseGEOLabel;

        /// @brief checkBox for use GEO
        FXCheckButton* myUseGEOCheckButton;

        /// @brief button for help
        FXButton* myHelpButton;
    };

    // ===========================================================================
    // class DrawingMode
    // ===========================================================================

    class DrawingMode : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrame::DrawingMode)

    public:
        /// @brief constructor
        DrawingMode(GNEFrame* frameParent);

        /// @brief destructor
        ~DrawingMode();

        /// @brief show Drawing mode
        void showDrawingMode();

        /// @brief hide Drawing mode
        void hideDrawingMode();

        /// @brief start drawing
        void startDrawing();

        /// @brief stop drawing and create polygon or
        void stopDrawing();

        /// @brief abort drawing
        void abortDrawing();

        /// @brief add new point to temporal shape
        void addNewPoint(const Position& P);

        /// @brief remove last added point
        void removeLastPoint();

        /// @brief get Temporal shape
        const PositionVector& getTemporalShape() const;

        /// @brief return true if currently a shape is drawed
        bool isDrawing() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user press start drawing button
        long onCmdStartDrawing(FXObject*, FXSelector, void*);

        /// @brief Called when the user press stop drawing button
        long onCmdStopDrawing(FXObject*, FXSelector, void*);

        /// @brief Called when the user press abort drawing button
        long onCmdAbortDrawing(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        DrawingMode() {}

    private:
        /// @brief frame frame parent
        GNEFrame* myFrameParent;

        /// @brief button for start drawing
        FXButton* myStartDrawingButton;

        /// @brief button for stop drawing
        FXButton* myStopDrawingButton;

        /// @brief button for abort drawing
        FXButton* myAbortDrawingButton;

        /// @brief Label with information
        FXLabel* myInformationLabel;

        /// @brief current drawed shape
        PositionVector myTemporalShapeShape;
    };


    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     * @brief frameLabel label of the frame
     */
    GNEFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet, const std::string& frameLabel);

    /// @brief destructor
    ~GNEFrame();

    /// @brief focus upper element of frame
    void focusUpperElement();

    /**@brief show Frame
     * @note some GNEFrames needs a re-implementation
     */
    virtual void show();

    /**@brief hide Frame
     * @note some GNEFrames needs a re-implementation
     */
    virtual void hide();

    /// @brief set width of GNEFrame
    void setFrameWidth(int newWidth);

    /// @brief get view net
    GNEViewNet* getViewNet() const;

    /// @brief get the label for the frame's header
    FXLabel* getFrameHeaderLabel() const;

    /// @brief get font of the header's frame
    FXFont* getFrameHeaderFont() const;

    /// @brief get netedit attributes editor
    GNEFrame::NeteditAttributes* getNeteditAttributes() const;

    /// @brief get GEO Parameters editor
    GNEFrame::GEOAttributes* getGEOAttributes() const;

    /// @brief get drawing mode editor
    GNEFrame::DrawingMode* getDrawingMode() const;

protected:
    /// @brief FOX needs this
    GNEFrame() {}

    /// @brief View Net for changes
    GNEViewNet* myViewNet;

    /// @brief Vertical frame that holds all widgets of frame
    FXVerticalFrame* myContentFrame;

    /// @brief fame for header elements
    FXHorizontalFrame* myHeaderFrame;

    /// @brief fame for left header elements
    FXHorizontalFrame* myHeaderLeftFrame;

    /// @brief fame for right header elements
    FXHorizontalFrame* myHeaderRightFrame;

    /// @brief Netedit parameter
    GNEFrame::NeteditAttributes* myNeteditAttributes;

    /// @brief GEO Parameters
    GNEFrame::GEOAttributes* myGEOAttributes;

    /// @brief drawing mode
    GNEFrame::DrawingMode* myDrawingMode;

private:
    /// @brief scroll windows that holds the content frame
    FXScrollWindow* myScrollWindowsContents;

    /// @brief Font for the Header
    FXFont* myFrameHeaderFont;

    /// @brief the label for the frame's header
    FXLabel* myFrameHeaderLabel;

    /// @brief Invalidated copy constructor.
    GNEFrame(const GNEFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFrame& operator=(const GNEFrame&) = delete;
};


#endif

/****************************************************************************/

/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEPolygonFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2017
/// @version $Id$
///
// The Widget for add polygons
/****************************************************************************/
#ifndef GNEPolygonFrame_h
#define GNEPolygonFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "GNEFrame.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAttributeCarrier;
class GNENetElement;
class GNEShape;

// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEPolygonFrame
* The Widget for setting internal attributes of shape elements
*/
class GNEPolygonFrame : public GNEFrame {

public:

    /// @brief enum with all possible values after try to create an shape using frame
    enum AddShapeResult {
        ADDSHAPE_SUCCESS,               // Shape was successfully created
        ADDSHAPE_UPDATEDTEMPORALSHAPE,  // Added or removed a new point to temporal shape
        ADDSHAPE_INVALID,               // Shape wasn't created
        ADDSHAPE_NOTHING                // Nothing to do
    };

    /// @brief class declaration
    class ShapeAttributes;

    // ===========================================================================
    // class ShapeSelector
    // ===========================================================================

    class ShapeSelector : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEPolygonFrame::ShapeSelector)

    public:
        /// @brief constructor
        ShapeSelector(GNEPolygonFrame* shapeFrameParent);

        /// @brief destructor
        ~ShapeSelector();

        /// @brief get current shape type
        SumoXMLTag getCurrentShapeType() const;

        /// @brief set parameters depending of the given shapeType
        void setCurrentShape(SumoXMLTag actualShapeType);

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another shape Type
        long onCmdselectAttributeCarrier(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        ShapeSelector() {}

    private:
        /// @brief pointer to Shape Frame Parent
        GNEPolygonFrame* myShapeFrameParent;

        /// @brief combo box with the list of shape elements
        FXComboBox* myShapeMatchBox;

        /// @brief actual shape type selected in the match Box
        SumoXMLTag myCurrentShapeType;
    };

    // ===========================================================================
    // class ShapeAttributeSingle
    // ===========================================================================

    class ShapeAttributeSingle : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEPolygonFrame::ShapeAttributeSingle)

    public:
        /// @brief constructor
        ShapeAttributeSingle(ShapeAttributes* shapeAttributesParent);

        /// @brief destructor
        ~ShapeAttributeSingle();

        /// @brief show name and value of attribute of type string
        void showParameter(SumoXMLAttr shapeAttr, std::string value);

        /// @brief hide all parameters
        void hideParameter();

        /// @brief return Attr
        SumoXMLAttr getAttr() const;

        /// @brief return value
        std::string getValue() const;

        /// @brief returns a empty string if current value is valid, a string with information about invalid value in other case
        const std::string& isAttributeValid() const;

        /// @brief get shape attributes parent
        ShapeAttributes* getShapeAttributesParent() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user set the value of an attribute of type int/float/string
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief called when user change the value of myBoolCheckButton
        long onCmdSetBooleanAttribute(FXObject*, FXSelector, void*);

        /// @brief called when user press the "Color" button
        long onCmdSetColorAttribute(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        ShapeAttributeSingle() {}

    private:
        /// @brief pointer to ShapeAttributes
        ShapeAttributes* myShapeAttributesParent;

        /// @brief current XML attribute
        SumoXMLAttr myShapeAttr;

        /// @brief lael with the name of the parameter
        FXLabel* myLabel;

        /// @brief textField to modify the default value of int/float/string parameters
        FXTextField* myTextFieldInt;

        /// @brief textField to modify the default value of real/times parameters
        FXTextField* myTextFieldReal;

        /// @brief textField to modify the default value of string parameters
        FXTextField* myTextFieldStrings;

        /// @brief check button to enable/disable the value of boolean parameters
        FXCheckButton* myBoolCheckButton;

        /// @brief Button for open color editor
        FXButton* myColorEditor;

        /// @brief string which indicates the reason due current value is invalid
        std::string myInvalidValue;
    };

    // ===========================================================================
    // class ShapeAttributes
    // ===========================================================================

    class ShapeAttributes : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEPolygonFrame::ShapeAttributes)

    public:
        /// @brief constructor
        ShapeAttributes(GNEPolygonFrame* polygonFrameParent);

        /// @brief destructor
        ~ShapeAttributes();

        /// @brief clear attributes
        void clearAttributes();

        /// @brief add attribute
        void addAttribute(SumoXMLAttr ShapeAttributeSingle);

        /// @brief show group box
        void showShapeParameters();

        /// @brief hide group box
        void hideShapeParameters();

        /// @brief get attributes and their values
        std::map<SumoXMLAttr, std::string> getAttributesAndValues() const;

        /// @brief check if parameters of attributes are valid
        bool areValuesValid() const;

        /// @brief show warning message with information about non-valid attributes
        void showWarningMessage(std::string extra = "") const;

        /// @brief get number of added attributes
        int getNumberOfAddedAttributes() const;

        /// @brief get PolygonFrame parent
        GNEPolygonFrame* getPolygonFrameParent() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        ShapeAttributes() {}

    private:
        /// @brief pointer to Polygon Frame Parent
        GNEPolygonFrame* myPolygonFrameParent;

        /// @brief vector with the shape parameters
        std::vector<ShapeAttributeSingle*> myVectorOfsingleShapeParameter;
    };

    // ===========================================================================
    // class NeteditAttributes
    // ===========================================================================

    class NeteditAttributes : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEPolygonFrame::NeteditAttributes)

    public:
        /// @brief constructor
        NeteditAttributes(GNEPolygonFrame* polygonFrameParent);

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
        long onCmdSetNeteditAttribute(FXObject*, FXSelector, void*);

        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
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
    // class DrawingMode
    // ===========================================================================

    class DrawingMode : private FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEPolygonFrame::DrawingMode)

    public:
        /// @brief constructor
        DrawingMode(GNEPolygonFrame* polygonFrameParent);

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

        /// @brief enable or disable delete last created point
        void setDeleteLastCreatedPoint(bool value);

        /// @brief get flag delete last created point
        bool getDeleteLastCreatedPoint();

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
        GNEPolygonFrame* myPolygonFrameParent;

        /// @brief flag to enable/disable delete point mode
        bool myDeleteLastCreatedPoint;

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
    */
    GNEPolygonFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEPolygonFrame();

    /**@brief process click over Viewnet
    * @param[in] clickedPosition clicked position over ViewNet
    * @param[in] lane clicked lane
    * @return AddShapeStatus with the result of operation
    */
    AddShapeResult processClick(const Position& clickedPosition, GNELane* lane);

    /**@brief build Polygon using values of Fields and drawed shape
     * return true if was sucesfully created
     * @note called when user stop drawing polygon
     */
    bool buildPoly(const PositionVector& drawedShape);

    /// @brief show shape frame and update use selected edges/lanes
    void show();

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

    /// @brief get shape selector
    ShapeSelector* getShapeSelector() const;

    /// @brief get shape attributes
    ShapeAttributes* getShapeAttributes() const;

    /// @brief get netedit attributes editor
    NeteditAttributes* getNeteditAttributes() const;

    /// @brief get drawing mode editor
    DrawingMode* getDrawingMode() const;

protected:
    /// @brief add Polygon
    bool addPolygon(const std::map<SumoXMLAttr, std::string>& POIValues);

    /// @brief add POI
    bool addPOI(const std::map<SumoXMLAttr, std::string>& POIValues);

    /// @brief add POILane
    bool addPOILane(const std::map<SumoXMLAttr, std::string>& POIValues);

private:
    /// @brief additional selector
    ShapeSelector* myShapeSelector;

    /// @brief shape internal attributes
    ShapeAttributes* myShapeAttributes;

    /// @brief Netedit parameter
    NeteditAttributes* myNeteditAttributes;

    /// @brief drawing mode
    DrawingMode* myDrawingMode;
};


#endif

/****************************************************************************/

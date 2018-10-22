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
#include "GNEFrame.h"

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

    class ShapeAttributes : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEPolygonFrame::ShapeAttributes)

        // declare friend class
        friend class ShapeAttributeSingle;

    public:
        /// @brief constructor
        ShapeAttributes(GNEPolygonFrame* polygonFrameParent);

        /// @brief destructor
        ~ShapeAttributes();

        /// @brief clear attributes
        void clearAttributes();

        /// @brief add attribute
        void addAttribute(SumoXMLAttr ShapeAttributeSingle);

        /// @brief show ShapeAttributes modul
        void showShapeAttributesModul();

        /// @brief hide group box
        void hideShapeAttributesModul();

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

    /// @brief show shape frame and update use selected edges/lanes
    void show();

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

    /// @brief get drawing mode editor
    DrawingShape* getDrawingShape() const;

protected:
    /**@brief build a shaped element using the drawed shape
     * return true if was sucesfully created
     * @note called when user stop drawing polygon
     */
    bool buildShape();

     /// @brief enable moduls depending of item selected in ItemSelector
    void enableModuls(const GNEAttributeCarrier::TagValues &tagValue);

    /// @brief disable moduls if element selected in itemSelector isn't valid
    void disableModuls();

    /// @brief add Polygon
    bool addPolygon(const std::map<SumoXMLAttr, std::string>& POIValues);

    /// @brief add POI
    bool addPOI(const std::map<SumoXMLAttr, std::string>& POIValues);

    /// @brief add POILane
    bool addPOILane(const std::map<SumoXMLAttr, std::string>& POIValues);

private:
    /// @brief item selector
    ItemSelector* myItemSelector;

    /// @brief shape internal attributes
    ShapeAttributes* myShapeAttributes;

    /// @brief Netedit parameter
    NeteditAttributes* myNeteditAttributes;

    /// @brief Drawing shape
    DrawingShape* myDrawingShape;
};


#endif

/****************************************************************************/

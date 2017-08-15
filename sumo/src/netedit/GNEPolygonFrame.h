/****************************************************************************/
/// @file    GNEPolygonFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2017
/// @version $Id: GNEPolygonFrame.h 25295 2017-07-22 17:55:46Z behrisch $
///
// The Widget for add polygons
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
#ifndef GNEPolygonFrame_h
#define GNEPolygonFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
* The Widget for setting internal attributes of additional elements
*/
class GNEPolygonFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNEPolygonFrame)

public:

    /// @brief enum with all possible values after try to create an additional using frame
    enum AddShapeResult {
        ADDSHAPE_SUCCESS,   // Shape was successfully created
        ADDSHAPE_INVALID    // Shape wasn't created
    };

    // ===========================================================================
    // class ShapeAttributeSingle
    // ===========================================================================

    class ShapeAttributeSingle : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEPolygonFrame::ShapeAttributeSingle)

    public:
        /// @brief constructor
        ShapeAttributeSingle(FXComposite* parent);

        /// @brief destructor
        ~ShapeAttributeSingle();

        /// @brief show name and value of attribute of type string
        void showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, std::string value);

        /// @brief show name and value of parameters of type int
        void showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, int value);

        /// @brief show name and value of parameters of type float/real/time
        void showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, double value);

        /// @brief show name and value of parameters of type bool
        void showParameter(SumoXMLTag additionalTag, SumoXMLAttr additionalAttr, bool value);

        /// @brief hide all parameters
        void hideParameter();

        /// @brief return tag
        SumoXMLTag getTag() const;

        /// @brief return Attr
        SumoXMLAttr getAttr() const;

        /// @brief return value
        std::string getValue() const;

        /// @brief returns a empty string if current value is valid, a string with information about invalid value in other case
        const std::string& isAttributeValid() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user set the value of an attribute of type int/float/string
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief called when user change the value of myBoolCheckButton
        long onCmdSetBooleanAttribute(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        ShapeAttributeSingle() {}

    private:
        /// @brief current XML attribute
        SumoXMLTag myShapeTag;

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
        ShapeAttributes(GNEViewNet* viewNet, FXComposite* parent);

        /// @brief destructor
        ~ShapeAttributes();

        /// @brief clear attributes
        void clearAttributes();

        /// @brief add attribute
        void addAttribute(SumoXMLTag additionalTag, SumoXMLAttr ShapeAttributeSingle);

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

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        ShapeAttributes() {}

    private:
        /// @brief pointer to viewNet
        GNEViewNet* myViewNet;

        /// @brief current additional tag
        SumoXMLTag myShapeTag;

        /// @brief vector with the additional parameters
        std::vector<ShapeAttributeSingle*> myVectorOfsingleShapeParameter;

        /// @brief Index for myVectorOfsingleShapeParameter
        int myIndexParameter;

        /// @brief index for myIndexParameterList
        int myIndexParameterList;

        /// @brief max number of parameters (Defined in constructor)
        int maxNumberOfParameters;

        /// @brief max number of parameters (Defined in constructor)
        int maxNumberOfListParameters;

        /// @brief button for help
        FXButton* helpShape;
    };

    // ===========================================================================
    // class NeteditAttributes
    // ===========================================================================

    class NeteditAttributes : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEPolygonFrame::NeteditAttributes)

    public:
        /// @brief constructor
        NeteditAttributes(FXComposite* parent);

        /// @brief destructor
        ~NeteditAttributes();

        /// @brief check if block movement is enabled
        bool isBlockMovementEnabled() const;

        /// @brief check if block shape is enabled
        bool isBlockShapeEnabled() const;

        /// @brief check if current length is valid
        bool isCurrentLengthValid() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user changes the checkbox "set blocking movement"
        long onCmdSetBlockMovement(FXObject*, FXSelector, void*);

        /// @brief Called when user changes the checkbox "set blocking shape"
        long onCmdSetBlockShape(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        NeteditAttributes() {}

    private:
        /// @brief Label for block movement
        FXLabel* myBlockMovementLabel;

        /// @brief checkBox for block movement
        FXCheckButton* myBlockMovementCheckButton;

        /// @brief Label for block shape
        FXLabel* myBlockShapeLabel;

        /// @brief checkBox for block shape
        FXCheckButton* myBlockShapeCheckButton;
    };


    // ===========================================================================
    // class DrawingMode
    // ===========================================================================

    class DrawingMode : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNEPolygonFrame::DrawingMode)

    public:
        /// @brief constructor
        DrawingMode(GNEPolygonFrame* polygonFrameParent);

        /// @brief destructor
        ~DrawingMode();

        /// @brief show Drawing mode
        void show();

        /// @brief show Drawing mode
        void hide();

        /// @brief start drawing
        void startDrawing();

        /// @brief stop drawing and create polygon or 
        void stopDrawing();

        /// @brief abort drawing
        void abortDrawing();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user select another additional Type
        long onCmdStartDrawing(FXObject*, FXSelector, void*);

        /// @brief Called when the user select another additional Type
        long onCmdStopDrawing(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        DrawingMode() {}

    private:
        /// @brief polygon frame parent
        GNEPolygonFrame* myPolygonFrameParent;

        /// @brief button for start drawing
        FXButton *myStartDrawingButton;

        /// @brief button for stop drawing
        FXButton *myStopDrawingButton;
        
        /// @brief current drawed shape
        PositionVector currentDrawedShape;
    };

    /**@brief Constructor
    * @brief parent FXHorizontalFrame in which this GNEFrame is placed
    * @brief viewNet viewNet that uses this GNEFrame
    */
    GNEPolygonFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEPolygonFrame();

    /**@brief process click ver Viewnet
    * @param[in] clickedPosition clicked position over ViewNet
    * @return AddShapeStatus with the result of operation
    */
    AddShapeResult processClick(Position clickedPosition);

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user select another additional Type
    long onCmdSelectShape(FXObject*, FXSelector, void*);
    /// @}

    /// @brief show additional frame and update use selected edges/lanes
    void show();

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

protected:
    /// @brief FOX needs this
    GNEPolygonFrame() {}

private:
    /// @brief set parameters depending of the new additionalType
    void setParametersOfShape(SumoXMLTag actualShapeType);

    /// @brief groupBox for Match Box of additionals
    FXGroupBox* myGroupBoxForMyShapeMatchBox;

    /// @brief combo box with the list of additional elements
    FXComboBox* myShapeMatchBox;

    /// @brief additional internal attributes
    GNEPolygonFrame::ShapeAttributes* myadditionalParameters;

    /// @brief Netedit parameter
    GNEPolygonFrame::NeteditAttributes* myEditorParameters;

    /// @brief drawing mode
    GNEPolygonFrame::DrawingMode* myDrawingMode;

    /// @brief actual additional type selected in the match Box
    SumoXMLTag myActualShapeType;
};


#endif

/****************************************************************************/

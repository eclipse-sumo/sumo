/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEShapeFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2017
///
// The Widget for add polygons
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <netedit/frames/GNEAttributesCreator.h>
#include <netedit/frames/GNETagSelector.h>
#include <netedit/frames/GNEDrawingShape.h>
#include <netedit/frames/GNENeteditAttributes.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
* @class GNEShapeFrame
* The Widget for setting internal attributes of shape elements
*/
class GNEShapeFrame : public GNEFrame {

public:

    // ===========================================================================
    // class GEOPOICreator
    // ===========================================================================

    class GEOPOICreator : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEShapeFrame::GEOPOICreator)

    public:
        /// @brief constructor
        GEOPOICreator(GNEShapeFrame* polygonFrameParent);

        /// @brief destructor
        ~GEOPOICreator();

        /// @brief Show list of GEOPOICreator Module
        void showGEOPOICreatorModule();

        /// @brief hide GEOPOICreator Module
        void hideGEOPOICreatorModule();

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user change the coordinates TextField
        long onCmdSetCoordinates(FXObject*, FXSelector, void*);

        /// @brief called when user select a format radio button
        long onCmdSetFormat(FXObject*, FXSelector, void*);

        /// @brief called when user type in search box
        long onCmdCreateGEOPOI(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(GEOPOICreator)

    private:
        /// @brief pointer to Shape frame parent
        GNEShapeFrame* myShapeFrameParent;

        /// @brief radio button for the configuration lon-lat
        FXRadioButton* myLonLatRadioButton;

        /// @brief radio button for the configuration lat-lon
        FXRadioButton* myLatLonRadioButton;

        /// @brief text field for given geo coordinates
        FXTextField* myCoordinatesTextField;

        /// @brief button for enable or disable certer view after creation of GEO POI
        FXCheckButton* myCenterViewAfterCreationCheckButton;

        /// @brief button for create GEO Coordinates
        FXButton* myCreateGEOPOIButton;

        /// @brief FXLabel for the equivalent position of GEO Position in Cartesian Position
        FXLabel* myLabelCartesianPosition;
    };

    /**@brief Constructor
    * @brief viewParent GNEViewParent in which this GNEFrame is placed
    * @brief viewNet viewNet that uses this GNEFrame
    */
    GNEShapeFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEShapeFrame();

    /// @brief show Frame
    void show();

    /**@brief process click over Viewnet
     * @param[in] clickedPosition clicked position over ViewNet
     * @param[in] ObjectsUnderCursor objects under cursor after click over view
     * @return AddShapeStatus with the result of operation
     */
    bool processClick(const Position& clickedPosition, const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor, bool& updateTemporalShape);

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

    /// @brief get drawing mode editor
    GNEDrawingShape* getDrawingShapeModule() const;

protected:
    /// @brief SumoBaseObject used for create shape
    CommonXMLStructure::SumoBaseObject* myBaseShape;

    // @brief create baseShapeObject
    void createBaseShapeObject(const SumoXMLTag shapeTag);

    /**@brief build a shaped element using the drawed shape
     * return true if was successfully created
     * @note called when user stop drawing polygon
     */
    bool shapeDrawed();

    /// @brief Tag selected in GNETagSelector
    void tagSelected();

    /// @brief add shape (using base shape)
    void addShape();

private:
    /// @brief shape tag selector
    GNETagSelector* myShapeTagSelector;

    /// @brief shape internal attributes
    GNEAttributesCreator* myShapeAttributes;

    /// @brief Netedit parameter
    GNENeteditAttributes* myNeteditAttributes;

    /// @brief Drawing shape
    GNEDrawingShape* myDrawingShape;

    /// @brief GEOPOICreator
    GEOPOICreator* myGEOPOICreator;
};

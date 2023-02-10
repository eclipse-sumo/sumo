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
/// @file    GNENeteditAttributes.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2022
///
// Modul for edit netedit attributes
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/xml/CommonXMLStructure.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;
class GNEAttributeCarrier;
class GNELane;

// ===========================================================================
// class definitions
// ===========================================================================

class GNENeteditAttributes : public MFXGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNENeteditAttributes)

public:
    /// @brief constructor
    GNENeteditAttributes(GNEFrame* frameParent);

    /// @brief destructor
    ~GNENeteditAttributes();

    /// @brief show Netedit attributes modul
    void showNeteditAttributesModule(GNEAttributeCarrier* templateAC);

    /// @brief hide Netedit attributes modul
    void hideNeteditAttributesModule();

    /// @brief fill valuesMap with netedit attributes
    bool getNeteditAttributesAndValues(CommonXMLStructure::SumoBaseObject* baseObject, const GNELane* lane) const;

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when user changes some element of GNENeteditAttributes
    long onCmdSetNeteditAttribute(FXObject*, FXSelector, void*);

    /// @brief Called when user press the help button
    long onCmdHelp(FXObject*, FXSelector, void*);
    /// @}

protected:
    FOX_CONSTRUCTOR(GNENeteditAttributes)

private:
    /// @brief list of the reference points
    enum class AdditionalReferencePoint {
        LEFT,
        RIGHT,
        CENTER,
        INVALID
    };

    /// @brief obtain the Start position values of StoppingPlaces and E2 detector over the lane
    double setStartPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const;

    /// @brief obtain the End position values of StoppingPlaces and E2 detector over the lane
    double setEndPosition(double positionOfTheMouseOverLane, double lengthOfAdditional) const;

    /// @brief pointer to frame parent
    GNEFrame* myFrameParent;

    /// @brief match box with the list of reference points
    FXComboBox* myReferencePointMatchBox;

    /// @brief horizontal frame for length
    FXHorizontalFrame* myLengthFrame;

    /// @brief textField for length
    FXTextField* myLengthTextField;

    /// @brief horizontal frame for close polygon
    FXHorizontalFrame* myCloseShapeFrame;

    /// @brief checkbox to enable/disable close polygon
    FXCheckButton* myCloseShapeCheckButton;

    /// @brief horizontal frame for center view after creation frame
    FXHorizontalFrame* myCenterViewAfterCreationFrame;

    /// @brief checkbox to enable/disable center element after creation
    FXCheckButton* myCenterViewAfterCreationButton;

    /// @brief Button for help about the reference point
    FXButton* helpReferencePoint;

    /// @brief Flag to check if current length is valid
    bool myCurrentLengthValid;

    /// @brief actual additional reference point selected in the match Box
    AdditionalReferencePoint myActualAdditionalReferencePoint;
};

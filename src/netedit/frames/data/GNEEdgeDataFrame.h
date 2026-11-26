/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNEEdgeDataFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2020
///
// The Widget for add edgeData elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <set>
#include <netedit/dialogs/GNEDialog.h>

#include "GNEGenericDataFrame.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEEdgeDataFrame
 * The Widget for setting internal attributes of additional elements
 */
class GNEEdgeDataFrame : public GNEGenericDataFrame {

public:
    /// @brief Dialog for changing edgeData values smoothly in an area
    class AreaDialog : public GNEDialog {
        /// @brief FOX-declaration
        FXDECLARE(GNEEdgeDataFrame::AreaDialog)

    public:
        /// @brief constructor
        AreaDialog(GNEApplicationWindow* applicationWindow, const Position& pos,
                   const std::set<std::string>& attributes, const std::string& defaultAttribute);

        /// @brief destructor
        ~AreaDialog();

        /// @brief check if dialog inputs are valid
        bool isValid() const;

        /// @brief get edited map position
        const Position& getEditedPosition() const;

        /// @brief get target attribute name
        const std::string& getAttributeName() const;

        /// @brief get value change at point
        double getDelta() const;

        /// @brief get range to zero
        double getRange() const;

        /// @brief check if smooth bell scaling is selected
        bool isSmooth() const;

        /// @brief run internal test
        void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) override;

        /// @name FOX-callbacks
        /// @{
        /// @brief event after changing an input field or scale mode
        long onCmdSetAttribute(FXObject* sender, FXSelector, void*);

        /// @brief event after pressing accept button
        long onCmdAccept(FXObject*, FXSelector, void*) override;

        /// @brief event after pressing reset button
        long onCmdReset(FXObject*, FXSelector, void*) override;
        /// @}

    protected:
        /// @brief FOX needs this
        FOX_CONSTRUCTOR(AreaDialog)

    private:
        /// @brief validate all dialog fields and update Accept button state
        void validateFields();

        /// @brief text field for X,Y position
        FXTextField* myPositionTextField = nullptr;

        /// @brief combo box for attribute name
        MFXComboBoxIcon* myAttributeComboBox = nullptr;

        /// @brief text field for change at point
        FXTextField* myDeltaTextField = nullptr;

        /// @brief text field for range to zero
        FXTextField* myRangeTextField = nullptr;

        /// @brief radio button for linear (cone) scaling
        FXRadioButton* myLinearRadioButton = nullptr;

        /// @brief radio button for smooth (bell) scaling
        FXRadioButton* mySmoothRadioButton = nullptr;

        /// @brief original position (for reset)
        Position myOriginalPosition;

        /// @brief original attribute name (for reset)
        std::string myOriginalAttribute;

        /// @brief edited position
        Position myEditedPosition;

        /// @brief selected or typed attribute name
        std::string myAttribute;

        /// @brief change at point
        double myDelta = 0.0;

        /// @brief range to zero
        double myRange = 100.0;

        /// @brief whether smooth (bell) scaling is selected
        bool mySmooth = false;

        /// @brief whether all fields are currently valid
        bool myValid = false;

        /// @brief Invalidated copy constructor
        AreaDialog(const AreaDialog&) = delete;

        /// @brief Invalidated assignment operator
        AreaDialog& operator=(const AreaDialog&) = delete;
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEEdgeDataFrame
     */
    GNEEdgeDataFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEEdgeDataFrame();

    /**@brief add additional element
     * @param viewObjects collection of objects under cursor after click over view
     * @param mouseButtonKeyPressed key pressed during click
     * @return true if additional was successfully added
     */
    bool addEdgeData(const GNEViewNetHelper::ViewObjectsSelector& viewObjects, const GNEViewNetHelper::MouseButtonKeyPressed& mouseButtonKeyPressed);

    /// @brief open dialog to change edgeData values in an area around pos
    void openAreaDialog(const Position& pos);

private:
    /// @brief Invalidated copy constructor.
    GNEEdgeDataFrame(const GNEEdgeDataFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNEEdgeDataFrame& operator=(const GNEEdgeDataFrame&) = delete;
};

/****************************************************************************/

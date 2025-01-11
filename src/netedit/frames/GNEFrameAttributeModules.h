/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    GNEFrameAttributeModules.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2019
///
// Auxiliar class for GNEFrame Modules (only for attributes edition)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/elements/GNEAttributeCarrier.h>
#include <netedit/GNEViewNetHelper.h>
#include <utils/common/Parameterised.h>
#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/foxtools/MFXToggleButtonTooltip.h>
#include <utils/foxtools/MFXTextFieldTooltip.h>
#include <utils/foxtools/MFXLabelTooltip.h>
#include <utils/xml/CommonXMLStructure.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEFrame;
class GNEViewParent;
class GNEFlowEditor;
class GNEInspectorFrame;
class GNETypeFrame;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEFrameAttributeModules {

public:
    // ===========================================================================
    // class declaration
    // ===========================================================================

    class AttributesEditor;

    // ===========================================================================
    // class GenericDataAttributes
    // ===========================================================================

    class GenericDataAttributes : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::GenericDataAttributes)

    public:
        /// @brief constructor
        GenericDataAttributes(GNEFrame* frameParent);

        /// @brief destructor
        ~GenericDataAttributes();

        /// @brief show netedit attributes EditorCreator
        void showGenericDataAttributes();

        /// @brief hide netedit attributes EditorCreator
        void hideGenericDataAttributes();

        /// @brief refresh netedit attributes
        void refreshGenericDataAttributes();

        /// @brief get parameters as map
        const Parameterised::Map& getParametersMap() const;

        /// @brief get parameters as string
        std::string getParametersStr() const;

        /// @brief get parameters as vector of strings
        std::vector<std::pair<std::string, std::string> > getParameters() const;

        /// @brief set parameters
        void setParameters(const std::vector<std::pair<std::string, std::string> >& parameters);

        /// @brief pointer to frame parent
        GNEFrame* getFrameParent() const;

        /// @brief check if current attributes are valid
        bool areAttributesValid() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user clicks over add parameter
        long onCmdEditParameters(FXObject*, FXSelector, void*);

        /// @brief Called when user udpate the parameter text field
        long onCmdSetParameters(FXObject*, FXSelector, void*);
        /// @}

    protected:
        FOX_CONSTRUCTOR(GenericDataAttributes)

    private:
        /// @brief pointer to frame parent
        GNEFrame* myFrameParent = nullptr;

        /// @brief pointer to current map of parameters
        Parameterised::Map myParameters;

        /// @brief text field for write parameters
        FXTextField* myTextFieldParameters = nullptr;

        /// @brief button for edit parameters using specific dialog
        FXButton* myButtonEditParameters = nullptr;
    };

    // ===========================================================================
    // class ParametersEditor
    // ===========================================================================

    class ParametersEditor : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNEFrameAttributeModules::ParametersEditor)

    public:
        /// @brief constructor for inspector frame
        ParametersEditor(GNEInspectorFrame* inspectorFrameParent);

        /// @brief constructor for type frame
        ParametersEditor(GNETypeFrame* typeFrameParent);

        /// @brief destructor
        ~ParametersEditor();

        /// @get viewNet
        GNEViewNet* getViewNet() const;

        /// @brief show netedit attributes EditorInspector
        void showParametersEditor();

        /// @brief hide netedit attributes EditorInspector
        void hideParametersEditor();

        /// @brief refresh netedit attributes
        void refreshParametersEditor();

        /// @brief get inspector frame parent
        GNEInspectorFrame* getInspectorFrameParent() const;

        /// @brief get type frame parent
        GNETypeFrame* getTypeFrameParent() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when user clicks over add parameter
        long onCmdEditParameters(FXObject*, FXSelector, void*);

        /// @brief Called when user udpate the parameter text field
        long onCmdSetParameters(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(ParametersEditor)

    private:
        /// @brief inspector frame parent
        GNEInspectorFrame* myInspectorFrameParent = nullptr;

        /// @brief type frame parent
        GNETypeFrame* myTypeFrameParent = nullptr;

        /// @brief text field for write parameters
        FXTextField* myTextFieldParameters = nullptr;

        /// @brief button for edit parameters using specific dialog
        FXButton* myButtonEditParameters = nullptr;
    };

    /// @brief return true if AC can be edited in the current supermode
    static bool isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeCarrier* AC);

    /// @brief return true if give ACAttr can be edited in the current supermode
    static bool isSupermodeValid(const GNEViewNet* viewNet, const GNEAttributeProperties& ACAttr);
};

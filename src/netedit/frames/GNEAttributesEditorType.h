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
/// @file    GNEAttributesEditorType.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2024
///
// Table used for pack GNEAttributeRows
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <map>
#include <unordered_set>
#include <netedit/frames/common/GNEGroupBoxModule.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/CommonXMLStructure.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEAttributeCarrier;
class GNEAttributesEditor;
class GNEAttributesEditorRow;
class GNEFrame;
class MFXButtonTooltip;

// ===========================================================================
// class GNEAttributesEditorType
// ===========================================================================

class GNEAttributesEditorType : public GNEGroupBoxModule {
    /// @brief FOX-declaration
    FXDECLARE(GNEAttributesEditorType)

    /// @brief declare friend class
    friend class GNEAttributesEditorRow;

public:

    /// @brief Editor type
    enum class EditorType {
        CREATOR,    // used in create elements frames
        EDITOR      // used in edit frames (inspector, vType..)
    };

    /// @brief Attribute types
    enum class AttributeType {
        BASIC,      // basic attributes
        EXTENDED,   // extended attributes (used in vType)
        FLOW,       // flow attributes
        GEO,        // GEO attributes (lon and lat)
        NETEDIT,    // Netedit attributes (front, select...)
        PARAMETERS, // Generic parameters
    };

    /// @brief constructor
    GNEAttributesEditorType(GNEFrame* frameParent, GNEAttributesEditor* attributesEditorParent,
                            const std::string attributesEditorName, EditorType editorType,
                            AttributeType attributeType);

    /// @brief constructor
    ~GNEAttributesEditorType();

    /// @brief pointer to GNEFrame parent
    GNEFrame* getFrameParent() const;

    /// @brief check if this is an attribute editor of type "creator"
    bool isEditorTypeCreator() const;

    /// @brief check if this is an attribute editor of type "editor"
    bool isEditorTypeEditor() const;

    /// @brief get edited attribute carriers
    const std::vector<GNEAttributeCarrier*>& getEditedAttributeCarriers() const;

    /// @brief edit attributes of the given AC (usually the edited template AC)
    void showAttributesEditor(GNEAttributeCarrier* AC, const bool primaryAttributeEditor);

    /// @brief edit attributes of the given hash of ACs (usually the inspected ACs)
    void showAttributesEditor(const std::unordered_set<GNEAttributeCarrier*>& ACs, const bool primaryAttributeEditor);

    /// @brief hide attribute editor
    void hideAttributesEditor();

    /// @brief refresh attribute editor
    void refreshAttributesEditor();

    /// @brief disable attribute editor
    void disableAttributesEditor();

    /// @brief check if current edited attributes are valid
    bool checkAttributes(const bool showWarning);

    /// @brief fill sumo Base object
    SumoXMLAttr fillSumoBaseObject(CommonXMLStructure::SumoBaseObject* baseObject, const bool insertDefaultValues) const;

    /// @name Functions related with selecting parents
    /// @{
    /// @brief check if we're selecting a parent clicking over view
    bool isReparenting() const;

    /// @brief check if the given AC can be a new parent
    bool checkNewParent(const GNEAttributeCarrier* AC) const;

    /// @brief set new parent
    void setNewParent(const GNEAttributeCarrier* AC);

    /// @brief abort selecting parent
    void abortReparenting();

    /// @}

    /// @name FOX-callbacks
    /// @{
    /// @brief called when user press the "Mark as front element" button
    long onCmdMarkAsFront(FXObject*, FXSelector, void*);

    /// @brief called when user update the "Mark as front element" button
    long onUpdMarkAsFront(FXObject*, FXSelector, void*);

    /// @brief called when user press the "Element dialog" button
    long onCmdOpenElementDialog(FXObject*, FXSelector, void*);

    /// @brief called when user press the "Extended attributes" button
    long onCmdOpenExtendedAttributesDialog(FXObject*, FXSelector, void*);

    /// @brief called when user press the "Edit parameters" button
    long onCmdOpenEditParametersDialog(FXObject*, FXSelector, void*);

    /// @brief called when user press the help button
    long onCmdAttributesEditorHelp(FXObject*, FXSelector, void*);

    /// @brief called when user press the reset button
    long onCmdAttributesEditorReset(FXObject*, FXSelector, void*);

    /// @}

protected:
    /// @brief fox need this
    FOX_CONSTRUCTOR(GNEAttributesEditorType)

    /// @name functions called from GNEAttributesEditorRow
    /// @{

    /// @brief set attribute in the current ACs (Callend from row)
    void setAttribute(SumoXMLAttr attr, const std::string& value);

    /// @brief set attribute in the current ACs (Callend from row)
    void toggleEnableAttribute(SumoXMLAttr attr, const bool value);

    /// @brief void enable reparent
    void enableReparent();

    /// @brief inspect parent (Callend from row)
    void inspectParent();

    /// @brief move lane up
    void moveLaneUp();

    /// @brief move lane down
    void moveLaneDown();

    /// @}

    /// @brief fill start end attributes
    void fillStartEndAttributes(CommonXMLStructure::SumoBaseObject* baseObject) const;

private:
    /// @brief typedef used for pack attributes editor row
    typedef std::map<AttributeType, std::vector<GNEAttributesEditorRow*> > AttributesEditorRows;

    /// @brief pointer to GNEFrame parent
    GNEFrame* myFrameParent;

    /// @brief pointer to GNEAttributesEditor parent
    GNEAttributesEditor* myAttributesEditorParent;

    /// @brief pointer to front button
    MFXButtonTooltip* myFrontButton = nullptr;

    /// @brief pointer to open dialog button (usually additionals)
    MFXButtonTooltip* myOpenDialogButton = nullptr;

    /// @brief pointer to open extended attributes button
    MFXButtonTooltip* myOpenExtendedAttributesButton = nullptr;

    /// @brief pointer to open generic parameters editor button
    MFXButtonTooltip* myOpenGenericParametersEditorButton = nullptr;

    /// @brief frame for netedit buttons (helps and reset)
    FXHorizontalFrame* myFrameNeteditButtons = nullptr;

    /// @brief first singleton with attributes editor rows
    static AttributesEditorRows myFirstSingletonAttributesEditorRows;

    /// @brief second singleton with attributes editor rows
    static AttributesEditorRows mySecondSingletonAttributesEditorRows;

    /// @brief current edited ACs
    std::vector<GNEAttributeCarrier*> myEditedACs;

    /// @brief singleton with attributes editor rows
    std::vector<GNEAttributesEditorRow*> myAttributesEditorRows;

    /// @brief check if we're reparent
    SumoXMLTag myReparentTag = SUMO_TAG_NOTHING;

    /// @brief variable use for packing editorType type options
    const EditorType myEditorType = EditorType::EDITOR;

    /// @brief variable use for packing attribute type options
    const AttributeType myAttributeType = AttributeType::BASIC;

    /// @brief build rows
    static void buildRows(GNEAttributesEditorType* editorParent);

    /// @brief Invalidated copy constructor.
    GNEAttributesEditorType(GNEAttributesEditorType*) = delete;

    /// @brief Invalidated assignment operator.
    GNEAttributesEditorType& operator=(GNEAttributesEditorType*) = delete;
};

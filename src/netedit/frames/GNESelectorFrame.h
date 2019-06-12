/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNESelectorFrame.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// The Widget for modifying selections of network-elements
// (some elements adapted from GUIDialog_GLChosenEditor)
/****************************************************************************/
#ifndef GNESelectorFrame_h
#define GNESelectorFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#include "GNEFrame.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNESelectorFrame
 * The Widget for modifying selections of network-elements
 */
class GNESelectorFrame : public GNEFrame {

public:

    // ===========================================================================
    // class LockGLObjectTypes
    // ===========================================================================

    class LockGLObjectTypes : protected FXGroupBox {

    public:
        /// @brief class for object types entries
        class ObjectTypeEntry : protected FXObject {
            /// @brief FOX-declaration
            FXDECLARE(GNESelectorFrame::LockGLObjectTypes::ObjectTypeEntry)

        public:
            /// @brief constructor
            ObjectTypeEntry(FXMatrix* matrixParent, const std::string& label);

            /// @brief show ObjectTypeEntry
            void showObjectTypeEntry();

            /// @brief hide ObjectTypeEntry
            void hideObjectTypeEntry();

            /// @brief up count
            void counterUp();

            /// @brief down count
            void counterDown();

            /// @brief check if current GLType is blocked
            bool isGLTypeLocked() const;

            /// @name FOX-callbacks
            /// @{
            /// @brief called when user change the CheckBox
            long onCmdSetCheckBox(FXObject*, FXSelector, void*);

            /// @}

        protected:
            /// @brief FOX needs this
            ObjectTypeEntry() {}

        private:
            /// @brief label counter
            FXLabel* myLabelCounter;

            /// @brief label type nane
            FXLabel* myLabelTypeName;

            /// @brief check box to check if GLObject type is blocked
            FXCheckButton* myCheckBoxLocked;

            /// @brief counter
            int myCounter;
        };

        /// @brief constructor
        LockGLObjectTypes(GNESelectorFrame* selectorFrameParent);

        /// @brief destructor
        ~LockGLObjectTypes();

        /// @brief set object selected
        void addedLockedObject(const GUIGlObjectType type);

        /// @brief set object unselected
        void removeLockedObject(const GUIGlObjectType type);

        /// @brief check if an object is locked
        bool IsObjectTypeLocked(const GUIGlObjectType type) const;

        /// @brief show type Entries (depending if we're in Network or demand supermode)
        void showTypeEntries();

    private:
        /// @brief pointer to Selector Frame Parent
        GNESelectorFrame* mySelectorFrameParent;

        /// @brief check boxes for type-based selection locking and selected object counts
        std::map<GUIGlObjectType, std::pair<Supermode, ObjectTypeEntry* > > myTypeEntries;
    };

    // ===========================================================================
    // class ModificationMode
    // ===========================================================================

    class ModificationMode : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNESelectorFrame::ModificationMode)

    public:
        /// @brief operations of selector
        enum SetOperation {
            SET_ADD      = 1,
            SET_SUB      = 2,
            SET_RESTRICT = 3,
            SET_REPLACE  = 4,
            SET_DEFAULT  = 5  // use mySetOperation instead of override
        };

        /// @brief constructor
        ModificationMode(GNESelectorFrame* selectorFrameParent);

        /// @brief destructor
        ~ModificationMode();

        /// @brief get current modification mode
        SetOperation getModificationMode() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user change type of selction operation
        long onCmdSelectModificationMode(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        ModificationMode() {}

    private:
        /// @brief add radio button
        FXRadioButton* myAddRadioButton;

        /// @brief remove radio button
        FXRadioButton* myRemoveRadioButton;

        /// @brief keep button
        FXRadioButton* myKeepRadioButton;

        /// @brief replace radio button
        FXRadioButton* myReplaceRadioButton;

        /// @brief how to modify selection
        SetOperation myModificationModeType;
    };

    // ===========================================================================
    // class ElementSet
    // ===========================================================================

    class ElementSet : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNESelectorFrame::ElementSet)

    public:
        /// @brief type of Set
        enum ElementSetType {
            ELEMENTSET_NETELEMENT    = 1,
            ELEMENTSET_ADDITIONAL    = 2,
            ELEMENTSET_SHAPE         = 3,
            ELEMENTSET_DEMANDELEMENT = 4,
            ELEMENTSET_INVALID       = 5,
        };

        /// @brief constructor
        ElementSet(GNESelectorFrame* selectorFrameParent);

        /// @brief destructor
        ~ElementSet();

        /// @brief get current selected element set
        ElementSetType getElementSet() const;

        /// @brief refresh element set
        void refreshElementSet();

        /// @brief update current element set (called after

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when the user change the set of element to search (netElement, Additional or shape)
        long onCmdSelectElementSet(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        ElementSet() {}

    private:
        /// @brief pointer to Selector Frame Parent
        GNESelectorFrame* mySelectorFrameParent;

        /// @brief Combo Box with the element sets
        FXComboBox* mySetComboBox;

        /// @brief current element set selected
        ElementSetType myCurrentElementSet;
    };

    // ===========================================================================
    // class MatchAttribute
    // ===========================================================================

    class MatchAttribute : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNESelectorFrame::MatchAttribute)

    public:
        /// @brief constructor
        MatchAttribute(GNESelectorFrame* selectorFrameParent);

        /// @brief destructor
        ~MatchAttribute();

        /// @brief enable match attributes
        void enableMatchAttribute();

        /// @brief disable match attributes
        void disableMatchAttribute();

        /// @name FOX-callbacks
        /// @{

        /**@brief Called when the user selectes a tag in the match box
         * @note updates the attr listbox and repaints itself
         */
        long onCmdSelMBTag(FXObject*, FXSelector, void*);

        /**@brief Called when the user selectes a tag in the match box
         * @note updates the attr listbox and repaints itself
         */
        long onCmdSelMBAttribute(FXObject*, FXSelector, void*);

        /**@brief Called when the user enters a new selection expression
         * @note validates expression and modifies current selection
         */
        long onCmdSelMBString(FXObject*, FXSelector, void*);

        /**@brief Called when the user clicks the help button
         * @note pop up help window
         */
        long onCmdHelp(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        MatchAttribute() {}

    private:
        /// @brief pointer to Selector Frame Parent
        GNESelectorFrame* mySelectorFrameParent;

        /// @brief tag of the match box
        FXComboBox* myMatchTagComboBox;

        /// @brief attributes of the match box
        FXComboBox* myMatchAttrComboBox;

        /// @brief current SumoXMLTag tag
        SumoXMLTag myCurrentTag;

        /// @brief current SumoXMLTag Attribute
        SumoXMLAttr myCurrentAttribute;

        /// @brief string of the match
        FXTextField* myMatchString;
    };

    // ===========================================================================
    // class VisualScaling
    // ===========================================================================

    class VisualScaling : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNESelectorFrame::VisualScaling)

    public:
        /// @brief constructor
        VisualScaling(GNESelectorFrame* selectorFrameParent);

        /// @brief destructor
        ~VisualScaling();

        /// @name FOX-callbacks
        /// @{

        /// @brief Called when the user changes visual scaling
        long onCmdScaleSelection(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        VisualScaling() {}

    private:
        /// @brief pointer to Selector Frame Parent
        GNESelectorFrame* mySelectorFrameParent;

        /// @brief Spinner for selection scaling
        FXRealSpinner* mySelectionScaling;
    };

    // ===========================================================================
    // class SelectionOperation
    // ===========================================================================

    class SelectionOperation : protected FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNESelectorFrame::SelectionOperation)

    public:
        /// @brief constructor
        SelectionOperation(GNESelectorFrame* selectorFrameParent);

        /// @brief destructor
        ~SelectionOperation();

        /// @name FOX-callbacks
        /// @{

        /**@brief Called when the user presses the Load-button
         * @note Opens a file dialog and forces the parent to load the list of selected
         * objects when a file was chosen. Rebuilds the list, then, and redraws itself.
         */
        long onCmdLoad(FXObject*, FXSelector, void*);

        /** @brief Called when the user presses the Save-button
         * @note Opens a file dialog and forces the selection container to save the list
           of selected objects when a file was chosen. If the saveing failed, a message window is shown.
         */
        long onCmdSave(FXObject*, FXSelector, void*);

        /**@brief Called when the user presses the Clear-button
         * @note Clear the internal list and calls GUISelectedStorage::clear and repaints itself
         */
        long onCmdClear(FXObject*, FXSelector, void*);

        /**@brief Called when the user presses the Invert-button
         * @note invert the selection and repaints itself
         */
        long onCmdInvert(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX needs this
        SelectionOperation() {}

    private:
        /// @brief pointer to Selector Frame Parent
        GNESelectorFrame* mySelectorFrameParent;
    };

    /**@brief Constructor
     * @brief parent FXHorizontalFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNESelectorFrame(FXHorizontalFrame* horizontalFrameParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNESelectorFrame();

    /// @brief show Frame
    void show();

    /// @brief hide Frame
    void hide();

    /// @brief clear current selection with possibility of undo/redo
    void clearCurrentSelection() const;

    /**@brief apply list of ids to the current selection according to SetOperation,
     * @note if setop==SET_DEFAULT than the currently set mode (mySetOperation) is used
     */
    void handleIDs(const std::vector<GNEAttributeCarrier*>& ACs, ModificationMode::SetOperation setop = ModificationMode::SET_DEFAULT);

    /// @brief get selected items Modul
    LockGLObjectTypes* getLockGLObjectTypes() const;

    /// @brief get modification mode modul
    ModificationMode* getModificationModeModul() const;

private:
    /// @brief modul for lock selected items
    LockGLObjectTypes* myLockGLObjectTypes;

    /// @brief modul for change modification mode
    ModificationMode* myModificationMode;

    /// @brief modul for select element set
    ElementSet* myElementSet;

    /// @brief modul for matchA ttribute
    MatchAttribute* myMatchAttribute;

    /// @brief modul for visual scaling
    VisualScaling* myVisualScaling;

    /// @brief modul for selection operations
    SelectionOperation* mySelectionOperation;

    /// @brief check if there is ACs to select/unselect
    bool ACsToSelected() const;

private:
    /**@brief return ACs of the given type with matching attrs
     * @param[in] ACTag XML Tag of AttributeCarrier
     * @param[in] ACAttr XML Attribute of AttributeCarrier
     * @param[in] compOp One of {<,>,=} for matching against val or '@' for matching against expr
     */
    std::vector<GNEAttributeCarrier*> getMatches(SumoXMLTag ACTag, SumoXMLAttr ACAttr, char compOp, double val, const std::string& expr);
};


#endif

/****************************************************************************/


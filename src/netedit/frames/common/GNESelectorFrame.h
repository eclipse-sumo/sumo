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
/// @file    GNESelectorFrame.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
///
// The Widget for modifying selections of network-elements
// (some elements adapted from GUIDialog_GLChosenEditor)
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/frames/GNEFrame.h>
#include <netedit/GNEViewNetHelper.h>
#include <utils/foxtools/MFXComboBoxIcon.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEElementSet;
class GNEMatchAttribute;
class GNEMatchGenericDataAttribute;

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
    // class SelectionInformation
    // ===========================================================================

    class SelectionInformation : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        SelectionInformation(GNESelectorFrame* selectorFrameParent);

        /// @brief destructor
        ~SelectionInformation();

        /// @brief update information label
        void updateInformationLabel();

    protected:
        /// @brief update information label
        void updateInformationLabel(const std::string& element, int number);

    private:
        /// @brief  string for keep information
        std::string myInformation;

        /// @brief information label
        FXLabel* myInformationLabel;

        /// @brief pointer to Selector Frame Parent
        GNESelectorFrame* mySelectorFrameParent;

        /// @brief Invalidated copy constructor.
        SelectionInformation(const SelectionInformation&) = delete;

        /// @brief Invalidated assignment operator.
        SelectionInformation& operator=(const SelectionInformation&) = delete;
    };

    // ===========================================================================
    // class ModificationMode
    // ===========================================================================

    class ModificationMode : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNESelectorFrame::ModificationMode)

    public:
        /// @brief operations of selector
        enum class Operation {
            ADD,
            SUB,
            RESTRICT,
            REPLACE,
            DEFAULT
        };

        /// @brief constructor
        ModificationMode(GNESelectorFrame* selectorFrameParent);

        /// @brief destructor
        ~ModificationMode();

        /// @brief get current modification mode
        Operation getModificationMode() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user change type of selection operation
        long onCmdSelectModificationMode(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(ModificationMode)

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
        Operation myModificationModeType;

        /// @brief Invalidated copy constructor.
        ModificationMode(const ModificationMode&) = delete;

        /// @brief Invalidated assignment operator.
        ModificationMode& operator=(const ModificationMode&) = delete;
    };

    // ===========================================================================
    // class VisualScaling
    // ===========================================================================

    class VisualScaling : public MFXGroupBoxModule {
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
        /// @brief FOX need this
        FOX_CONSTRUCTOR(VisualScaling)

    private:
        /// @brief pointer to Selector Frame Parent
        GNESelectorFrame* mySelectorFrameParent;

        /// @brief Spinner for selection scaling
        FXRealSpinner* mySelectionScaling;

        /// @brief Invalidated copy constructor.
        VisualScaling(const VisualScaling&) = delete;

        /// @brief Invalidated assignment operator.
        VisualScaling& operator=(const VisualScaling&) = delete;
    };

    // ===========================================================================
    // class SelectionOperation
    // ===========================================================================

    class SelectionOperation : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNESelectorFrame::SelectionOperation)

    public:
        /// @brief constructor
        SelectionOperation(GNESelectorFrame* selectorFrameParent);

        /// @brief destructor
        ~SelectionOperation();

        /// @brief load from file
        void loadFromFile(const std::string& file) const;

        /// @name FOX-callbacks
        /// @{

        /**@brief Called when the user presses the Load-button
         * @note Opens a file dialog and forces the parent to load the list of selected
         * objects when a file was chosen. Rebuilds the list, then, and redraws itself.
         */
        long onCmdLoad(FXObject*, FXSelector, void*);

        /** @brief Called when the user presses the Save-button
         * @note Opens a file dialog and forces the selection container to save the list
           of selected objects when a file was chosen. If the saving failed, a message window is shown.
         */
        long onCmdSave(FXObject*, FXSelector, void*);

        /**@brief Called when the user presses the Clear-button
         * @note Clear the internal list and calls GUISelectedStorage::clear and repaints itself
         */
        long onCmdClear(FXObject*, FXSelector, void*);

        /**@brief Called when the user presses the delete-button
         */
        long onCmdDelete(FXObject*, FXSelector, void*);

        /**@brief Called when the user presses the Invert-button
         * @note invert the selection and repaints itself
         */
        long onCmdInvert(FXObject*, FXSelector, void*);

        /**@brief Called when the user presses the Reduce-button
         * @note Reduce network
         */
        long onCmdReduce(FXObject*, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(SelectionOperation)

        /// @brief process network element selection
        bool processNetworkElementSelection(const bool onlyCount, const bool onlyUnselect, bool& ignoreLocking);

        /// @brief process demand element selection
        bool processDemandElementSelection(const bool onlyCount, const bool onlyUnselect, bool& ignoreLocking);

        /// @brief process data element selection
        bool processDataElementSelection(const bool onlyCount, const bool onlyUnselect, bool& ignoreLocking);

        /// @brief ask if continue due locking
        bool askContinueIfLock() const;

    private:
        /// @brief pointer to Selector Frame Parent
        GNESelectorFrame* mySelectorFrameParent;

        /// @brief Invalidated copy constructor.
        SelectionOperation(const SelectionOperation&) = delete;

        /// @brief Invalidated assignment operator.
        SelectionOperation& operator=(const SelectionOperation&) = delete;
    };

    // ===========================================================================
    // class SelectionHierarchy
    // ===========================================================================

    class SelectionHierarchy : public MFXGroupBoxModule {
        /// @brief FOX-declaration
        FXDECLARE(GNESelectorFrame::SelectionHierarchy)

    public:
        /// @brief constructor
        SelectionHierarchy(GNESelectorFrame* selectorFrameParent);

        /// @brief destructor
        ~SelectionHierarchy();

        /// @name FOX-callbacks
        /// @{

        /// @brief called when user select an item in comboBox
        long onCmdSelectItem(FXObject* obj, FXSelector, void*);

        /// @brief called when user press select/unselect parents button
        long onCmdParents(FXObject* obj, FXSelector, void*);

        /// @brief called when user press select/unselect children button
        long onCmdChildren(FXObject* obj, FXSelector, void*);

        /// @}

    protected:
        /// @brief FOX need this
        FOX_CONSTRUCTOR(SelectionHierarchy)

    private:
        /// @brief enum used in comboBox
        enum class Selection {
            ALL,
            JUNCTION,
            EDGE,
            LANE,
            CONNECTION,
            CROSSING,
            ADDITIONAL,
            WIRE,
            SHAPE,
            DEMAND,
            DATA,
            NOTHING,
        };

        /// @brief pointer to Selector Frame Parent
        GNESelectorFrame* mySelectorFrameParent;

        /// @brief comboBox for parents
        FXComboBox* myParentsComboBox = nullptr;

        /// @brief comboBox for children
        FXComboBox* myChildrenComboBox = nullptr;

        /// @brief select parents button
        FXButton* mySelectParentsButton = nullptr;

        /// @brief unselect parents button
        FXButton* myUnselectParentsButton = nullptr;

        /// @brief select children button
        FXButton* mySelectChildrenButton = nullptr;

        /// @brief unselect parents button
        FXButton* myUnselectChildrenButton = nullptr;

        // @brief items
        const std::vector<std::pair<Selection, std::string> > myItems = {
            std::make_pair(Selection::ALL, "all"),
            std::make_pair(Selection::JUNCTION, "junction"),
            std::make_pair(Selection::EDGE, "edge"),
            std::make_pair(Selection::LANE, "lane"),
            std::make_pair(Selection::CONNECTION, "connection"),
            std::make_pair(Selection::CROSSING, "crossing"),
            std::make_pair(Selection::ADDITIONAL, "additionalElements"),
            std::make_pair(Selection::WIRE, "wireElements"),
            std::make_pair(Selection::SHAPE, "shapeElements"),
            std::make_pair(Selection::DEMAND, "demandElements"),
            std::make_pair(Selection::DATA, "dataElements")
        };

        /// @brief current selected parent
        Selection myCurrentSelectedParent;

        /// @brief current selected child
        Selection myCurrentSelectedChild;

        /// @brief Invalidated copy constructor.
        SelectionHierarchy(const SelectionHierarchy&) = delete;

        /// @brief Invalidated assignment operator.
        SelectionHierarchy& operator=(const SelectionHierarchy&) = delete;
    };

    // ===========================================================================
    // class Legend
    // ===========================================================================

    class Information : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        Information(GNESelectorFrame* selectorFrameParent);

        /// @brief destructor
        ~Information();
    };

    /**@brief Constructor
     * @brief viewParent GNEViewParent in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNESelectorFrame(GNEViewParent* viewParent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNESelectorFrame();

    /// @brief show Frame
    void show();

    /// @brief hide Frame
    void hide();

    /// @brief function called after undo/redo in the current frame
    void updateFrameAfterUndoRedo();

    /// @brief clear current selection with possibility of undo/redo
    void clearCurrentSelection() const;

    /**@brief select attribute carrier (element)
     * @param objectsUnderCursor objects under cursors
     */
    bool selectAttributeCarrier(const GNEViewNetHelper::ObjectsUnderCursor& objectsUnderCursor);

    /**@brief apply list of ids to the current selection according to Operation,
     * @note if setop==DEFAULT than the currently set mode (myOperation) is used
     */
    void handleIDs(const std::vector<GNEAttributeCarrier*>& ACs, const ModificationMode::Operation setop = ModificationMode::Operation::DEFAULT);

    /**@brief return ACs of the given type with matching attrs
     * @param[in] ACTag XML Tag of AttributeCarrier
     * @param[in] ACAttr XML Attribute of AttributeCarrier
     * @param[in] compOp One of {<,>,=} for matching against val or '@' for matching against expr
     */
    std::vector<GNEAttributeCarrier*> getMatches(const SumoXMLTag ACTag, const SumoXMLAttr ACAttr, const char compOp, const double val, const std::string& expr);

    /**@brief return GenericDatas of the given type with matching attrs
     * @param[in] genericDatas list of filter generic datas
     * @param[in] attr XML Attribute used to filter
     * @param[in] compOp One of {<,>,=} for matching against val or '@' for matching against expr
     */
    std::vector<GNEAttributeCarrier*> getGenericMatches(const std::vector<GNEGenericData*>& genericDatas, const std::string& attr, const char compOp, const double val, const std::string& expr);

    /// @brief get vertical frame that holds all widgets of frame
    FXVerticalFrame* getContentFrame() const;

    /// @brief get modification mode modul
    ModificationMode* getModificationModeModul() const;

    /// @brief get selection operation modul
    GNESelectorFrame::SelectionOperation* getSelectionOperationModul() const;

    /// @brief get modul for selection information
    SelectionInformation* getSelectionInformation() const;

private:
    /// @brief modul for selection information
    GNESelectorFrame::SelectionInformation* mySelectionInformation = nullptr;

    /// @brief modul for change modification mode
    GNESelectorFrame::ModificationMode* myModificationMode = nullptr;

    /// @brief moduls for select network element set
    GNEElementSet* myNetworkElementSet = nullptr;

    /// @brief moduls for select demand element set
    GNEElementSet* myDemandElementSet = nullptr;

    /// @brief moduls for select data element set
    GNEElementSet* myDataElementSet = nullptr;

    /// @brief modul for visual scaling
    GNESelectorFrame::VisualScaling* myVisualScaling = nullptr;

    /// @brief modul for selection operations
    GNESelectorFrame::SelectionOperation* mySelectionOperation = nullptr;

    /// @brief modul for selection hierarchy
    GNESelectorFrame::SelectionHierarchy* mySelectionHierarchy = nullptr;

    /// @brief information modul
    GNESelectorFrame::Information* myInformation = nullptr;

    /// @brief Invalidated copy constructor.
    GNESelectorFrame(const GNESelectorFrame&) = delete;

    /// @brief Invalidated assignment operator.
    GNESelectorFrame& operator=(const GNESelectorFrame&) = delete;
};

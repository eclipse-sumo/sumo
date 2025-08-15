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
/// @file    GNEElementDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2025
///
// A abstract dialog class for editing elements
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/GNEDialog.h>
#include <netedit/elements/GNEHierarchicalStructureChildren.h>
#include <netedit/GNENet.h>
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class definitions
// ===========================================================================

template <typename T>
class GNEElementDialog : protected GNEDialog {

public:
    /// @brief edit table
    template <typename U, typename V>
    class ElementList : protected FXVerticalFrame {

    public:
        /// @brief constructor
        ElementList(GNEElementDialog<T>* elementDialogParent, FXVerticalFrame* contentFrame, SumoXMLTag tag,
                    const GNEHierarchicalContainerChildren<U*>& elementChildren) :
            FXVerticalFrame(contentFrame, GUIDesignAuxiliarVerticalFrame) {
            // get tag property
            const auto* tagPropertiesDatabase = elementDialogParent->getElement()->getNet()->getViewNet()->getNet()->getTagPropertiesDatabase();
            const auto* tagProperty = tagPropertiesDatabase->getTagProperty(tag, true);
            // fill editable attributes
            for (const auto& attrProperty : tagProperty->getAttributeProperties()) {
                if (attrProperty->isEditMode() && attrProperty->isBasicEditor()) {
                    myAttrProperties.push_back(attrProperty);
                }
            }
            // horizontal frame for buttons
            FXHorizontalFrame* buttonFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
            // create label and button
            myAddButton = GUIDesigns::buildFXButton(buttonFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::ADD),
                                                    elementDialogParent, MID_GNE_ELEMENTLIST_ADD, GUIDesignButtonIcon);
            myLabel = new FXLabel(buttonFrame, TLF("Add new %", tagProperty->getTagStr()).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
            // create and configure table
            myTable = new FXTable(this, elementDialogParent, MID_GNE_ELEMENTLIST_EDIT, GUIDesignElementList);
            myTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
            myTable->setSelTextColor(FXRGBA(0, 0, 0, 255));
            // fill edited elements
            for (const auto& child : elementChildren) {
                if (child->getTagProperty()->getTag() == tag) {
                    myEditedElements.push_back(child);
                }
            }
            // reset list
            refreshList();
        }

        /// @brief get elements
        const std::vector<U*>& getEditedElements() const {
            return myEditedElements;
        }

        /// @brief check if object is one of the elements of this class
        bool checkObject(const FXObject* obj) const {
            if (obj == myAddButton) {
                return true;
            } else if (obj == myTable) {
                return true;
            } else {
                return false;
            }
        }

        /// @brief add element
        long addElement(T* element) {
            // insert in list
            myEditedElements.push_back(element);
            // add change command
            element->getNet()->getViewNet()->getUndoList()->add(new V(element, true), true);
            // reset list
            refreshList();
            return 1;
        }

        /// @brief called when user clicks over list
        long onCmdClickedList(FXObject* obj, FXSelector sel, void* ptr) {
            // first check if any delete button was pressed
            for (int i = 0; i < (int)myEditedElements.size(); i++) {
                // check if the remove button has focus
                if (myTable->getItem(i, (myAttrProperties.size() + 1))->hasFocus()) {
                    // add change command
                    myEditedElements.at(i)->getNet()->getViewNet()->getUndoList()->add(new V(myEditedElements.at(i), false), true);
                    // remove element from edited elements
                    myEditedElements.erase(myEditedElements.begin() + i);
                    // reset list
                    refreshList();
                    // abort
                    return 1;
                }
            }
            // handle in table
            return myTable->handle(obj, sel, ptr);
        }

        /// @brief update list
        long onCmdUpdateList(FXObject* obj, FXSelector sel, void* ptr) {
            // check validity of all elements
            for (int i = 0; i < myEditedElements.size(); i++) {
                for (int j = 0; j < myAttrProperties.size(); j++) {
                    // get cell value
                    const std::string value = myTable->getItem(i, j)->getText().text();
                    // set icon depending of validity
                    if (myEditedElements.at(i)->isValid(myAttrProperties.at(j)->getAttr(), value)) {
                        myTable->getItem(i, myAttrProperties.size())->setIcon(GUIIconSubSys::getIcon(GUIIcon::CORRECT));
                    } else {
                        myTable->getItem(i, myAttrProperties.size())->setIcon(GUIIconSubSys::getIcon(GUIIcon::INCORRECT));
                    }
                }
            }
            // recalculate table for see new icons
            myTable->recalc();
            // continue handling in table
            return myTable->handle(obj, sel, ptr);
        }

        /// @brief disable list
        void disableList(const std::string& reason) {
            myTable->disable();
            myAddButton->disable();
            myLabel->disable();
            myLabel->setText(reason.c_str());
        }

        /// @brief check if the current table is valid
        bool isValid() const {
            // simply check if we have the incon "valid" in all rows
            for (int i = 0; i < getNumRows(); i++) {
                if (myTable->getItem(i, myAttrProperties.size())->getIcon() != GUIIconSubSys::getIcon(GUIIcon::CORRECT)) {
                    return false;
                }
            }
            // all ok, then return true
            return true;
        }

        /// @brief get num row
        FXint getNumRows() const {
            return myTable->getNumRows();
        }

        /// @brief get table item
        FXTableItem* getItem(const int row, const int col) const {
            return myTable->getItem(row, col);
        }

        /// @brief refresh list
        void refreshList() {
            // get number of columns and rows
            const int numRows = (int)myEditedElements.size();
            const int numCols = (int)myAttrProperties.size() + 2;
            // calculate attribute width
            std::vector<int>attributesWidth;
            attributesWidth.resize(3);
            // if neccesary, extend with more sizes
            if (myAttrProperties.size() == 2) {
                attributesWidth[0] = 144;
                attributesWidth[1] = 145;
            } else if (myAttrProperties.size() == 3) {
                attributesWidth[0] = 96;
                attributesWidth[1] = 96;
                attributesWidth[2] = 97;
            } else {
                throw ProcessError("More attributesWidth needed");
            }
            // clear table
            myTable->clearItems();
            // set number of rows
            myTable->setTableSize(numRows, numCols);
            // Configure columns
            myTable->setVisibleColumns(numCols);
            // set column height
            myTable->setColumnHeaderHeight(GUIDesignHeight);
            for (int i = 0; i < numCols; i++) {
                // set column width and text
                if (i < (numCols - 2)) {
                    myTable->setColumnWidth(i, attributesWidth.at(i));
                    myTable->setColumnText(i, myAttrProperties.at(i)->getAttrStr().c_str());
                    myTable->setColumnJustify(i, FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
                } else {
                    myTable->setColumnWidth(i, GUIDesignHeight);
                    myTable->setColumnText(i, "");
                }
            }
            // hide row header
            myTable->getRowHeader()->setWidth(0);
            // now configure rows
            for (int i = 0; i < numRows; i++) {
                FXTableItem* item = new FXTableItem("");
                // add attributes
                for (int j = 0; j < numCols - 2; j++) {
                    // create item using attribute
                    item = new FXTableItem(myEditedElements.at(i)->getAttribute(myAttrProperties.at(j)->getAttr()).c_str());
                    // set item to table
                    myTable->setItem(i, j, item);
                }
                // set valid icon
                item = new FXTableItem("", GUIIconSubSys::getIcon(GUIIcon::CORRECT));
                item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
                item->setEnabled(false);
                myTable->setItem(i, numCols - 2, item);
                // set remove
                item = new FXTableItem("", GUIIconSubSys::getIcon(GUIIcon::REMOVE));
                item->setJustify(FXTableItem::CENTER_X | FXTableItem::CENTER_Y);
                item->setEnabled(false);
                myTable->setItem(i, numCols - 1, item);
            }
        }

    protected:
        /// @brief list of edited attrs
        std::vector<const GNEAttributeProperties*> myAttrProperties;

        /// @brief edited elements
        std::vector<U*> myEditedElements;

        /// @brief add button
        FXButton* myAddButton = nullptr;

        /// @brief label
        FXLabel* myLabel = nullptr;

        /// @brief table
        FXTable* myTable = nullptr;

    private:
        /// @brief Invalidated copy constructor
        ElementList(const ElementList&) = delete;

        /// @brief Invalidated assignment operator
        ElementList& operator=(const ElementList&) = delete;
    };

    /// @brief constructor (temporal)
    GNEElementDialog(T* element, const bool updatingElement, const int width, const int height) :
        GNEDialog(element->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                  TLF("Edit '%' data", element->getID()), element->getTagProperty()->getGUIIcon(),
                  Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, ResizeMode::STATIC, width, height),
        myElement(element),
        myUpdatingElement(updatingElement),
        myChangesDescription(TLF("change % values", element->getTagStr())),
        myNumberOfChanges(0) {
        // change dialog title depending if we are updating or creating an element
        if (updatingElement) {
            setTitle(TLF("Create %", element->getTagStr()).c_str());
        } else {
            setTitle(TLF("edit % '%'", element->getTagStr(), element->getID()).c_str());
        }
        // init commandGroup
        myElement->getNet()->getViewNet()->getUndoList()->begin(myElement, myChangesDescription);
        // save number of command group changes
        myNumberOfChanges = myElement->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize();
    }

    /// @brief constructor
    GNEElementDialog(T* element, const bool updatingElement) :
        GNEDialog(element->getNet()->getViewNet()->getViewParent()->getGNEAppWindows(),
                  TLF("Edit '%' data", element->getID()), element->getTagProperty()->getGUIIcon(),
                  Buttons::ACCEPT_CANCEL_RESET, OpenType::MODAL, ResizeMode::STATIC),
        myElement(element),
        myUpdatingElement(updatingElement),
        myChangesDescription(TLF("change % values", element->getTagStr())),
        myNumberOfChanges(0) {
        // change dialog title depending if we are updating or creating an element
        if (updatingElement) {
            setTitle(TLF("Create %", element->getTagStr()).c_str());
        } else {
            setTitle(TLF("edit % '%'", element->getTagStr(), element->getID()).c_str());
        }
        // init commandGroup
        myElement->getNet()->getViewNet()->getUndoList()->begin(myElement, myChangesDescription);
        // save number of command group changes
        myNumberOfChanges = myElement->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize();
    }

    /// @brief destructor
    ~GNEElementDialog() {}

    /// @brief get edited element
    T* getElement() const {
        return myElement;
    }

    /// @brief run internal test
    virtual void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) = 0;

    /// @name FOX-callbacks
    /// @{
    /// @brief event after press accept button
    virtual long onCmdAccept(FXObject* sender, FXSelector sel, void* ptr) = 0;

    /// @brief event after press cancel button
    virtual long onCmdCancel(FXObject* sender, FXSelector sel, void* ptr) = 0;

    /// @brief event after press cancel button
    virtual long onCmdReset(FXObject*, FXSelector, void*) = 0;

    /// @}

protected:
    /// @brief FOX needs this
    FOX_CONSTRUCTOR(GNEElementDialog)

    /// @brief pointer to edited element
    T* myElement = nullptr;

    /// @brief flag to indicate if element are being created or modified (cannot be changed after open dialog)
    bool myUpdatingElement = false;

    /// @brief Accept changes did in this dialog.
    void acceptChanges() {
        // commit changes or abort last command group depending of number of changes did
        if (myNumberOfChanges < myElement->getNet()->getViewNet()->getUndoList()->currentCommandGroupSize()) {
            myElement->getNet()->getViewNet()->getUndoList()->end();
        } else {
            myElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
        }
    }

    /// @brief Cancel changes did in this dialog.
    void cancelChanges() {
        myElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
    }

    /// @brief reset changes did in this dialog.
    void resetChanges() {
        // abort last command group an start editing again
        myElement->getNet()->getViewNet()->getUndoList()->abortLastChangeGroup();
        myElement->getNet()->getViewNet()->getUndoList()->begin(myElement, myChangesDescription);
    }

private:
    /// @brief description of changes did in this element dialog
    std::string myChangesDescription;

    /// @brief number of GNEChanges_... in dialog
    int myNumberOfChanges;

    /// @brief Invalidated copy constructor
    GNEElementDialog(const GNEElementDialog&) = delete;

    /// @brief Invalidated assignment operator
    GNEElementDialog& operator=(const GNEElementDialog&) = delete;
};

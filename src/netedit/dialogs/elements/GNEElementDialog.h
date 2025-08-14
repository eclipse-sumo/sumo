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
#include <netedit/GNENet.h>
#include <netedit/GNETagProperties.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewParent.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class definitions
// ===========================================================================

template <typename T>
class GNEElementDialog : public GNEDialog {

public:
    /// @brief edit table
    template <typename U>
    class EditTable : public FXVerticalFrame {

    public:
        /// @brief constructor
        EditTable(GNEElementDialog<T>* elementDialogParent, FXVerticalFrame* contentFrame, SumoXMLTag elementTag,
                  FXSelector addSelector, FXSelector tableSelector, const std::vector<SumoXMLAttr> attrs) :
            FXVerticalFrame(contentFrame, GUIDesignAuxiliarVerticalFrame),
            myAttrs(attrs) {
            // horizontal frame for buttons
            FXHorizontalFrame* buttonFrame = new FXHorizontalFrame(this, GUIDesignAuxiliarHorizontalFrame);
            // create label and button
            myAddButton = GUIDesigns::buildFXButton(buttonFrame, "", "", "", GUIIconSubSys::getIcon(GUIIcon::ADD), elementDialogParent, addSelector, GUIDesignButtonIcon);
            myLabel = new FXLabel(buttonFrame, TLF("Add new %", toString(elementTag)).c_str(), nullptr, GUIDesignLabelThick(JUSTIFY_NORMAL));
            // create and configure table
            myTable = new FXTable(this, elementDialogParent, tableSelector, GUIDesignTableAdditionals);
            myTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
            myTable->setSelTextColor(FXRGBA(0, 0, 0, 255));
        }

        /// @brief disable table
        void disableTable(const std::string reason) {
            myTable->disable();
            myAddButton->disable();
            myLabel->disable();
            myLabel->setText(reason.c_str());
        }

        /// @brief get num row
        FXint getNumRows() const {
            return myTable->getNumRows();
        }

        /// @brief get table item
        FXTableItem* getItem(const int row, const int col) const {
            return myTable->getItem(row, col);
        }

        /// @brief remove row
        void removeRow(const int row) {
            myTable->removeRows(row);
        }

        /// @brief set number of columns
        void configureTable(const std::vector<U*>& elements) {
            // get number of columns and rows
            const int numRows = (int)elements.size();
            const int numCols = (int)myAttrs.size() + 2;
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
                    myTable->setColumnWidth(i, 100);
                    myTable->setColumnText(i, toString(myAttrs.at(i)).c_str());
                } else {
                    myTable->setColumnWidth(i, GUIDesignHeight);
                    myTable->setColumnText(i, "");
                }
            }
            // hide row header
            myTable->getRowHeader()->setWidth(0);
            // now configure rows
            for (int i = 0; i < numRows; i++) {
                FXTableItem* item = nullptr;
                // add attributes
                for (int j = 0; j < numCols - 2; j++) {
                    // create item using attribute
                    item = new FXTableItem(elements.at(i)->getAttribute(myAttrs.at(j)).c_str());
                    // set item to table
                    myTable->setItem(i, j, item);
                }
                // set valid icon
                item = new FXTableItem("");
                item->setIcon(GUIIconSubSys::getIcon(GUIIcon::CORRECT));
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
        /// @brief add button
        FXButton* myAddButton = nullptr;

        /// @brief label
        FXLabel* myLabel = nullptr;

        /// @brief table
        FXTable* myTable = nullptr;

        /// @brief list of edited attrs
        const std::vector<SumoXMLAttr> myAttrs;

    private:
        /// @brief Invalidated copy constructor
        EditTable(const EditTable&) = delete;

        /// @brief Invalidated assignment operator
        EditTable& operator=(const EditTable&) = delete;
    };

    /// @brief constructor
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

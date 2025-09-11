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
/// @file    GNEFixElementsDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    Jul 2025
///
// template used to fix elements during saving
/****************************************************************************/
#pragma once
#include <config.h>

#include <netedit/dialogs/basic/GNEErrorBasicDialog.h>
#include <netedit/dialogs/basic/GNEInformationBasicDialog.h>
#include <utils/foxtools/MFXGroupBoxModule.h>
#include <utils/gui/div/GUIDesigns.h>
#include <utils/gui/windows/GUIAppEnum.h>

// ===========================================================================
// class definitions
// ===========================================================================

template <typename T>
class GNEFixElementsDialog : public GNEDialog {

public:
    /// @brief Conflict
    class ConflictElement {

    public:
        /// @brief constructor
        ConflictElement(T element, const std::string& id, FXIcon* icon, const std::string& description) :
            myElement(element),
            myID(id),
            myIcon(icon),
            myDescription(description) {}

        /// @brief destructor
        ~ConflictElement() {}

        /// @brief conflicted element
        T getElement() const {
            return myElement;
        }

        /// @brief get element ID
        const std::string& getID() const {
            return myID;
        }

        /// @brief get element icon
        FXIcon* getIcon() const {
            return myIcon;
        }

        /// @brief get conflict description
        const std::string& getDescription() const {
            return myDescription;
        }

    private:
        /// @brief conflicted element
        T myElement;

        /// @brief ID of the element
        std::string myID;

        /// @brief icon
        FXIcon* myIcon = nullptr;

        /// @brief conflict description
        std::string myDescription;

        /// @brief invalidate default constructor
        ConflictElement() = delete;
    };

    /// @brief GNEFixOptions module
    class FixOptions : public MFXGroupBoxModule {

    public:
        /// @brief constructor
        FixOptions(GNEFixElementsDialog<T>* fixElementDialog, FXVerticalFrame* frameParent, const std::string& title) :
            MFXGroupBoxModule(frameParent, title, MFXGroupBoxModule::Options::SAVE),
            myFixElementDialogParent(fixElementDialog) {
            // register this fix option to list of fix options
            fixElementDialog->registerFixOptions(this);
            // Create table
            myTable = new FXTable(getCollapsableFrame(), this, MID_TABLE, GUIDesignTableFixElements);
            // create frames for options
            FXHorizontalFrame* optionsFrame = new FXHorizontalFrame(getCollapsableFrame(), GUIDesignAuxiliarHorizontalFrame);
            myLeftFrameOptions = new FXVerticalFrame(optionsFrame, GUIDesignAuxiliarFrameFixedWidth(int(myTable->getWidth() * 0.5)));
            myRightFrameOptions = new FXVerticalFrame(optionsFrame, GUIDesignAuxiliarFrameFixedWidth(int(myTable->getWidth() * 0.5)));
        }

        /// @brief select internal test solution (used in internal tests)
        virtual void selectInternalTestSolution(const std::string& solution) = 0;

        /// @brief apply selected fix option
        virtual bool applyFixOption() = 0;

        /// @name FOX-callbacks
        /// @{

        /// @brief called when user select a option
        virtual long onCmdSelectOption(FXObject*, FXSelector, void*) = 0;

        /// @}

        /// @brief set invalid elements to fix
        void setInvalidElements(const std::vector<ConflictElement>& conflictedElements) {
            // parse invalid elements
            myConflictedElements = conflictedElements;
            // configure table
            myTable->setTableSize((int)(myConflictedElements.size()), 3);
            myTable->setSelBackColor(FXRGBA(255, 255, 255, 255));
            myTable->setSelTextColor(FXRGBA(0, 0, 0, 255));
            myTable->setEditable(false);
            // configure header
            myTable->setVisibleColumns(4);
            myTable->setColumnHeaderHeight(GUIDesignHeight);
            myTable->getRowHeader()->setWidth(0);
            // icon
            myTable->setColumnWidth(0, GUIDesignHeight);
            myTable->setColumnText(0, "");
            // ID
            myTable->setColumnWidth(1, 110);
            myTable->setColumnText(1, toString(SUMO_ATTR_ID).c_str());
            // description
            myTable->setColumnWidth(2, myTable->getWidth() - GUIDesignHeight - 110 - 15);
            myTable->setColumnText(2, TL("Conflict"));
            // Declare pointer to FXTableItem
            FXTableItem* item = nullptr;
            // iterate over invalid edges
            for (int i = 0; i < (int)myConflictedElements.size(); i++) {
                // Set icon
                item = new FXTableItem("", myConflictedElements.at(i).getIcon());
                item->setIconPosition(FXTableItem::CENTER_X);
                myTable->setItem(i, 0, item);
                // Set ID
                item = new FXTableItem(myConflictedElements.at(i).getID().c_str());
                item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
                myTable->setItem(i, 1, item);
                // Set conflict
                item = new FXTableItem(myConflictedElements.at(i).getDescription().c_str());
                item->setJustify(FXTableItem::LEFT | FXTableItem::CENTER_Y);
                myTable->setItem(i, 2, item);
                // set row height
                myTable->setRowHeight(i, GUIDesignHeight);
            }
            // check if enable or disable options
            if (myConflictedElements.size() > 0) {
                enableOptions();
            } else {
                disableOptions();
            }
        }

    protected:
        /// @brief pointer to the parent dialog
        GNEFixElementsDialog* myFixElementDialogParent = nullptr;

        /// @brief Table with the demand elements
        FXTable* myTable = nullptr;

        /// @brief vertical left frame for options
        FXVerticalFrame* myLeftFrameOptions = nullptr;

        /// @brief vertical right frame for options
        FXVerticalFrame* myRightFrameOptions = nullptr;

        /// @brief list of elements to fix
        std::vector<ConflictElement> myConflictedElements;

        /// @brief default constructor
        FixOptions() :
            MFXGroupBoxModule() {
        }

        /// @brief add option to options container (used for adjust width and enable/disable)
        void registerOption(FXWindow* option) {
            myOptions.push_back(option);
            // adjust width
            option->setWidth(int(myTable->getWidth() * 0.5));
        }

    private:
        /// @brief list of options
        std::vector<FXWindow*> myOptions;

        /// @brief enable options
        void enableOptions() {
            // enable each option
            for (auto option : myOptions) {
                option->enable();
            }
        }

        /// @brief disable options
        void disableOptions() {
            // disable each option
            for (auto option : myOptions) {
                option->disable();
            }
        }

        /// @brief save save list of conflicted items to a file (Reimplemented from MFXGroupBoxModule)
        bool saveContents() const {
            // open file dialog to save list of conflicted items
            const FXString file = MFXUtils::getFilename2Write(myTable,
                                  TL("Save list of conflicted items"),
                                  SUMOXMLDefinitions::TXTFileExtensions.getMultilineString().c_str(),
                                  GUIIconSubSys::getIcon(GUIIcon::SAVE), gCurrentFolder);
            // continue if file is not empty
            if (file == "") {
                return false;
            } else {
                try {
                    // open output device
                    OutputDevice& device = OutputDevice::getDevice(file.text());
                    // get invalid element ID and problem
                    for (const auto& conflictedElement : myConflictedElements) {
                        device << conflictedElement.getID() << ":" << conflictedElement.getDescription() << "\n";
                    }
                    // close output device
                    device.close();
                    // open information message box
                    GNEInformationBasicDialog(myFixElementDialogParent->myApplicationWindow,
                                              TL("Saving successfully"),
                                              TL("List of conflicted items was successfully saved"));
                    return true;
                } catch (IOError& e) {
                    // open message box error
                    GNEErrorBasicDialog(myFixElementDialogParent->myApplicationWindow,
                                        TL("Saving list of conflicted items failed"), e.what());
                    return false;
                }
            }
        }

        /// @brief Invalidated copy constructor.
        FixOptions(const FixOptions&) = delete;

        /// @brief Invalidated assignment operator.
        FixOptions& operator=(const FixOptions&) = delete;
    };

    /// @brief Constructor
    GNEFixElementsDialog(GNEApplicationWindow* mainWindow, const std::string title,
                         GUIIcon icon, DialogType type):
        GNEDialog(mainWindow, title.c_str(), icon, type, GNEDialog::Buttons::ACCEPT_CANCEL,
                  GNEDialog::OpenType::MODAL, ResizeMode::STATIC) {
        // create left and right frames
        FXHorizontalFrame* columnFrame = new FXHorizontalFrame(myContentFrame, GUIDesignAuxiliarHorizontalFrame);
        myLeftFrame = new FXVerticalFrame(columnFrame, GUIDesignAuxiliarVerticalFrame);
        myRightFrame = new FXVerticalFrame(columnFrame, GUIDesignAuxiliarVerticalFrame);
    }

    /// @brief destructor
    ~GNEFixElementsDialog() {}

    /// @brief pointer to the main window
    GNEApplicationWindow* getApplicationWindow() {
        return myApplicationWindow;
    }

    /// @brief register fix options to the dialog (called automatically during FixOptions constructor)
    void registerFixOptions(FixOptions* fixOptions) {
        myFixOptions.push_back(fixOptions);
    }

    /// @brief run internal test
    void runInternalTest(const InternalTestStep::DialogArgument* dialogArgument) {
        // run internal test for each fix option
        for (auto fixOption : myFixOptions) {
            fixOption->selectInternalTestSolution(dialogArgument->getCustomAction());
        }
    }

    /// @name FOX-callbacks
    /// @{

    /// @brief event after press accept button
    long onCmdAccept(FXObject*, FXSelector, void*) {
        bool abortSaving = false;
        // apply each fix option in their correspond fixOption
        for (auto fixOption : myFixOptions) {
            // if applyFixOption returns false, abort saving (usually for selecting invalid elements)
            if (fixOption->applyFixOption() == false) {
                abortSaving = true;
            }
        }
        // continue depending of abortSaving
        if (abortSaving == false) {
            return closeDialogAccepting();
        } else {
            return closeDialogCanceling();
        }
    }

    /// @}

protected:
    /// @brief vector with all fix options
    std::vector<GNEFixElementsDialog::FixOptions*> myFixOptions;

    /// @brief left frame for fix options
    FXVerticalFrame* myLeftFrame = nullptr;

    /// @brief right frame for fix options
    FXVerticalFrame* myRightFrame = nullptr;

private:
    /// @brief Invalidated copy constructor.
    GNEFixElementsDialog(const GNEFixElementsDialog&) = delete;

    /// @brief Invalidated assignment operator.
    GNEFixElementsDialog& operator=(const GNEFixElementsDialog&) = delete;
};

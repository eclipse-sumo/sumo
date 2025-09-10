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
/// @file    InternalTestStep.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Single operation used in InternalTests
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/foxtools/fxheader.h>

#include "InternalTest.h"

// ===========================================================================
// class definitions
// ===========================================================================

class InternalTestStep {

public:
    /// @name category step
    enum class Category {
        META,           // Meta step (used for packing set of steps like click or write)
        INIT,           // Setup and start step
        FINISH,         // Write last line
        APP,            // send signal to APP (Either GUIAppWindows or GNEApplicationWindow)
        VIEW,           // send signal to view (either GUIView or GNEViewNet)
        TLS_PHASES,     // send signal to TLS Phases module (used for TLS Phases)
        TLS_PHASETABLE, // send signal to TLSTable (used for TLS Phases)
        DIALOG,         // send signal to dialog (used for modal dialogs)
        COLOR,          // send signal to color dialog (temporal)
    };

    /// @brief dialog arguments, used for certain modal dialogs that can not be edited using tab
    class DialogArgument {

    public:
        /// @name basic actions
        enum class BasicAction {
            ACCEPT,     // press accept button
            CANCEL,     // press cancel button
            RESET,      // press reset button
            ABORT,      // abort dialog
            NONE,       // no action
        };

        /// @name basic actions
        enum class ExtendedAction {
            CUSTOM,     // custom extended action
            NONE        // no extended action
        };

        /// @brief constructor for basic actions
        DialogArgument(BasicAction basicAction);

        /// @brief constructor for basic actions
        DialogArgument(ExtendedAction extendedAction, const std::string& customAction);

        /// @brief constructor for custom actions
        DialogArgument(const std::string& customAction);

        /// @brief constructor for custom actions and prefix to remove in the action
        DialogArgument(const std::string& prefixToRemove, const std::string& customAction);

        /// @brief get basic action
        BasicAction getBasicAction() const;

        /// @brief get extended action
        ExtendedAction getExtendedAction() const;

        /// @brief get custom action
        const std::string& getCustomAction() const;

    protected:
        /// @brief basic action
        BasicAction myBasicAction = BasicAction::NONE;

        /// @brief extended action
        ExtendedAction myExtendedAction = ExtendedAction::NONE;

        /// @brief action to be carried out in the dialog
        std::string myCustomAction;

    private:
        /// @brief invalidated default constructor
        DialogArgument() = delete;

        /// @brief invalidated copy constructor
        DialogArgument(const DialogArgument&) = delete;
    };

    /// @brief struct used for test TLS Tables
    class TLSTableTest {

    public:
        /// brief default constructor for phases
        TLSTableTest(FXSelector sel_, const int row_);

        /// brief default constructor with text
        TLSTableTest(FXSelector sel_, const int row_, const int column_, const std::string& text_);

        /// @brief selector
        const FXSelector sel = 0;

        /// @brief x coordinate
        const int row = 0;

        /// @brief y coordinate
        const int column = 0;

        /// @brief text
        const std::string text = "";

    private:
        /// @brief invalidated default constructor
        TLSTableTest() = delete;

        /// @brief invalidated copy constructor
        TLSTableTest(const TLSTableTest&) = delete;
    };

    /// @brief constructor for parsing step in string format
    InternalTestStep(InternalTest* testSystem, const std::string& step);

    /// @brief constructor for shortcuts
    InternalTestStep(InternalTest* testSystem, FXSelector messageType, FXSelector messageID,
                     Category category, const std::string& description);

    /// @brief constructor for input events (click, keyPress, etc.)
    InternalTestStep(InternalTest* testSystem, FXSelector messageType, Category category,
                     FXEvent* event, const bool updateView, const std::string& description);

    /// @brief constructor for dialog arguments
    InternalTestStep(InternalTest* testSystem, DialogArgument* dialogArgument,
                     const std::string& description);

    /// @brief destructor
    ~InternalTestStep();

    /// @name next step management
    /// @{

    /// @brief get next step
    InternalTestStep* getNextStep() const;

    /// @brief set next step
    void setNextStep(InternalTestStep* nextStep);

    /// @}

    /// @brief get message type
    FXSelector getMessageType() const;

    /// @brief get message ID
    FXSelector getMessageID() const;

    /// @brief get dialog argument
    DialogArgument* getDialogArgument() const;

    /// @brief get TLS Table test
    TLSTableTest* getTLSTableTest() const;

    /// @brief get selector (based in messageType and messageID)
    FXSelector getSelector() const;

    /// @brief check if update view
    bool updateView() const;

    /// @brief get category
    Category getCategory() const;

    /// @brief get event associated with this step
    void* getEvent() const;

    ///  @brief get description
    const std::string& getDescription() const;

private:
    /// @brief test system parent
    InternalTest* myTestSystem = nullptr;

    /// @brief next step in the test
    InternalTestStep* myNextStep = nullptr;

    /// @brief message type (by default SEL_COMMAND)
    FXSelector myMessageType = SEL_COMMAND;

    /// @brief message ID
    FXSelector myMessageID = 0;

    /// @brief step category
    Category myCategory = Category::META;

    /// @brief flag to enable or disable view after execute step
    bool myUpdateView = false;

    /// @brief description
    std::string myDescription;

    /// @brief arguments
    std::vector<std::string> myArguments;

    /// @brief list of events associated with this step
    FXEvent* myEvent = nullptr;

    /// @brief dialog argument
    DialogArgument* myDialogArgument = nullptr;

    /// @brief TLS Table test
    TLSTableTest* myTLSTableTest = nullptr;

    /// @brief parse function and arguments
    std::string parseStep(const std::string& rowText);

    /// @brief parse arguments
    void parseArguments(const std::string& arguments);

    /// @brief process setupAndStart function
    void setupAndStart();

    /// @brief finish function
    void finish();

    /// @brief process click function
    void mouseClick(const std::string& button, const std::string& modifier) const;

    /// @brief process click function
    void leftClickOffset(const std::string& button) const;

    /// @brief process moveElementHorizontal function
    void moveElementHorizontal() const;

    /// @brief process moveElementVertical function
    void moveElementVertical() const;

    /// @brief process moveElement function
    void moveElement() const;

    /// @brief process typeKey function
    void typeKey() const;

    /// @brief process contextualMenuOperation function
    void contextualMenuOperation() const;

    /// @brief process protect elements function
    void protectElements() const;

    /// @brief process waitDeleteWarning function
    void waitDeleteWarning() const;

    /// @brief process modifyAttribute function
    void modifyAttribute(const int overlappedTabs) const;

    /// @brief process modifyBoolAttribute function
    void modifyBoolAttribute(const int overlappedTabs) const;

    /// @brief process modifyColorAttribute function
    void modifyColorAttribute(const int overlappedTabs) const;

    /// @brief process modifyVClassDialog_DisallowAll function
    void modifyVClassDialog_NoDisallowAll(const int overlappedTabs) const;

    /// @brief process modifyVClassDialog_DisallowAll function
    void modifyVClassDialog_DisallowAll(const int overlappedTabs) const;

    /// @brief process modifyVClassDialog_Cancel function
    void modifyVClassDialog_Cancel(const int overlappedTabs) const;

    /// @brief process modifyVClassDialog_Reset function
    void modifyVClassDialog_Reset(const int overlappedTabs) const;

    /// @brief process modifyVTypeDialogAttribute function
    void modifyVTypeDialogAttribute() const;

    /// @brief process createConnection function
    void createConnection(const std::string& keyModifier) const;

    /// @brief process createCrossing function
    void createCrossing() const;

    /// @brief process modifyCrossingDefaultValue function
    void modifyCrossingDefaultValue() const;

    /// @brief process modifyCrossingDefaultBoolValue function
    void modifyCrossingDefaultBoolValue() const;

    /// @brief process crossingClearEdges function
    void crossingClearEdges() const;

    /// @brief process crossingInvertEdges function
    void crossingInvertEdges() const;

    /// @brief process createConnectionEdit function
    void saveConnectionEdit() const;

    /// @brief process fixCrossings function
    void fixCrossings();

    /// @brief process fixStoppingPlace function
    void fixStoppingPlace();

    /// @brief process fixRoutes function
    void fixRoute();

    /// @brief process createTLS function
    void createTLS(const int overlappedTabs) const;

    /// @brief process Copy TLS function
    void copyTLS() const;

    /// @brief process join TLS function
    void joinTSL() const;

    /// @brief process disJoin TLS function
    void disJoinTLS() const;

    /// @brief process delete TLS function
    void deleteTLS() const;

    /// @brief process modifyTLSTable function
    void modifyTLSTable();

    /// @brief process resetSingleTLSPhases function
    void resetSingleTLSPhases() const;

    /// @brief process resetAllTLSPhases function
    void resetAllTLSPhases() const;

    /// @brief process pressTLSPhaseButton function
    void pressTLSPhaseButton() const;

    /// @brief process addPhase function
    void addPhase(const std::string& type);

    /// @brief process pressTLSButton function
    void pressTLSButton(const std::string& type);

    /// @brief process checkParameters function
    void checkParameters(const int overlappedTabs) const;

    /// @brief process checkDoubleParameters function
    void checkDoubleParameters(const int overlappedTabs) const;

    /// @brief process changeEditMode function
    void changeEditMode();

    /// @brief process save existent function
    void saveExistentFile();

    /// @brief process check undo-redo function
    void checkUndoRedo() const;

    /// @brief process delete function
    void deleteFunction() const;

    /// @brief process selection function
    void selection() const;

    /// @brief process selectNetworkItems function
    void selectNetworkItems() const;

    /// @brief process lockSelection function
    void lockSelection() const;

    /// @brief process selectionRectangle function
    void selectionRectangle() const;

    /// @brief process createDataSet function
    void createDataSet() const;

    /// @brief process createDataInterval function
    void createDataInterval() const;

    /// @brief process openAboutDialog function
    void openAboutDialog();

    /// @brief process load file function
    void loadFile();

    /// @brief process save new file function
    void saveNewFile();

    /// @brief process save file as function
    void saveFileAs();

    /// @brief process reload file function
    void reloadFile();

    /// @brief process overwriting accept function
    void overwritingAccept();

    /// @brief process overwriting cancel function
    void overwritingCancel();

    /// @brief process overwriting abort function
    void overwritingAbort();

    /// @brief process overwriting apply to all function
    void overwritingApplyToAll();

    /// @brief process undo function
    void undo() const;

    /// @brief process redo function
    void redo() const;

    /// @brief process changeSupermode function
    void changeSupermode();

    /// @brief process changeMode function
    void changeMode();

    /// @brief process changeElement function
    void changeElement() const;

    /// @bief process changePlan function
    void changePlan() const;

    /// @brief process computeJunctions function
    void computeJunctions();

    /// @brief process computeJunctionsVolatileOptions function
    void computeJunctionsVolatileOptions();

    /// @brief process selectChild function
    void selectAdditionalChild();

    /// @brief process createRectangledShape function
    void createRectangledShape();

    /// @brief process createSquaredShape function
    void createSquaredShape();

    /// @brief process createLineShape function
    void createLineShape();

    /// @brief process createMeanData function
    void createMeanData();

    /// @brief process deleteMeanData function
    void deleteMeanData();

    /// @brief process copyMeanData function
    void copyMeanData();

    /// @brief process quit function
    void quit();

    /// @brief check int argument
    bool checkIntArgument(const std::string& argument) const;

    /// @brief get int argument
    int getIntArgument(const std::string& argument) const;

    /// @brief check bool argument
    bool checkBoolArgument(const std::string& argument) const;

    /// @brief get bool argument
    bool getBoolArgument(const std::string& argument) const;

    /// @brief check string argument
    bool checkStringArgument(const std::string& argument) const;

    /// @brief get string argument
    std::string getStringArgument(const std::string& argument) const;

    /// @brief strip spaces
    std::string stripSpaces(const std::string& str) const;

    /// @brief write error
    void writeError(const std::string& function, const int overlapping,
                    const std::string& expected) const;

    /// @brief create shape
    void createShape(const InternalTest::ViewPosition& viewPosition,
                     const int sizeX, const int sizeY, const bool close,
                     const bool line) const;

    /// @name modify attribute functions
    /// @{

    /// @brief modify attribute
    void modifyStringAttribute(Category category, const int tabs, const int overlappedTabs, const std::string& value) const;

    /// @brief modify bool attribute
    void modifyBoolAttribute(Category category, const int tabs, const int overlappedTabs) const;

    /// @}

    /// @name undo-redo functions
    /// @{

    /// @brief process check undo function
    void buildUndo(const int number) const;

    /// @brief process check redo function
    void buildRedo(const int number) const;

    /// @}

    /// @name key functions
    /// @{

    /// @brief translate key
    std::pair<FXint, FXString> translateKey(const std::string& key) const;

    /// @brief build key press event
    FXEvent* buildKeyPressEvent(const std::string& key) const;

    /// @brief build key release event
    FXEvent* buildKeyReleaseEvent(const std::string& key) const;

    /// @brief build a key press and key release (used for tabs, spaces, enter, etc)
    void buildPressKeyEvent(Category category, const std::string& key, const bool updateView) const;

    /// @brief build a two key press and key release (used for tabs, spaces, enter, etc)
    void buildTwoPressKeyEvent(Category category, const std::string& keyA, const std::string& keyB, const bool updateView) const;

    /// @}

    /// @name mouse functions
    /// @{

    /// @brief build mouse click event
    void buildMouseClick(const InternalTest::ViewPosition& viewPosition,
                         const int offsetX, const int offsetY,
                         const std::string& button, const std::string& keyModifier) const;

    /// @brief build mouse dragdrop
    void buildMouseDragDrop(const InternalTest::ViewPosition& viewStartPosition,
                            const int offsetStartX, const int offsetStartY,
                            const InternalTest::ViewPosition& viewEndPosition,
                            const int offsetEndX, const int offsetEndY,
                            const std::string& keyModifier) const;

    /// @brief build mouse move event
    FXEvent* buildMouseMoveEvent(const InternalTest::ViewPosition& viewPosition,
                                 const int offsetX, const int offsetY, const int clickedButton,
                                 const std::string& keyModifier, const int numberOfClicks) const;

    /// @brief build mouse left click press event
    FXEvent* buildMouseClickEvent(FXSelType type, const InternalTest::ViewPosition& viewPosition,
                                  const int offsetX, const int offsetY, const std::string& keyModifier,
                                  const int numberOfClicks) const;

    /// @brief write click info
    void writeClickInfo(const InternalTest::ViewPosition& viewPosition,
                        const int offsetX, const int offsetY,
                        const std::string modifier) const;

    /// @}

    /// @brief invalidate default constructor
    InternalTestStep() = delete;

    /// @brief Invalidated copy constructor.
    InternalTestStep(const InternalTestStep&) = delete;

    /// @brief Invalidated assignment operator
    InternalTestStep& operator=(const InternalTestStep&) = delete;
};

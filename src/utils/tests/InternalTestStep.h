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
        APP,            // send signal to APP (Either GUIAppWindows or GNEApplicationWindow)
        VIEW,           // send signal to view (either GUIView or GNEViewNet)
        TLS_PHASES,     // send signal to TLS Phases module (used for TLS Phases)
        TLS_PHASETABLE, // send signal to TLSTable (used for TLS Phases)
        COLOR,          // send signal to color dialog
    };

    /// @brief dialog arguments (used for certain functions that opens modal dialogs)
    class DialogTest {

    public:
        /// @brief constructor for yes/no argument
        DialogTest(const FXuint value);

        /// @brief constructor for question dialogs
        DialogTest(const std::vector<FXuint>& values);

        /// @brief constructor fix dialogs
        DialogTest(const std::string& solution);

        /// @brief yes value
        static const FXuint yes = 1;

        /// @brief no value
        static const FXuint no = 2;

        /// @brief ESC or cancel value
        static const FXuint esc = 4;

        /// @brief color dialog value
        static const std::string colorValue;

        /// @brief used if we have multiple modal dialogs
        const std::vector<FXuint> questionDialogValues;

        /// @brief solution for fix dialogs
        const std::string fixSolution;

    private:
        /// @brief invalidated default constructor
        DialogTest() = delete;

        /// @brief invalidated copy constructor
        DialogTest(const DialogTest&) = delete;
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

    /// @brief constructor for parsing step in strin format
    InternalTestStep(InternalTest* testSystem, const std::string& step);

    /// @brief constructor for shortcuts
    InternalTestStep(InternalTest* testSystem, FXSelector messageType, FXSelector messageID,
                     Category category);

    /// @brief constructor for input events (click, keyPress, etc.)
    InternalTestStep(InternalTest* testSystem, FXSelector messageType, Category category,
                     FXEvent* event, const bool updateView);

    /// @brief constructor for fix dialogs
    InternalTestStep(InternalTestStep* parent, const std::string& solution);

    /// @brief constructor for key steps (only used for dialog steps)
    InternalTestStep(InternalTestStep* parent, FXSelector messageType, FXEvent* event);

    /// @brief destructor
    ~InternalTestStep();

    /// @brief get message type
    FXSelector getMessageType() const;

    /// @brief get message ID
    FXSelector getMessageID() const;

    /// @brief get dialog arguments
    DialogTest* getDialogTest() const;

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

    /// @brief get key events used in certain dialogs (allowDialog, etc.)
    const std::vector<const InternalTestStep*>& getModalDialogTestSteps() const;

private:
    /// @brief test system parent
    InternalTest* myTestSystem;

    /// @brief message type (by default SEL_COMMAND)
    FXSelector myMessageType = SEL_COMMAND;

    /// @brief message ID
    FXSelector myMessageID = 0;

    /// @brief step category
    Category myCategory = Category::META;

    /// @brief flag to enable or disable view after execute step
    bool myUpdateView = false;

    /// @brief arguments
    std::vector<std::string> myArguments;

    /// @brief list of events associated with this step
    FXEvent* myEvent = nullptr;

    /// @brief dialog test
    DialogTest* myDialogTest = nullptr;

    /// @brief TLS Table test
    TLSTableTest* myTLSTableTest = nullptr;

    /// @brief Test steps used in dialog test
    std::vector<const InternalTestStep*> myDialogTestSteps;

    /// @brief parse function and arguments
    std::string parseStep(const std::string& rowText);

    /// @brief parse arguments
    void parseArguments(const std::string& arguments);

    /// @brief process setupAndStart function
    void setupAndStart();

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

    /// @brief process changeEditMode function
    void changeEditMode();

    /// @brief process save function
    void saveExistentShortcut();

    /// @brief process check undo-redo function
    void checkUndoRedo() const;

    /// @brief process delete function
    void deleteFunction() const;

    /// @brief process selection function
    void selection() const;

    /// @brief process selectNetworkItems function
    void selectNetworkItems() const;

    /// @brief process check undo function
    void undo() const;

    /// @brief process check redo function
    void redo() const;

    /// @brief process supermode function
    void changeSupermode();

    /// @brief process change mode function
    void changeMode();

    /// @brief process change element function
    void changeElement() const;

    /// @bief process change plan function
    void changePlan() const;

    /// @brief process compute junctions function
    void computeJunctions();

    /// @brief process compute junctions with volatile options function
    void computeJunctionsVolatileOptions();

    /// @brief create rectangle shape
    void selectAdditionalChild();

    /// @brief process create rectangle shape function
    void createRectangledShape();

    /// @brief process create squared shape function
    void createSquaredShape();

    /// @brief process create line shape function
    void createLineShape();

    /// @brief process create mean data function
    void createMeanData();

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
    void modifyStringAttribute(const int tabs, const int overlappedTabs, const std::string& value) const;

    /// @brief modify bool attribute
    void modifyBoolAttribute(const int tabs, const int overlappedTabs) const;

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
    void buildPressKeyEvent(const std::string& key, const bool updateView) const;

    /// @brief build a key press and key release (used for tabs, spaces, enter, etc)
    void buildPressKeyEvent(InternalTestStep* parent, const std::string& key) const;

    /// @brief build a two key press and key release (used for tabs, spaces, enter, etc)
    void buildTwoPressKeyEvent(const std::string& keyA, const std::string& keyB, const bool updateView) const;

    /// @brief build a two key press and key release (used for tabs, spaces, enter, etc)
    void buildTwoPressKeyEvent(InternalTestStep* parent, const std::string& keyA, const std::string& keyB) const;

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
                            const int offsetEndX, const int offsetEndY) const;

    /// @brief build mouse move event
    FXEvent* buildMouseMoveEvent(const InternalTest::ViewPosition& viewPosition,
                                 const int offsetX, const int offsetY, const int clickedButton) const;

    /// @brief build mouse left click press event
    FXEvent* buildMouseClickEvent(FXSelType type, const InternalTest::ViewPosition& viewPosition,
                                  const int offsetX, const int offsetY, const std::string& keyModifier) const;

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

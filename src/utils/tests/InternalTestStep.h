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

// ===========================================================================
// class declaration
// ===========================================================================

class InternalTest;

// ===========================================================================
// class definitions
// ===========================================================================

class InternalTestStep {

public:
    /// @name category step
    enum class Category {
        META,   // Meta step (used for packing set of steps like click or write)
        INIT,   // Setup and start step
        APP,    // send signal to APP (Either GUIAppWindows or GNEApplicationWindow)
        VIEW,   // send signal to view (either GUIView or GNEViewNet)
        COLOR,  // send signal to color dialog
    };

    /// @brief constructor for parsing step in strin format
    InternalTestStep(InternalTest* testSystem, const std::string& step);

    /// @brief constructor for shortcuts
    InternalTestStep(InternalTest* testSystem, FXSelector messageType, FXSelector messageID,
                     Category category);

    /// @brief constructor for input events (click, keyPress, etc.)
    InternalTestStep(InternalTest* testSystem, FXSelector messageType, Category category,
                     FXEvent* event, const bool updateView);

    /// @brief constructor for key steps (only used for dialog steps)
    InternalTestStep(InternalTestStep* parent, FXSelector messageType, FXEvent* event);

    /// @brief destructor
    ~InternalTestStep();

    /// @brief get message type
    FXSelector getMessageType() const;

    /// @brief get message ID
    FXSelector getMessageID() const;

    /// @brief get selector (based in messageType and messageID)
    FXSelector getSelector() const;

    /// @brief check if update view
    bool updateView() const;

    /// @brief get category
    Category getCategory() const;

    /// @brief get event associated with this step
    void* getEvent() const;

    /// @brief get key events used in certain dialogs (color, allowDialog, etc.)
    const std::vector<const InternalTestStep*>& getKeySteps() const;

protected:
    /// @brief build mouse move event
    FXEvent* buildMouseMoveEvent(const int posX, const int posY) const;

    /// @brief build mouse left click press event
    FXEvent* buildMouseLeftClickPressEvent(const int posX, const int posY) const;

    /// @brief build mouse left click release event
    FXEvent* buildMouseLeftClickReleaseEvent(const int posX, const int posY) const;

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

    /// @brief key events used in certain dialogs (color, allowDialog, etc.)
    std::vector<const InternalTestStep*> myKeySteps;

    /// @brief parse function and arguments
    std::string parseStep(const std::string& rowText);

    /// @brief parse arguments
    void parseArguments(const std::string& arguments);

    /// @brief process setupAndStart function
    void processSetupAndStartFunction();

    /// @brief process left click function
    void processLeftClickFunction(const std::string& modifier) const;

    /// @brief process typeKey function
    void processTypeKeyFunction() const;

    /// @brief process modifyAttribute function
    void processModifyAttributeFunction() const;

    /// @brief process modifyAttributeOverlapped function
    void processModifyAttributeOverlappedFunction() const;

    /// @brief process modifyBoolAttribute function
    void processModifyBoolAttributeFunction() const;

    /// @brief process modifyBoolAttributeOverlapped function
    void processModifyBoolAttributeOverlappedFunction() const;

    /// @brief process modifyColorAttribute function
    void processModifyColorAttributeFunction() const;

    /// @brief process modifyColorAttributeOverlapped function
    void processModifyColorAttributeOverlappedFunction() const;

    /// @brief process changeEditMode function
    void processChangeEditModeFunction();

    /// @brief process save function
    void processSaveExistentShortcutFunction();

    /// @brief process check undo-redo function
    void processCheckUndoRedoFunction() const;

    /// @brief process delete function
    void processDeleteFunction() const;

    /// @brief process selection function
    void processSelectionFunction() const;

    /// @brief process check undo function
    void processUndoFunction() const;

    /// @brief process check redo function
    void processRedoFunction() const;

    /// @brief process supermode function
    void processChangeSupermodeFunction();

    /// @brief process change mode function
    void processChangeModeFunction();

    /// @brief process change element function
    void processChangeElementArgument() const;

    /// @brief process compute function
    void processComputeFunction();

    /// @brief process quit function
    void processQuitFunction();

    /// @brief check int argument
    bool checkIntArgument(const std::string& argument, const std::map<std::string, int>& map) const;

    /// @brief get int argument
    int getIntArgument(const std::string& argument, const std::map<std::string, int>& map) const;

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
    void writeError(const std::string& function, const std::string& expected) const;

    /// @name key functions
    /// @{

    /// @brief translate key
    template<typename T>
    std::pair<FXint, FXString> translateKey(const T key) const;

    /// @brief build key press event
    template<typename T>
    FXEvent* buildKeyPressEvent(const T key) const {
        const auto keyValues = translateKey(key);
        FXEvent* keyPressEvent = new FXEvent();
        // set event values
        keyPressEvent->type = SEL_KEYPRESS;
        keyPressEvent->code = keyValues.first;
        keyPressEvent->text = keyValues.second;
        return keyPressEvent;
    }

    /// @brief build key release event
    template<typename T>
    FXEvent* buildKeyReleaseEvent(const T key) const {
        const auto keyValues = translateKey(key);
        FXEvent* keyPressEvent = new FXEvent();
        // set event values
        keyPressEvent->type = SEL_KEYPRESS;
        keyPressEvent->code = keyValues.first;
        keyPressEvent->text = keyValues.second;
        return keyPressEvent;
    }

    /// @brief build a key press and key release (used for tabs, spaces, enter, etc)
    template<typename T>
    InternalTestStep* buildPressKeyEvent(const T key, const bool updateView) const {
        new InternalTestStep(myTestSystem, SEL_KEYPRESS, Category::APP, buildKeyPressEvent(key), updateView);
        return new InternalTestStep(myTestSystem, SEL_KEYRELEASE, Category::APP, buildKeyReleaseEvent(key), updateView);
    }

    /// @brief build a key press and key release (used for tabs, spaces, enter, etc)
    template<typename T>
    void buildPressKeyEvent(InternalTestStep* parent, const T key) const {
        new InternalTestStep(parent, SEL_KEYPRESS, buildKeyPressEvent(key));
        new InternalTestStep(parent, SEL_KEYRELEASE, buildKeyReleaseEvent(key));
    }

    /// @brief build a two key press and key release (used for tabs, spaces, enter, etc)
    template<typename T, typename J>
    InternalTestStep* buildTwoPressKeyEvent(const T keyA, const J keyB, const bool updateView) const {
        new InternalTestStep(myTestSystem, SEL_KEYPRESS, Category::APP, buildKeyPressEvent(keyA), updateView);
        new InternalTestStep(myTestSystem, SEL_KEYPRESS, Category::APP, buildKeyPressEvent(keyB), updateView);
        new InternalTestStep(myTestSystem, SEL_KEYRELEASE, Category::APP, buildKeyReleaseEvent(keyB), updateView);
        return new InternalTestStep(myTestSystem, SEL_KEYRELEASE, Category::APP, buildKeyReleaseEvent(keyA), updateView);
    }

    /// @brief build a two key press and key release (used for tabs, spaces, enter, etc)
    template<typename T, typename J>
    void buildTwoPressKeyEvent(InternalTestStep* parent, const T keyA, const J keyB) const {
        new InternalTestStep(parent, SEL_KEYPRESS, buildKeyPressEvent(keyA));
        new InternalTestStep(parent, SEL_KEYPRESS, buildKeyPressEvent(keyB));
        new InternalTestStep(parent, SEL_KEYRELEASE, buildKeyReleaseEvent(keyB));
        new InternalTestStep(parent, SEL_KEYRELEASE, buildKeyReleaseEvent(keyA));
    }

    /// @}

    /// @brief invalidate default constructor
    InternalTestStep() = delete;

    /// @brief Invalidated copy constructor.
    InternalTestStep(const InternalTestStep&) = delete;

    /// @brief Invalidated assignment operator
    InternalTestStep& operator=(const InternalTestStep& src) = delete;
};

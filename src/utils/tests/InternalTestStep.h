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
    };

    /// @brief constructor for parsing step in strin format
    InternalTestStep(InternalTest* testSystem, const std::string& step);

    /// @brief constructor for shortcuts
    InternalTestStep(InternalTest* testSystem, FXSelector messageType, FXSelector messageID,
                     Category category);

    /// @brief constructor for input events (click, keyPress, etc.)
    InternalTestStep(InternalTest* testSystem, FXSelector messageType, Category category,
                     FXEvent* event, const bool updateView);

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

protected:
    /// @brief build mouse move event
    FXEvent* buildMouseMoveEvent(const int posX, const int posY) const;

    /// @brief build mouse left click press event
    FXEvent* buildMouseLeftClickPressEvent(const int posX, const int posY) const;

    /// @brief build mouse left click release event
    FXEvent* buildMouseLeftClickReleaseEvent(const int posX, const int posY) const;

    /// @brief build key press event (string format, for special keys)
    FXEvent* buildKeyPressEvent(const std::string& key) const;

    /// @brief build key press event (char format, for basic characters)
    FXEvent* buildKeyPressEvent(const char key) const;

    /// @brief build key release event (string format, for special keys)
    FXEvent* buildKeyReleaseEvent(const std::string& key) const;

    /// @brief build key release event (char format, for basic characters)
    FXEvent* buildKeyReleaseEvent(const char key) const;

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

    /// @brief parse function and arguments
    std::string parseStep(const std::string& rowText);

    /// @brief parse arguments
    void parseArguments(const std::string& arguments);

    /// @brief translate key
    std::pair<FXint, FXString> translateKey(const std::string& key) const;

    /// @brief process setupAndStart function
    void processSetupAndStartFunction();

    /// @brief process left click function
    void processLeftClickFunction() const;

    /// @brief process modifyAttribute function
    void processModifyAttributeFunction() const;

    /// @brief process modifyAttributeOverlapped function
    void processModifyAttributeOverlappedFunction() const;

    /// @brief process save function
    void processSaveExistentShortcutFunction();

    /// @brief process check undo-redo function
    void processCheckUndoRedoFunction() const ;

    /// @brief process check undo function
    void processUndoFunction() const ;

    /// @brief process check redo function
    void processRedoFunction() const ;

    /// @brief process supermode function
    void processSupermodeFunction();

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

    /// @brief invalidate default constructor
    InternalTestStep() = delete;

    /// @brief Invalidated copy constructor.
    InternalTestStep(const InternalTestStep&) = delete;

    /// @brief Invalidated assignment operator
    InternalTestStep& operator=(const InternalTestStep& src) = delete;
};

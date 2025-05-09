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
/// @file    GUITestSystem.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Thread used for testing netedit
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>

#include <utils/foxtools/fxheader.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GUISUMOAbstractView;
class GUIMainWindow;

// ===========================================================================
// class definitions
// ===========================================================================

class GUITestSystem : public FXObject, public FXThread {

public:
    /// @brief constructor
    GUITestSystem();

    /// @brief destructor
    ~GUITestSystem();

    /// @brief start test. The argument is either GNEApplicationWindow or GUIApplicationWindows
    void startTests(GUISUMOAbstractView* view, GUIMainWindow* mainWindow);

    /// @brief execute next test
    void nextTest(FXObject* sender, FXSelector sel);

    /// @brief 
    void writeSignalInfo(FXObject* sender, FXSelector sel) const;

    /// @brief run all tests
    int run();

protected:

    /// @brief test step
    struct TestStep {

        /// @brief constructor using a row
        TestStep(GUITestSystem* testSystem, const std::string &row);

        /// @brief constructor using parameters (needed for certain functions like click)
        TestStep(FXSelector messageType, FXSelector messageID, const std::string &category,
                 const std::string &function, const std::vector<std::string> &arguments,
                 FXString* text, FXEvent* event);

        /// @brief destructor
        ~TestStep();

        /// @brief get message type
        FXSelector getMessageType() const;
        
        /// @brief get message ID
        FXSelector getMessageID() const;

        /// @brief get selector (based in messageType and messageID)
        FXSelector getSelector() const;

        /// @brief get category
        const std::string &getCategory() const;

        /// @brief get function
        const std::string &getFunction() const;

        /// @brief get text
        FXString* getText() const;

        /// @brief get event associated with this step
        const FXEvent* getEvent() const;

    protected:
        /// @brief build mouse move event
        FXEvent* buildMouseMoveEvent(const int posX, const int posY) const;

        /// @brief build mouse left click press event
        FXEvent* buildMouseLeftClickPressEvent(const int posX, const int posY) const;

        /// @brief build mouse left click release event
        FXEvent* buildMouseLeftClickReleaseEvent(const int posX, const int posY) const;

    private:
        /// @brief message type (by default SEL_COMMAND)
        FXSelector myMessageType = SEL_COMMAND;
        
        /// @brief message ID
        FXSelector myMessageID = 0;

        // @brief category
        std::string myCategory;
        
        /// @brief function
        std::string myFunction;

        /// @brief arguments
        std::vector<std::string> myArguments;

        /// @brief tag (used in certain tests)
        FXString* myText = nullptr;

        /// @brief list of events associated with this step
        FXEvent* myEvent;

        /// @brief parse function and arguments
        void parseFunctionAndArguments(const std::string &row);

        /// @brief invalidate default constructor
        TestStep() = delete;
    };

    /// @brief run specific test
    virtual void setSpecificMainWindow(GUIMainWindow* mainWindow) = 0;

    /// @brief run specific test
    virtual void runSpecificTest(const TestStep* testStep) = 0;

private:
    /// @brief test steps
    std::vector<TestStep*> myTestSteps;

    /// @brief flag to check if test are initedinited
    bool myInitedTest = false;

    /// @brief flag used for continue
    bool myContinue = true;

    /// @brief current selector
    FXSelector myCurrentSelector = 0;

    /// @brief abstract view
    GUISUMOAbstractView* myAbstractView = nullptr;

    /// @brief wait for continue
    void waitForContinue() const;

    /// @brief process test file
    void processTestFile();
};

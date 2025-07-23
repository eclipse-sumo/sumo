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
/// @file    InternalTest.h
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Class used for internal tests
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <map>

#include <utils/foxtools/fxheader.h>

// ===========================================================================
// class declaration
// ===========================================================================

class InternalTestStep;

// ===========================================================================
// class definitions
// ===========================================================================

class InternalTest : public FXObject {

public:
    /// @brief view position
    class ViewPosition {

    public:
        /// @brief default constructor
        ViewPosition();

        /// @brief parameter constructor (string)
        ViewPosition(const int xValue, const int yValue);

        /// @brief parameter constructor
        ViewPosition(const std::string& xValue, const std::string& yValue);

        ///  @brief get x value
        int getX() const;

        /// @brief get y value
        int getY() const;

    private:
        /// @brief x value
        int myX = 0;

        /// @brief y value
        int myY = 0;
    };

    /// @brief contextual menu
    struct ContextualMenu {

    public:
        /// @brief default constructor
        ContextualMenu();

        /// @brief constructor
        ContextualMenu(const std::string& mainMenuValue, const std::string& subMenuAValue,
                       const std::string& subMenuBValue);

        /// @brief get main menu position
        int getMainMenuPosition() const;

        /// @brief get submenu A position
        int getSubMenuAPosition() const;

        /// @brief get submenu B position
        int getSubMenuBPosition() const;

    private:
        /// @brief main manue
        int myMainMenu = 0;

        /// @brief submenu A
        int mySubMenuA = 0;

        /// @brief submenu B
        int mySubMenuB = 0;
    };

    /// @brief view position
    class Movement {

    public:
        /// @brief default constructor
        Movement();

        /// @brief constructor
        Movement(const std::string& up, const std::string& down,
                 const std::string& left, const std::string& right);

        ///  @brief get up value
        int getUp() const;

        /// @brief get down value
        int getDown() const;

        /// @brief get left value
        int getLeft() const;

        /// @brief get right value
        int getRight() const;

    private:
        /// @brief up value
        int myUp = 0;

        /// @brief down value
        int myDown = 0;

        /// @brief left value
        int myLeft = 0;

        /// @brief right value
        int myRight = 0;
    };

    /// @brief constructor
    InternalTest(const std::string& testFile);

    /// @brief destructor
    ~InternalTest();

    /// @brief check if test is running
    bool isRunning() const;

    /// @brief get currentTime
    FXint getTime() const;

    /// @brief add test steps
    void addTestSteps(InternalTestStep* internalTestStep);

    /// @brief get current step
    InternalTestStep* getCurrentStep() const;

    /// @brief get last test step
    InternalTestStep* getLastTestStep() const;

    /// @brief get map with attributesEnum jump steps
    const std::map<std::string, int>& getAttributesEnum() const;

    /// @brief get map with contextual menu operation jump steps
    const std::map<std::string, InternalTest::ContextualMenu>& getContextualMenuOperations() const;

    /// @brief get map with view position pairs
    const std::map<std::string, InternalTest::ViewPosition>& getViewPositions() const;

    /// @brief get map with movement pairs
    const std::map<std::string, InternalTest::Movement>& getMovements() const;

    /// @brief get last moved position
    const InternalTest::ViewPosition& getLastMovedPosition() const;

    /// @brief update last moved position
    void updateLastMovedPosition(const int x, const int y);

    /// @brief interpolate view positions
    std::vector<InternalTest::ViewPosition> interpolateViewPositions(
        const InternalTest::ViewPosition& viewStartPosition,
        const int offsetStartX, const int offsetStartY,
        const InternalTest::ViewPosition& viewEndPosition,
        const int offsetEndX, const int offsetEndY) const;

protected:
    /// @brief test steps
    std::vector<InternalTestStep*> myTestSteps;

    /// @brief flag to indicate if test is running
    bool myRunning = false;

    /// @brief current step index
    size_t myCurrentStep = 0;

    /// @brief vector with attributesEnum jump steps
    std::map<std::string, int> myAttributesEnum;

    /// @brief vector with contextual menu operation jump steps
    std::map<std::string, InternalTest::ContextualMenu> myContextualMenuOperations;

    /// @brief vector with view positions
    std::map<std::string, InternalTest::ViewPosition> myViewPositions;

    /// @brief vector with movements
    std::map<std::string, InternalTest::Movement> myMovements;

    /// @brief last moved position
    InternalTest::ViewPosition myLastMovedPosition;

    /// @brief parse attributesEnum file
    std::map<std::string, int> parseAttributesEnumFile(const std::string filePath) const;

    /// @brief parse attributesEnum file
    std::map<std::string, InternalTest::ContextualMenu> parseContextualMenuOperationsFile(const std::string filePath) const;

    /// @brief parse viewPositions file
    std::map<std::string, InternalTest::ViewPosition> parseViewPositionsFile(const std::string filePath) const;

    /// @brief parse movements file
    std::map<std::string, InternalTest::Movement> parseMovementsFile(const std::string filePath) const;

    /// @brief clear lines
    std::vector<std::string> cleanLines(const std::vector<std::pair<bool, std::string> >& linesRaw) const;

    /// @brief check if the given string start with
    bool startWith(const std::string& str, const std::string& prefix) const;

private:
    /// @brief invalidate default constructor
    InternalTest() = delete;

    /// @brief Invalidated copy constructor.
    InternalTest(const InternalTest&) = delete;

    /// @brief Invalidated assignment operator
    InternalTest& operator=(const InternalTest& src) = delete;
};

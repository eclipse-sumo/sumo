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
/// @file    InternalTest.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2025
///
// Class used for internal tests
/****************************************************************************/
#include <config.h>

#include <fstream>
#include <utils/common/MsgHandler.h>

#include "InternalTest.h"
#include "InternalTestStep.h"

#ifdef _MSC_VER
// disable using unsecure functions (getenv)
#pragma warning(disable:4996)
#endif

// define number of points to interpolate
#define numPointsInterpolation 100

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// InternalTest::ViewPosition - public methods
// ---------------------------------------------------------------------------

InternalTest::ViewPosition::ViewPosition() {}


InternalTest::ViewPosition::ViewPosition(const int x, const int y) :
    myX(x),
    myY(y) {
}


InternalTest::ViewPosition::ViewPosition(const std::string& x, const std::string& y) :
    myX(StringUtils::toInt(x)),
    myY(StringUtils::toInt(y)) {
}


int
InternalTest::ViewPosition::getX() const {
    return myX;
}


int
InternalTest::ViewPosition::getY() const {
    return myY;
}

// ---------------------------------------------------------------------------
// InternalTest::ContextualMenu - public methods
// ---------------------------------------------------------------------------

InternalTest::ContextualMenu::ContextualMenu() {}


InternalTest::ContextualMenu::ContextualMenu(const std::string& mainMenuValue,
        const std::string& subMenuAValue, const std::string& subMenuBValue) :
    myMainMenu(StringUtils::toInt(mainMenuValue)),
    mySubMenuA(StringUtils::toInt(subMenuAValue)),
    mySubMenuB(StringUtils::toInt(subMenuBValue)) {
}


int
InternalTest::ContextualMenu::getMainMenuPosition() const {
    return myMainMenu;
}


int
InternalTest::ContextualMenu::getSubMenuAPosition() const {
    return mySubMenuA;
}


int
InternalTest::ContextualMenu::getSubMenuBPosition() const {
    return mySubMenuB;
}

// ---------------------------------------------------------------------------
// InternalTest::Movement - public methods
// ---------------------------------------------------------------------------

InternalTest::Movement::Movement() {}


InternalTest::Movement::Movement(const std::string& up, const std::string& down,
                                 const std::string& left, const std::string& right) :
    myUp(StringUtils::toInt(up)),
    myDown(StringUtils::toInt(down)),
    myLeft(StringUtils::toInt(left)),
    myRight(StringUtils::toInt(right)) {
}


int
InternalTest::Movement::getUp() const {
    return myUp;
}


int
InternalTest::Movement::getDown() const {
    return myDown;
}


int
InternalTest::Movement::getLeft() const {
    return myLeft;
}


int
InternalTest::Movement::getRight() const {
    return myRight;
}

// ---------------------------------------------------------------------------
// InternalTest - public methods
// ---------------------------------------------------------------------------

InternalTest::InternalTest(const std::string& testFile) {
    // locate sumo home directory
    const auto sumoHome = std::string(getenv("SUMO_HOME"));
    // load data files
    myAttributesEnum = parseAttributesEnumFile(sumoHome + "/data/tests/attributesEnum.txt");
    myContextualMenuOperations = parseContextualMenuOperationsFile(sumoHome + "/data/tests/contextualMenuOperations.txt");
    myViewPositions = parseViewPositionsFile(sumoHome + "/data/tests/viewPositions.txt");
    myMovements = parseMovementsFile(sumoHome + "/data/tests/movements.txt");
    // open file
    std::ifstream strm(testFile);
    // check if file can be opened
    if (!strm.good()) {
        std::cout << "Could not open test file '" + testFile + "'." << std::endl;
        throw ProcessError();
    } else if (myAttributesEnum.empty() || myContextualMenuOperations.empty() || myViewPositions.empty() || myMovements.empty()) {
        std::cout << "Error loading test data files" << std::endl;
        throw ProcessError();
    } else {
        std::string line;
        std::vector<std::pair<bool, std::string> > linesRaw;
        // read full lines until end of file
        while (std::getline(strm, line)) {
            // filter lines
            if (!line.empty() &&                // emty lines
                    !(line[0] == '#') &&            // comments
                    !startWith(line, "import") &&   // imports
                    !startWith(line, "time.") &&    // time calls
                    !startWith(line, "sys.")) {     // sys calls
                linesRaw.push_back(std::make_pair(startWith(line, "netedit."), line));
            }
        }
        // clean lines
        const auto lines = cleanLines(linesRaw);
        // create steps
        new InternalTestStep(this, "netedit.setupAndStart");
        for (const auto& clearLine : lines) {
            new InternalTestStep(this, clearLine);
        }
        new InternalTestStep(this, "netedit.finish");
    }
}


InternalTest::~InternalTest() {
    // delete all test steps
    while (myInitialTestStep != nullptr) {
        // store next step
        auto nextStep = myInitialTestStep->getNextStep();
        // delete current step
        delete myInitialTestStep;
        // set next step as initial step
        myInitialTestStep = nextStep;
    }
}


void
InternalTest::addTestSteps(InternalTestStep* internalTestStep) {
    if (myLastTestStep == nullptr) {
        // set initial step
        myInitialTestStep = internalTestStep;
        myLastTestStep = internalTestStep;
        myCurrentTestStep = internalTestStep;
    } else {
        // set next step
        myLastTestStep->setNextStep(internalTestStep);
        myLastTestStep = internalTestStep;
    }
}


InternalTestStep*
InternalTest::getCurrentStep() const {
    return myCurrentTestStep;
}


InternalTestStep*
InternalTest::setNextStep() {
    const auto currentStep = myCurrentTestStep;
    myCurrentTestStep = myCurrentTestStep->getNextStep();
    return currentStep;
}


bool
InternalTest::isRunning() const {
    return myRunning;
}


void
InternalTest::stopTests() {
    myRunning = false;
}


FXint
InternalTest::getTime() const {
    return static_cast<FXuint>(
               std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::steady_clock::now().time_since_epoch()
               ).count());
}


const std::map<std::string, int>&
InternalTest::getAttributesEnum() const {
    return myAttributesEnum;
}


const std::map<std::string, InternalTest::ContextualMenu>&
InternalTest::getContextualMenuOperations() const {
    return myContextualMenuOperations;
}


const std::map<std::string, InternalTest::ViewPosition>&
InternalTest::getViewPositions() const {
    return myViewPositions;
}


const std::map<std::string, InternalTest::Movement>&
InternalTest::getMovements() const {
    return myMovements;
}


const InternalTest::ViewPosition&
InternalTest::getLastMovedPosition() const {
    return myLastMovedPosition;
}


void
InternalTest::updateLastMovedPosition(const int x, const int y) {
    myLastMovedPosition = InternalTest::ViewPosition(x, y);
}


std::vector<InternalTest::ViewPosition>
InternalTest::interpolateViewPositions(const InternalTest::ViewPosition& viewStartPosition,
                                       const int offsetStartX, const int offsetStartY,
                                       const InternalTest::ViewPosition& viewEndPosition,
                                       const int offsetEndX, const int offsetEndY) const {
    // declare trajectory vector
    std::vector<InternalTest::ViewPosition> trajectory;
    trajectory.reserve(numPointsInterpolation);
    // calulate from using offsets
    const auto from = InternalTest::ViewPosition(viewStartPosition.getX() + offsetStartX, viewStartPosition.getY() + offsetStartY);
    const auto to = InternalTest::ViewPosition(viewEndPosition.getX() + offsetEndX, viewEndPosition.getY() + offsetEndY);
    // itearte over the number of points to interpolate
    for (int i = 0; i < numPointsInterpolation; i++) {
        const double t = static_cast<double>(i) / (numPointsInterpolation - 1); // t in [0, 1]
        // calculate interpolated position
        const int interpolatedX = int(from.getX() + t * (to.getX() - from.getX()));
        const int interpolatedY = int(from.getY() + t * (to.getY() - from.getY()));
        // add interpolated position
        trajectory.push_back(ViewPosition(interpolatedX, interpolatedY));
    }
    return trajectory;
}


std::map<std::string, int>
InternalTest::parseAttributesEnumFile(const std::string filePath) const {
    std::map<std::string, int> solution;
    // open file
    std::ifstream strm(filePath);
    // check if file can be opened
    if (!strm.good()) {
        WRITE_ERRORF(TL("Could not open attributes enum file '%'."), filePath);
    } else {
        std::string line;
        // read full lines until end of file
        while (std::getline(strm, line)) {
            // use stringstream for
            std::stringstream ss(line);
            // read key and value
            std::string key;
            std::string value;
            std::getline(ss, key, ' ');
            std::getline(ss, value, '\n');
            // check that int can be parsed
            if (!StringUtils::isInt(value)) {
                WRITE_ERRORF(TL("In internal test file, value '%' cannot be parsed to int."), value);
            } else {
                solution[key] = StringUtils::toInt(value);
            }
        }
    }
    return solution;
}


std::map<std::string, InternalTest::ContextualMenu>
InternalTest::parseContextualMenuOperationsFile(const std::string filePath) const {
    std::map<std::string, InternalTest::ContextualMenu> solution;
    // open file
    std::ifstream strm(filePath);
    // check if file can be opened
    if (!strm.good()) {
        WRITE_ERRORF(TL("Could not open view positions file '%'."), filePath);
    } else {
        std::string line;
        // read full lines until end of file
        while (std::getline(strm, line)) {
            // read key and value
            std::string mainMenuKey;
            std::string mainMenuValue;
            std::string subMenuAKey;
            std::string subMenuAValue;
            std::string subMenuBKey;
            std::string subMenuBValue;
            // parse first line
            std::stringstream mainMenuSS(line);
            std::getline(mainMenuSS, mainMenuKey, ' ');
            std::getline(mainMenuSS, mainMenuValue, '\n');
            // parse second line
            std::getline(strm, line);
            std::stringstream subMenuASS(line);
            std::getline(subMenuASS, subMenuAKey, ' ');
            std::getline(subMenuASS, subMenuAValue, '\n');
            // parse third line
            std::getline(strm, line);
            std::stringstream subMenuBSS(line);
            std::getline(subMenuBSS, subMenuBKey, ' ');
            std::getline(subMenuBSS, subMenuBValue, '\n');
            // check that int can be parsed
            if (!StringUtils::isInt(mainMenuValue)) {
                WRITE_ERRORF(TL("In internal test file, mainMenu value '%' cannot be parsed to int."), mainMenuValue);
            } else if (!StringUtils::isInt(subMenuAValue)) {
                WRITE_ERRORF(TL("In internal test file, subMenuA value '%' cannot be parsed to int."), subMenuAValue);
            } else if (!StringUtils::isInt(subMenuBValue)) {
                WRITE_ERRORF(TL("In internal test file, subMenuB value '%' cannot be parsed to int."), subMenuBValue);
            } else {
                // remove '.mainMenuPosition' from mainMenuKey
                solution[mainMenuKey.erase(mainMenuKey.size() - 17)] = InternalTest::ContextualMenu(mainMenuValue, subMenuAValue, subMenuBValue);
            }
        }
    }
    return solution;
}


std::map<std::string, InternalTest::ViewPosition>
InternalTest::parseViewPositionsFile(const std::string filePath) const {
    std::map<std::string, InternalTest::ViewPosition> solution;
    // open file
    std::ifstream strm(filePath);
    // check if file can be opened
    if (!strm.good()) {
        WRITE_ERRORF(TL("Could not open view positions file '%'."), filePath);
    } else {
        std::string line;
        // read full lines until end of file
        while (std::getline(strm, line)) {
            // use stringstream for
            std::stringstream ss(line);
            // read key and value
            std::string key;
            std::string xValue;
            std::string yValue;
            std::getline(ss, key, ' ');
            std::getline(ss, xValue, ' ');
            std::getline(ss, yValue, '\n');
            // check that int can be parsed
            if (!StringUtils::isInt(xValue)) {
                WRITE_ERRORF(TL("In internal test file, x value '%' cannot be parsed to int."), xValue);
            } else if (!StringUtils::isInt(yValue)) {
                WRITE_ERRORF(TL("In internal test file, y value '%' cannot be parsed to int."), yValue);
            } else {
                solution[key] = InternalTest::ViewPosition(xValue, yValue);
            }
        }
    }
    return solution;
}


std::map<std::string, InternalTest::Movement>
InternalTest::parseMovementsFile(const std::string filePath) const {
    std::map<std::string, InternalTest::Movement> solution;
    // open file
    std::ifstream strm(filePath);
    // check if file can be opened
    if (!strm.good()) {
        WRITE_ERRORF(TL("Could not open view positions file '%'."), filePath);
    } else {
        std::string line;
        // read full lines until end of file
        while (std::getline(strm, line)) {
            // use stringstream for
            std::stringstream ss(line);
            // read key and value
            std::string key;
            std::string upValue;
            std::string downValue;
            std::string leftValue;
            std::string rightValue;
            std::getline(ss, key, ' ');
            std::getline(ss, upValue, ' ');
            std::getline(ss, downValue, ' ');
            std::getline(ss, leftValue, ' ');
            std::getline(ss, rightValue, '\n');
            // check that int can be parsed
            if (!StringUtils::isInt(upValue)) {
                WRITE_ERRORF(TL("In internal test file, x value '%' cannot be parsed to int."), upValue);
            } else if (!StringUtils::isInt(downValue)) {
                WRITE_ERRORF(TL("In internal test file, y value '%' cannot be parsed to int."), downValue);
            } else if (!StringUtils::isInt(leftValue)) {
                WRITE_ERRORF(TL("In internal test file, y value '%' cannot be parsed to int."), leftValue);
            } else if (!StringUtils::isInt(rightValue)) {
                WRITE_ERRORF(TL("In internal test file, y value '%' cannot be parsed to int."), rightValue);
            } else {
                solution[key] = InternalTest::Movement(upValue, downValue, leftValue, rightValue);
            }
        }
    }
    return solution;
}


std::vector<std::string>
InternalTest::cleanLines(const std::vector<std::pair<bool, std::string> >& linesRaw) const {
    std::vector<std::string> results;
    for (const auto& lineRaw : linesRaw) {
        if (lineRaw.first) {
            results.push_back(lineRaw.second);
        } else if (results.size() > 0) {
            results.back().append(lineRaw.second);
        }
    }
    return results;
}


bool
InternalTest::startWith(const std::string& str, const std::string& prefix) const {
    if (prefix.size() > str.size()) {
        return false;
    } else {
        for (int i = 0; i < (int)prefix.size(); i++) {
            if (str[i] != prefix[i]) {
                return false;
            }
        }
        return true;
    }
}

/****************************************************************************/

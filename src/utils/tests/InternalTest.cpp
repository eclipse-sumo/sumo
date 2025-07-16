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

// ===========================================================================
// member method definitions
// ===========================================================================

InternalTest::ViewPosition::ViewPosition(const std::string& xValue, const std::string& yValue) :
    x(StringUtils::toInt(xValue)),
    y(StringUtils::toInt(yValue)) {
}


InternalTest::ContextualMenu::ContextualMenu(const std::string& mainMenuValue, const std::string& subMenuAValue,
        const std::string& subMenuBValue) :
    mainMenu(StringUtils::toInt(mainMenuValue)),
    subMenuA(StringUtils::toInt(subMenuAValue)),
    subMenuB(StringUtils::toInt(subMenuBValue)) {
}


InternalTest::InternalTest(const std::string& testFile) {
    // locate sumo home directory
    const auto sumoHome = std::string(getenv("SUMO_HOME"));
    // load data files
    myAttributesEnum = parseAttributesEnumFile(sumoHome + "/data/tests/attributesEnum.txt");
    myContextualMenuOperations = parseContextualMenuOperationsFile(sumoHome + "/data/tests/contextualMenuOperations.txt");
    myViewPositions = parseViewPositionsFile(sumoHome + "/data/tests/viewPositions.txt");
    // open file
    std::ifstream strm(testFile);
    // check if file can be opened
    if (!strm.good()) {
        std::cout << "Could not open test file '" + testFile + "'." << std::endl;
        throw ProcessError();
    } else if (myAttributesEnum.empty() || myContextualMenuOperations.empty() || myViewPositions.empty()) {
        std::cout << "Error loading test data files" << std::endl;
        throw ProcessError();
    } else {
        std::string line;
        std::vector<std::pair<bool, std::string> > linesRaw;
        // read full lines until end of file
        while (std::getline(strm, line)) {
            // ignore comments (#) and all lines that doesn't start with netedit.
            if (!line.empty() && (line[0] != '#')) {
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
    }
}


InternalTest::~InternalTest() {
    for (auto testStep : myTestSteps) {
        delete testStep;
    }
}


const FXint
InternalTest::getTime() const {
    return static_cast<FXuint>(
               std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::steady_clock::now().time_since_epoch()
               ).count());
}


void
InternalTest::addTestSteps(InternalTestStep* internalTestStep) {
    myTestSteps.push_back(internalTestStep);
}


InternalTestStep*
InternalTest::getCurrentStep() const {
    if (myCurrentStep < myTestSteps.size()) {
        return myTestSteps.at(myCurrentStep);
    } else {
        return nullptr;
    }
}


InternalTestStep*
InternalTest::getLastTestStep() const {
    if (myTestSteps.empty()) {
        return nullptr;
    } else {
        return myTestSteps.back();
    }
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


const
std::pair<FXint, FXint> InternalTest::getLastMovedPosition() const {
    return myLastMovedPosition;
}


void
InternalTest::updateLastMovedPosition(const FXint x, const FXint y) {
    myLastMovedPosition.first = x;
    myLastMovedPosition.second = y;
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

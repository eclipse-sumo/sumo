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

InternalTest::InternalTest(const std::string& testFile) {
    const auto sumoHome = std::string(getenv("SUMO_HOME"));
    // load data files
    myAttributesEnum = parseIntTestDataFile(sumoHome + "\\data\\tests\\attributesEnum.txt");
    myContextualMenuOperations = parseIntTestDataFile(sumoHome + "\\data\\tests\\contextualMenuOperations.txt");
    myViewPositions = parsePositionTestDataFile(sumoHome + "\\data\\tests\\viewPositions.txt");
    // open file
    std::ifstream strm(testFile);
    // check if file can be opened
    if (!strm.good()) {
        WRITE_ERRORF(TL("Could not open test file '%'."), testFile);
    } else {
        std::string line;
        // read full lines until end of file
        while (std::getline(strm, line)) {
            // ignore comments (#) and all lines that doesn't start with netedit.
            if (!line.empty() && (line[0] != '#') && (line.compare(0, 8, "netedit.") == 0)) {
                new InternalTestStep(this, line);
            }
        }
    }
}


InternalTest::~InternalTest() {
    for (auto testStep : myTestSteps) {
        delete testStep;
    }
}


bool
InternalTest::isTestFinished() const {
    return myTestFinished;
}


std::map<std::string, int>
InternalTest::parseIntTestDataFile(const std::string filePath) const {
    std::map<std::string, int> solution;
    // open file
    std::ifstream strm(filePath);
    // check if file can be opened
    if (!strm.good()) {
        WRITE_ERRORF(TL("Could not open internal test data file '%'."), filePath);
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


std::map<std::string, std::pair<int, int> >
InternalTest::parsePositionTestDataFile(const std::string filePath) const {
    std::map<std::string, std::pair<int, int> > solution;
    // open file
    std::ifstream strm(filePath);
    // check if file can be opened
    if (!strm.good()) {
        WRITE_ERRORF(TL("Could not open internal test data file '%'."), filePath);
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
                solution[key] = std::make_pair(StringUtils::toInt(xValue), StringUtils::toInt(yValue));
            }
        }
    }
    return solution;
}

/****************************************************************************/

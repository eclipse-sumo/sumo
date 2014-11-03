/****************************************************************************/
/// @file    LineReader.cpp
/// @author  Daniel Krajzewicz
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// Retrieves a file linewise and reports the lines to a handler.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <utils/common/UtilExceptions.h>
#include "LineHandler.h"
#include "LineReader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
LineReader::LineReader() {}


LineReader::LineReader(const std::string& file)
    : myFileName(file),
      myRead(0) {
    reinit();
}


LineReader::~LineReader() {}


bool
LineReader::hasMore() const {
    return myRread < myAvailable;
}


void
LineReader::readAll(LineHandler& lh) {
    while (myRread < myAvailable) {
        if (!readLine(lh)) {
            return;
        }
    }
}


bool
LineReader::readLine(LineHandler& lh) {
    std::string toReport;
    bool moreAvailable = true;
    while (toReport.length() == 0) {
        size_t idx = myStrBuffer.find('\n');
        if (idx == 0) {
            myStrBuffer = myStrBuffer.substr(1);
            myRread++;
            return lh.report("");
        }
        if (idx != std::string::npos) {
            toReport = myStrBuffer.substr(0, idx);
            myStrBuffer = myStrBuffer.substr(idx + 1);
            myRread += (unsigned int)idx + 1;
        } else {
            if (myRead < myAvailable) {
                myStrm.read(myBuffer,
                            myAvailable - myRead < 1024
                            ? myAvailable - myRead
                            : 1024);
                size_t noBytes = myAvailable - myRead;
                noBytes = noBytes > 1024 ? 1024 : noBytes;
                myStrBuffer += std::string(myBuffer, noBytes);
                myRead += 1024;
            } else {
                toReport = myStrBuffer;
                moreAvailable = false;
                if (toReport == "") {
                    return lh.report(toReport);
                }
            }
        }
    }
    // remove trailing blanks
    int idx = (int)toReport.length() - 1;
    while (idx >= 0 && toReport[idx] < 32) {
        idx--;
    }
    if (idx >= 0) {
        toReport = toReport.substr(0, idx + 1);
    } else {
        toReport = "";
    }
    // give it to the handler
    if (!lh.report(toReport)) {
        return false;
    }
    return moreAvailable;
}


std::string
LineReader::readLine() {
    std::string toReport;
    while (toReport.length() == 0 && myStrm.good()) {
        size_t idx = myStrBuffer.find('\n');
        if (idx == 0) {
            myStrBuffer = myStrBuffer.substr(1);
            myRread++;
            return "";
        }
        if (idx != std::string::npos) {
            toReport = myStrBuffer.substr(0, idx);
            myStrBuffer = myStrBuffer.substr(idx + 1);
            myRread += (unsigned int) idx + 1;
        } else {
            if (myRead < myAvailable) {
                myStrm.read(myBuffer,
                            myAvailable - myRead < 1024
                            ? myAvailable - myRead
                            : 1024);
                size_t noBytes = myAvailable - myRead;
                noBytes = noBytes > 1024 ? 1024 : noBytes;
                myStrBuffer += std::string(myBuffer, noBytes);
                myRead += 1024;
            } else {
                toReport = myStrBuffer;
                myRread += 1024;
                if (toReport == "") {
                    return toReport;
                }
            }
        }
    }
    if (!myStrm.good()) {
        return "";
    }
    // remove trailing blanks
    int idx = (int)toReport.length() - 1;
    while (idx >= 0 && toReport[idx] < 32) {
        idx--;
    }
    if (idx >= 0) {
        toReport = toReport.substr(0, idx + 1);
    } else {
        toReport = "";
    }
    return toReport;
}



std::string
LineReader::getFileName() const {
    return myFileName;
}


bool
LineReader::setFile(const std::string& file) {
    myFileName = file;
    reinit();
    return myStrm.good();
}


unsigned long
LineReader::getPosition() {
    return myRread;
}


void
LineReader::reinit() {
    if (myStrm.is_open()) {
        myStrm.close();
    }
    myStrm.clear();
    myStrm.open(myFileName.c_str(), std::ios::binary);
    myStrm.unsetf(std::ios::skipws);
    myStrm.seekg(0, std::ios::end);
    myAvailable = static_cast<unsigned int>(myStrm.tellg());
    myStrm.seekg(0, std::ios::beg);
    myRead = 0;
    myRread = 0;
    myStrBuffer = "";
}


void
LineReader::setPos(unsigned long pos) {
    myStrm.seekg(pos, std::ios::beg);
    myRead = pos;
    myRread = pos;
    myStrBuffer = "";
}


bool
LineReader::good() const {
    return myStrm.good();
}



/****************************************************************************/


/****************************************************************************/
/// @file    StringTokenizer.cpp
/// @author  Daniel Krajzewicz
/// @date    ?
/// @version $Id$
///
// A java-style StringTokenizer for c++ (stl)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <vector>
#include <iostream> // !!! debug only
#include "UtilExceptions.h"
#include "StringTokenizer.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// variable definitions
// ===========================================================================
const int StringTokenizer::NEWLINE = -256;
const int StringTokenizer::WHITECHARS = -257;


// ===========================================================================
// method definitions
// ===========================================================================
StringTokenizer::StringTokenizer(std::string tosplit)
        : myTosplit(tosplit), myPos(0) {
    prepareWhitechar(tosplit);
}


StringTokenizer::StringTokenizer(std::string tosplit, std::string token, bool splitAtAllChars)
        : myTosplit(tosplit), myPos(0) {
    prepare(tosplit, token, splitAtAllChars);
}


StringTokenizer::StringTokenizer(std::string tosplit, int special)
        : myTosplit(tosplit), myPos(0) {
    switch (special) {
    case NEWLINE:
        prepare(tosplit, "\r\n", true);
        break;
    case WHITECHARS:
        prepareWhitechar(tosplit);
        break;
    default:
        char *buf = new char[2];
        buf[0] = (char) special;
        buf[1] = 0;
        prepare(tosplit, buf, false);
        delete[] buf;
        break;
    }
}


StringTokenizer::~StringTokenizer() {}

void StringTokenizer::reinit() {
    myPos = 0;
}

bool StringTokenizer::hasNext() {
    return myPos!=myStarts.size();
}

std::string StringTokenizer::next() {
    if (myPos>=myStarts.size()) {
        throw OutOfBoundsException();
    }
    if (myLengths[myPos]==0) {
        myPos++;
        return "";
    }
    size_t start = myStarts[myPos];
    size_t length = myLengths[myPos++];
    return myTosplit.substr(start,length);
}

std::string StringTokenizer::front() {
    if (myStarts.size()==0) {
        throw OutOfBoundsException();
    }
    if (myLengths[0]==0) {
        return "";
    }
    return myTosplit.substr(myStarts[0],myLengths[0]);
}

std::string StringTokenizer::get(size_t pos) const {
    if (pos>=myStarts.size()) {
        throw OutOfBoundsException();
    }
    if (myLengths[pos]==0) {
        return "";
    }
    size_t start = myStarts[pos];
    size_t length = myLengths[pos];
    return myTosplit.substr(start, length);
}


size_t StringTokenizer::size() const {
    return myStarts.size();
}

void StringTokenizer::prepare(const string &tosplit, const string &token, bool splitAtAllChars) {
    size_t beg = 0;
    size_t len = token.length();
    if (splitAtAllChars) {
        len = 1;
    }
    while (beg<tosplit.length()) {
        size_t end;
        if (splitAtAllChars) {
            end = tosplit.find_first_of(token, beg);
        } else {
            end = tosplit.find(token, beg);
        }
        if (end == string::npos) {
            end = tosplit.length();
        }
        myStarts.push_back(beg);
        myLengths.push_back(end-beg);
        beg = end + len;
        if (beg==tosplit.length()) {
            myStarts.push_back(beg-1);
            myLengths.push_back(0);
        }
    }
}

void StringTokenizer::prepareWhitechar(const string &tosplit) {
    size_t len = tosplit.length();
    size_t beg = 0;
    while (beg<len&&tosplit[beg]<=32) {
        beg++;
    }
    while (beg!=string::npos&&beg<len) {
        size_t end = beg;
        while (end<len&&tosplit[end]>32) {
            end++;
        }
        myStarts.push_back(beg);
        myLengths.push_back(end-beg);
        beg = end;
        while (beg<len&&tosplit[beg]<=32) {
            beg++;
        }
    }
}

std::vector<std::string>
StringTokenizer::getVector() {
    std::vector<std::string> ret;
    ret.reserve(size());
    while (hasNext()) {
        ret.push_back(next());
    }
    reinit();
    return ret;
}



/****************************************************************************/


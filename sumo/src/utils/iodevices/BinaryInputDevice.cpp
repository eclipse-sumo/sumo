/****************************************************************************/
/// @file    BinaryInputDevice.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2005-09-15
/// @version $Id$
///
// Encapsulates binary reading operations on a file
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include "BinaryFormatter.h"
#include "BinaryInputDevice.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// constants definitions
// ===========================================================================
#define BUF_MAX 1000


// ===========================================================================
// method definitions
// ===========================================================================
BinaryInputDevice::BinaryInputDevice(const std::string& name,
                                     const bool isTyped, const bool doValidate)
    : myStream(name.c_str(), std::fstream::in | std::fstream::binary),
      myAmTyped(isTyped), myEnableValidation(doValidate) {}


BinaryInputDevice::~BinaryInputDevice() {}


bool
BinaryInputDevice::good() const {
    return myStream.good();
}


int
BinaryInputDevice::peek() {
    return myStream.peek();
}


void
BinaryInputDevice::checkType(BinaryFormatter::DataType t) {
    if (myAmTyped) {
        char c;
        myStream.read(&c, sizeof(char));
        if (myEnableValidation && c != t) {
            throw ProcessError("Unexpected type.");
        }
    }
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, char& c) {
    os.checkType(BinaryFormatter::BF_BYTE);
    os.myStream.read(&c, sizeof(char));
    return os;
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, int& i) {
    os.checkType(BinaryFormatter::BF_INTEGER);
    os.myStream.read((char*) &i, sizeof(int));
    return os;
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, unsigned int& i) {
    os.checkType(BinaryFormatter::BF_INTEGER);
    os.myStream.read((char*) &i, sizeof(unsigned int));
    return os;
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, SUMOReal& f) {
    os.checkType(BinaryFormatter::BF_FLOAT);
    os.myStream.read((char*) &f, sizeof(SUMOReal));
    return os;
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, bool& b) {
    os.checkType(BinaryFormatter::BF_BYTE);
    b = false;
    os.myStream.read((char*) &b, sizeof(char));
    return os;
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, std::string& s) {
    os.checkType(BinaryFormatter::BF_STRING);
    unsigned int size;
    os.myStream.read((char*) &size, sizeof(unsigned int));
    if (size < BUF_MAX) {
        os.myStream.read((char*) &os.myBuffer, sizeof(char)*size);
        os.myBuffer[size] = 0;
        s = std::string(os.myBuffer);
    }
    return os;
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, std::vector<std::string>& v) {
    os.checkType(BinaryFormatter::BF_LIST);
    unsigned int size;
    os.myStream.read((char*) &size, sizeof(unsigned int));
    while (size > 0) {
        std::string s;
        os >> s;
        v.push_back(s);
        size--;
    }
    return os;
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, std::vector<unsigned int>& v) {
    os.checkType(BinaryFormatter::BF_LIST);
    unsigned int size;
    os.myStream.read((char*) &size, sizeof(unsigned int));
    while (size > 0) {
        unsigned int i;
        os >> i;
        v.push_back(i);
        size--;
    }
    return os;
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, std::vector< std::vector<unsigned int> >& v) {
    os.checkType(BinaryFormatter::BF_LIST);
    unsigned int size;
    os.myStream.read((char*) &size, sizeof(unsigned int));
    while (size > 0) {
        std::vector<unsigned int> nested;
        os >> nested;
        v.push_back(nested);
        size--;
    }
    return os;
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, long& l) {
    os.checkType(BinaryFormatter::BF_INTEGER);
    os.myStream.read((char*) &l, sizeof(long));
    return os;
}



/****************************************************************************/

/****************************************************************************/
/// @file    BinaryInputDevice.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2005-09-15
/// @version $Id$
///
// Encapsulates binary reading operations on a file
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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
#include <utils/common/StdDefs.h>
#include <utils/geom/Position.h>
#include "BinaryFormatter.h"
#include "BinaryInputDevice.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// constants definitions
// ===========================================================================
#define BUF_MAX 10000


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


std::string
BinaryInputDevice::read(int numBytes) {
    myStream.read((char*) &myBuffer, sizeof(char)*numBytes);
    return std::string(myBuffer, numBytes);
}


void
BinaryInputDevice::putback(char c) {
    myStream.putback(c);
}


int
BinaryInputDevice::checkType(BinaryFormatter::DataType t) {
    if (myAmTyped) {
        char c;
        myStream.read(&c, sizeof(char));
        if (myEnableValidation && c != t) {
            throw ProcessError("Unexpected type.");
        }
        return c;
    }
    return -1;
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, char& c) {
    os.checkType(BinaryFormatter::BF_BYTE);
    os.myStream.read(&c, sizeof(char));
    return os;
}


BinaryInputDevice&
operator>>(BinaryInputDevice& os, unsigned char& c) {
    os.checkType(BinaryFormatter::BF_BYTE);
    os.myStream.read((char*) &c, sizeof(unsigned char));
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
    int t = os.checkType(BinaryFormatter::BF_FLOAT);
    if (t == BinaryFormatter::BF_SCALED2INT) {
        int v;
        os.myStream.read((char*) &v, sizeof(int));
        f = v / 100.;
    } else {
        os.myStream.read((char*) &f, sizeof(SUMOReal));
    }
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
    unsigned int done = 0;
    while (done < size) {
        const unsigned int toRead = MIN2((unsigned int)size - done, (unsigned int)BUF_MAX - 1);
        os.myStream.read((char*) &os.myBuffer, sizeof(char)*toRead);
        os.myBuffer[toRead] = 0;
        s += std::string(os.myBuffer);
        done += toRead;
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
operator>>(BinaryInputDevice& os, Position& p) {
    int t = os.checkType(BinaryFormatter::BF_POSITION_2D);
    SUMOReal x, y, z = 0;
    if (t == BinaryFormatter::BF_SCALED2INT_POSITION_2D || t == BinaryFormatter::BF_SCALED2INT_POSITION_2D) {
        int v;
        os.myStream.read((char*) &v, sizeof(int));
        x = v / 100.;
        os.myStream.read((char*) &v, sizeof(int));
        y = v / 100.;
        if (t == BinaryFormatter::BF_SCALED2INT_POSITION_3D) {
            os.myStream.read((char*) &v, sizeof(int));
            z = v / 100.;
        }
    } else {
        os.myStream.read((char*) &x, sizeof(SUMOReal));
        os.myStream.read((char*) &y, sizeof(SUMOReal));
        if (t == BinaryFormatter::BF_POSITION_3D) {
            os.myStream.read((char*) &z, sizeof(SUMOReal));
        }
    }
    p.set(x, y, z);
    return os;
}



/****************************************************************************/

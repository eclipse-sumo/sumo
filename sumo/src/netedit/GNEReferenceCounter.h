/****************************************************************************/
/// @file    GNEReferenceCounter.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A class that counts references to itself
// We may wish to keep references to junctions/nodes either in the network or in the undoList
// to clean up properly we have to resort to reference counting
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEReferenceCounter_h
#define GNEReferenceCounter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>


//#define _DEBUG_REFERENCECOUNTER

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class  GNEReferenceCounter
 */
class GNEReferenceCounter {
public:
    //// @brief constructor
    GNEReferenceCounter(): myCount(0) {}

    //// @brief destructor
    ~GNEReferenceCounter() {
        if (myCount) {
            // cannot print id here, it already got destructed
            throw ProcessError("Attempt to delete instance of GNEReferenceCounter with count " + toString(myCount));
        }
    }


    void decRef(const std::string& debugMsg = "") {
#ifdef _DEBUG_REFERENCECOUNTER
        std::cout << "decRef (" + toString(myCount) + ") for " + getID() + ": " << debugMsg << "\n";
#endif
        UNUSED_PARAMETER(debugMsg); // only used for print debugging
        if (myCount < 1) {
            throw ProcessError("Attempt to decrement references below zero for instance of GNEReferenceCounter");
        }
        myCount--;
    }


    void incRef(const std::string& debugMsg = "") {
#ifdef _DEBUG_REFERENCECOUNTER
        std::cout << "incRef (" + toString(myCount) + ") for " + getID() + ": " << debugMsg << "\n";
#endif
        UNUSED_PARAMETER(debugMsg); // only used for print debugging
        myCount++;
    }


    bool unreferenced() {
        return myCount == 0;
    }


    virtual const std::string getID() const = 0;


private:
    int myCount;

};


#endif

/****************************************************************************/


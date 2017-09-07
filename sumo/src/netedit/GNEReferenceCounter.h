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

#include <utils/common/MsgHandler.h>
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
        // If myCount is different of 0, means that references weren't removed correctly
        if (myCount != 0) {
            // cannot print id here, it already got destructed
            WRITE_ERROR("Attempt to delete instance of GNEReferenceCounter with count " + toString(myCount));
        }
    }

    /// @brief Decrease reference
    void decRef(const std::string& debugMsg = "") {
        // debugMsg only used for print debugging
#ifdef _DEBUG_REFERENCECOUNTER
        std::cout << "decRef (" + toString(myCount) + ") for " + getID() + ": " << debugMsg << "\n";
#else
        UNUSED_PARAMETER(debugMsg);
#endif
        // write error if decrement results into a negative count
        if (myCount < 1) {
            WRITE_ERROR("Attempt to decrement references below zero for instance of GNEReferenceCounter");
        }
        myCount--;
    }

    /// @brief Increarse reference
    void incRef(const std::string& debugMsg = "") {
        // debugMsg only used for print debugging
#ifdef _DEBUG_REFERENCECOUNTER
        std::cout << "incRef (" + toString(myCount) + ") for " + getID() + ": " << debugMsg << "\n";
#else
        UNUSED_PARAMETER(debugMsg);
#endif
        myCount++;
    }

    /// @brief check if object ins't referenced
    bool unreferenced() {
        return myCount == 0;
    }

    /// @brief return ID of object
    virtual const std::string getID() const = 0;


private:
    /// @brief reference counter
    int myCount;

};


#endif

/****************************************************************************/


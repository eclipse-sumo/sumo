/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNEDottedContourThread.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2019
///
// The thread that performs the calculation of contours around shapes
/****************************************************************************/
#ifndef GNEDottedContourThread_h
#define GNEDottedContourThread_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDottedContourThread
 */
class GNEDottedContourThread : protected FXThread {

public:
    /// @brief constructor
    GNEDottedContourThread(GNEViewNet *viewNet);

    /// @brief destructor
    ~GNEDottedContourThread();

    /// @brief starts the thread
    FXint run();

protected:
    /// @brief pointer to ViewNet
    const GNEViewNet *myViewNet;

private:
    /// @brief Invalidated copy constructor.
    GNEDottedContourThread(const GNEDottedContourThread&) = delete;

    /// @brief Invalidated assignment operator
    GNEDottedContourThread& operator=(const GNEDottedContourThread& src) = delete;
};


#endif

/****************************************************************************/

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
#include <queue>
#include <mutex>
#include <set>

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNENetElement;
class GNEAdditional;
class GNEShape;
class GNEDemandElement;
class GUIVisualizationSettings;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDottedContourThread
 */
class GNEDottedContourThread : protected FXThread {

public:
    /// @brief constructor
    GNEDottedContourThread(GNENet *net);

    /// @brief destructor
    ~GNEDottedContourThread();

    /// @brief set visualization settings and start thread
    void setVisualizationSettings(GUIVisualizationSettings* s);

    /// @brief add a net element into queue to update dotted contour
    void updateNetElementDottedContour(GNENetElement *netElement);

protected:
    /// @brief starts the thread
    FXint run();

    /// @brief pointer to current net
    const GNENet *myNet;

    /// @brief visualization settings
    GUIVisualizationSettings* myVisualizationSetting;

    /// @brief mutex
    std::mutex myMutex;

    /// @brief queue for net elements
    std::set<GNENetElement*> myNetElements;

    /// @brief queue for additionals
    std::queue<GNEAdditional*> myAdditionals;

    /// @brief queue for shapes
    std::queue<GNEShape*> myShapes;

    /// @brief queue for demand elements
    std::queue<GNEDemandElement*> myDemandElements;

    /// @brief lock netElements queue
    bool myLockNetElementsQueue;

    // @brief lock additionals queue
    bool myLockAdditionalsQueue;

    // @brief lock shapes queue
    bool myLockShapeQueue;

    // @brief lock demand elements queue
    bool myLockDemandElementsQueue;

private:
    /// @brief calculate junction dotted contour
    void calculateJunctionDottedContour(GNENetElement* junction);

    /// @brief calculate edge dotted contour
    void calculateEdgeDottedContour(GNENetElement* edge);

    /// @brief Invalidated copy constructor.
    GNEDottedContourThread(const GNEDottedContourThread&) = delete;

    /// @brief Invalidated assignment operator
    GNEDottedContourThread& operator=(const GNEDottedContourThread& src) = delete;
};


#endif

/****************************************************************************/

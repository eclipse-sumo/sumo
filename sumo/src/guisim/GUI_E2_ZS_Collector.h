/****************************************************************************/
/// @file    GUI_E2_ZS_Collector.h
/// @author  Daniel Krajzewicz
/// @date    Okt 2003
/// @version $Id$
///
// The gui-version of the MS_E2_ZS_Collector, together with the according
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUI_E2_ZS_Collector_h
#define GUI_E2_ZS_Collector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/PhysicalTypeDefs.h>
#include <microsim/output/MSE2Collector.h>
#include <microsim/MSNet.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/VectorHelper.h>
#include <utils/common/ValueSource.h>
#include "GUIDetectorWrapper.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObjectStorage;
class GUILaneWrapper;
class GUI_E2_ZS_CollectorOverLanes;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUI_E2_ZS_Collector
 * The gui-version of the MS_E2_ZS_Collector.
 * Allows the building of a wrapper (also declared herein) which draws the
 * detector on the gl-canvas. Beside this, the method "amVisible" is
 * overridden to signalise that this detector is not used for simulation-
 * -internal reasons, but is placed over the simulation by the user.
 */
class GUI_E2_ZS_Collector : public MSE2Collector
{
public:
    /// Constructor
    GUI_E2_ZS_Collector(const std::string &id, DetectorUsage usage,
                        MSLane* lane, SUMOReal startPos, SUMOReal detLength,
                        SUMOTime haltingTimeThreshold, SUMOReal haltingSpeedThreshold,
                        SUMOReal jamDistThreshold) throw();

    /// Destructor
    ~GUI_E2_ZS_Collector() throw();

    // valid for gui-version only
    virtual GUIDetectorWrapper *buildDetectorWrapper(
        GUIGlObjectStorage &idStorage,
        GUILaneWrapper &wrapper);

    // valid for gui-version and joined collectors only
    virtual GUIDetectorWrapper *buildDetectorWrapper(
        GUIGlObjectStorage &idStorage,
        GUILaneWrapper &wrapper, GUI_E2_ZS_CollectorOverLanes& p,
        size_t glID);

public:
    /**
     * @class GUI_E2_ZS_Collector::MyWrapper
     * A GUI_E2_ZS_Collector-visualiser
     */
class MyWrapper : public GUIDetectorWrapper
    {
    public:
        /// Constructor
        MyWrapper(GUI_E2_ZS_Collector &detector,
                  GUIGlObjectStorage &idStorage, GUILaneWrapper &wrapper) throw();

        /// Constructor for collectors joined over lanes
        MyWrapper(GUI_E2_ZS_Collector &detector,
                  GUIGlObjectStorage &idStorage, size_t glID,
                  GUI_E2_ZS_CollectorOverLanes &mustBe,
                  GUILaneWrapper &wrapper) throw();

        /// Destrutor
        ~MyWrapper() throw();

        /** @brief Returns the boundary to which the view shall be centered in order to show the object
         *
         * @return The boundary the object is within
         * @see GUIGlObject::getCenteringBoundary
         */
        Boundary getCenteringBoundary() const throw();


        /** @brief Draws the object
         *
         * @param[in] scale The current scale (meters to pixel) of the view
         * @param[in] upscale The factor by which the object's size shall be multiplied
         * @see GUIGlObject_AbstractAdd::drawGL
         */
        void drawGL(SUMOReal scale, SUMOReal upscale) throw();

        /// Draws the detector in full-geometry mode
        GUIParameterTableWindow *getParameterWindow(
            GUIMainWindow &app, GUISUMOAbstractView &parent) throw();

        /// returns the id of the object as known to microsim
        const std::string &microsimID() const throw();


        /// Returns the detector itself
        GUI_E2_ZS_Collector &getDetector();


    private:
        void myConstruct(GUI_E2_ZS_Collector &detector,
                         GUILaneWrapper &wrapper);

    private:
        /// The wrapped detector
        GUI_E2_ZS_Collector &myDetector;

        /// The detector's boundary
        Boundary myBoundary;

        /// A sequence of positions in full-geometry mode
        Position2DVector myFullGeometry;

        /// A sequence of lengths in full-geometry mode
        DoubleVector myShapeLengths;

        /// A sequence of rotations in full-geometry mode
        DoubleVector myShapeRotations;

    private:
        /// @brief Invalidated copy constructor.
        MyWrapper(const MyWrapper&);

        /// @brief Invalidated assignment operator.
        MyWrapper& operator=(const MyWrapper&);

    };

};


#endif

/****************************************************************************/


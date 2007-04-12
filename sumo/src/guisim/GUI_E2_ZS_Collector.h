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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/output/e2_detectors/MSE2Collector.h>
#include <microsim/MSNet.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/DoubleVector.h>
#include <utils/helpers/ValueSource.h>
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
    GUI_E2_ZS_Collector(std::string id, DetectorUsage usage, MSLane* lane,
                        SUMOReal startPos, SUMOReal detLength,
                        SUMOReal haltingTimeThreshold,
                        MSUnit::MetersPerSecond haltingSpeedThreshold,
                        SUMOReal jamDistThreshold,
                        SUMOTime deleteDataAfterSeconds);

    /// Destructor
    ~GUI_E2_ZS_Collector();

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
                  GUIGlObjectStorage &idStorage, GUILaneWrapper &wrapper);

        /// Constructor for collectors joined over lanes
        MyWrapper(GUI_E2_ZS_Collector &detector,
                  GUIGlObjectStorage &idStorage, size_t glID,
                  GUI_E2_ZS_CollectorOverLanes &mustBe,
                  GUILaneWrapper &wrapper);

        /// Destrutor
        ~MyWrapper();

        /// Returns the boundary of the wrapped detector
        Boundary getBoundary() const;

        /// Draws the detector in full-geometry mode
        void drawGL(SUMOReal scale, SUMOReal upscale);

        /// Draws the detector in full-geometry mode
        GUIParameterTableWindow *getParameterWindow(
            GUIMainWindow &app, GUISUMOAbstractView &parent);

        /// returns the id of the object as known to microsim
        const std::string &microsimID() const;

        /// Returns the information whether this detector is still active
        bool active() const;

        /// Returns the wrapped detector's coordinates
        Position2D getPosition() const;

        /// Returns the detector itself
        GUI_E2_ZS_Collector &getDetector();

    protected:
        /// Builds a view within the parameter table if the according type is available
        void myMkExistingItem(GUIParameterTableWindow &ret,
                              const std::string &name, E2::DetType type);

    private:
        void myConstruct(GUI_E2_ZS_Collector &detector,
                         GUILaneWrapper &wrapper);

    private:
        /// The wrapped detector
        GUI_E2_ZS_Collector &myDetector;

        /// The detector's boundary //!!!what about SG/FG
        Boundary myBoundary;

        /// A sequence of positions in full-geometry mode
        Position2DVector myFullGeometry;

        /// A sequence of lengths in full-geometry mode
        DoubleVector myShapeLengths;

        /// A sequence of rotations in full-geometry mode
        DoubleVector myShapeRotations;

        /**
         * @class GUI_E2_ZS_Collector::MyWrapper::ValueRetriever
         * This class realises the retrieval of a certain value
         * with a certain interval specification from the detector
         */
    class MyValueRetriever : public ValueSource<SUMOReal>
        {
        public:
            /// Constructor
            MyValueRetriever(GUI_E2_ZS_Collector &det,
                             E2::DetType type, size_t nSec)
                    : myDetector(det), myType(type), myNSec(nSec)
            { }

            /// Destructor
            ~MyValueRetriever()
            { }

            /// Returns the current value
            SUMOReal getValue() const
            {
                return myDetector.getAggregate(myType, myNSec);
            }

            /// Returns a copy of this instance
            ValueSource<SUMOReal> *copy() const
            {
                return new MyValueRetriever(myDetector, myType, myNSec);
            }

        private:
            /// The detctor to get the value from
            GUI_E2_ZS_Collector &myDetector;

            /// The type of the value to retrieve
            E2::DetType myType;

            /// The aggregation interval
            size_t myNSec;
        };

    };

};


#endif

/****************************************************************************/


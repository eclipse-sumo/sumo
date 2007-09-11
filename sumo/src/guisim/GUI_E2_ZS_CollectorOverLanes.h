/****************************************************************************/
/// @file    GUI_E2_ZS_CollectorOverLanes.h
/// @author  Daniel Krajzewicz
/// @date    Okt 2003
/// @version $Id$
///
// The gui-version of the MS_E2_ZS_CollectorOverLanes, together with the according
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
#ifndef GUI_E2_ZS_CollectorOverLanes_h
#define GUI_E2_ZS_CollectorOverLanes_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/output/e2_detectors/MS_E2_ZS_CollectorOverLanes.h>
#include <microsim/MSNet.h>
#include <microsim/MSUnit.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/VectorHelper.h>
#include <utils/helpers/ValueSource.h>
#include "GUI_E2_ZS_Collector.h"
#include "GUIDetectorWrapper.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObjectStorage;
class GUILaneWrapper;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUI_E2_ZS_CollectorOverLanes
 * The gui-version of the MS_E2_ZS_CollectorOverLanes.
 * Allows the building of a wrapper (also declared herein) which draws the
 * detector on the gl-canvas. Beside this, the method "amVisible" is
 * overridden to signalise that this detector is not used for simulation-
 * -internal reasons, but is placed over the simulation by the user.
 */
class GUI_E2_ZS_CollectorOverLanes : public MS_E2_ZS_CollectorOverLanes
{
public:
    typedef std::vector<GUI_E2_ZS_Collector*> CollectorVector;

    /// Constructor
    GUI_E2_ZS_CollectorOverLanes(std::string id, DetectorUsage usage,
                                 MSLane* lane, SUMOReal startPos,
                                 SUMOReal haltingTimeThreshold,
                                 MSUnit::MetersPerSecond haltingSpeedThreshold,
                                 SUMOReal jamDistThreshold,
                                 SUMOTime deleteDataAfterSeconds);

    /// Destructor
    ~GUI_E2_ZS_CollectorOverLanes();

    // invalid in fact, as collectors do not need a lane
    virtual GUIDetectorWrapper *buildDetectorWrapper(
        GUIGlObjectStorage &idStorage,
        GUILaneWrapper &lane);

    // valid for gui-version only
    virtual GUIDetectorWrapper *buildDetectorWrapper(
        GUIGlObjectStorage &idStorage);

protected:
    MSE2Collector *buildCollector(size_t c, size_t r,
                                  MSLane *l, SUMOReal start, SUMOReal end);


public:
    /**
     * @class GUI_E2_ZS_CollectorOverLanes::MyWrapper
     * A GUI_E2_ZS_CollectorOverLanes-visualiser
     */
class MyWrapper : public GUIDetectorWrapper
    {
    public:
        /// Constructor
        MyWrapper(GUI_E2_ZS_CollectorOverLanes &detector,
                  GUIGlObjectStorage &idStorage,
                  const LaneDetMap &detectors);

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

        /// Needed to set the id
        friend class GUIGlObjectStorage;

        /// Returns the information whether this detector is still active
        bool active() const;

        /// Returns the wrapped detector's coordinates
        Position2D getPosition() const;

        /// Returns the detector itself
        GUI_E2_ZS_CollectorOverLanes &getLoop();

    protected:
        /// Builds a view within the parameter table if the according type is available
        void myMkExistingItem(GUIParameterTableWindow &ret,
                              const std::string &name, E2::DetType type);

    private:
        /// The wrapped detector
        GUI_E2_ZS_CollectorOverLanes &myDetector;

        /// The detector's boundary //!!!what about SG/FG
        Boundary myBoundary;

        std::vector<GUIDetectorWrapper*> mySubWrappers;

        /**
         * @class GUI_E2_ZS_CollectorOverLanes::MyWrapper::ValueRetriever
         * This class realises the retrieval of a certain value
         * with a certain interval specification from the detector
         */
    class MyValueRetriever : public ValueSource<SUMOReal>
        {
        public:
            /// Constructor
            MyValueRetriever(GUI_E2_ZS_CollectorOverLanes &det,
                             E2::DetType type, MSUnit::Seconds nSec)
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
            GUI_E2_ZS_CollectorOverLanes &myDetector;

            /// The type of the value to retrieve
            E2::DetType myType;

            /// The aggregation interval
            MSUnit::Seconds myNSec;
        };

    };

};


#endif

/****************************************************************************/


#ifndef GUI_E2_ZS_CollectorOverLanes_h
#define GUI_E2_ZS_CollectorOverLanes_h
//---------------------------------------------------------------------------//
//                        GUI_E2_ZS_CollectorOverLanes.h -
//  The gui-version of the MS_E2_ZS_CollectorOverLanes, together with the according
//   wrapper
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Okt 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.7  2004/11/24 08:46:42  dkrajzew
// recent changes applied
//
// Revision 1.6  2004/07/02 08:40:42  dkrajzew
// changes in the detector drawer applied
//
// Revision 1.5  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.4  2004/01/26 06:59:38  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics; different detector visualistaion in dependence to his controller
//
// Revision 1.3  2003/12/04 13:31:28  dkrajzew
// detector name changes applied
//
// Revision 1.2  2003/11/18 14:27:39  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.1  2003/11/17 07:15:27  dkrajzew
// e2-detector over lanes merger added
//
// Revision 1.4  2003/11/12 14:00:19  dkrajzew
// commets added; added parameter windows to all detectors
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <microsim/output/e2_detectors/MS_E2_ZS_CollectorOverLanes.h>
#include <microsim/MSNet.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/DoubleVector.h>
#include <helpers/ValueSource.h>
#include "GUI_E2_ZS_Collector.h"
#include "GUIDetectorWrapper.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObjectStorage;
class GUILaneWrapper;


/* =========================================================================
 * class definitions
 * ======================================================================= */
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
    GUI_E2_ZS_CollectorOverLanes( std::string id, DetectorUsage usage,
        MSLane* lane, MSUnit::Meters startPos,
        MSUnit::Seconds haltingTimeThreshold = 1,
        MSUnit::MetersPerSecond haltingSpeedThreshold =5.0/3.6,
        MSUnit::Meters jamDistThreshold = 10,
        MSUnit::Seconds deleteDataAfterSeconds = 1800 );

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
        MSLane *l, double start, double end);


public:
    /**
     * @class GUI_E2_ZS_CollectorOverLanes::MyWrapper
     * A GUI_E2_ZS_CollectorOverLanes-visualiser
     */
    class MyWrapper : public GUIDetectorWrapper {
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
        void drawGL_FG(double scale);

        /// Draws the detector in simple-geometry mode
        void drawGL_SG(double scale);

        /// Draws the detector in full-geometry mode
        GUIParameterTableWindow *getParameterWindow(
            GUIMainWindow &app, GUISUMOAbstractView &parent);

        /// returns the id of the object as known to microsim
        std::string microsimID() const;

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
        class MyValueRetriever : public ValueSource<double> {
        public:
            /// Constructor
            MyValueRetriever(GUI_E2_ZS_CollectorOverLanes &det,
                E2::DetType type, size_t nSec)
                : myDetector(det), myType(type), myNSec(nSec) { }

            /// Destructor
            ~MyValueRetriever() { }

            /// Returns the current value
            double getValue() const
                {
                    return myDetector.getAggregate(myType, myNSec);
                }

            /// Returns a copy of this instance
            ValueSource<double> *copy() const {
                return new MyValueRetriever(myDetector, myType, myNSec);
            }

        private:
            /// The detctor to get the value from
            GUI_E2_ZS_CollectorOverLanes &myDetector;

            /// The type of the value to retrieve
            E2::DetType myType;

            /// The aggregation interval
            size_t myNSec;
        };

    };

};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

#endif

// Local Variables:
// mode:C++
// End:



#ifndef GUI_E2_ZS_Collector_h
#define GUI_E2_ZS_Collector_h
//---------------------------------------------------------------------------//
//                        GUI_E2_ZS_Collector.h -
//  The gui-version of the MS_E2_ZS_Collector, together with the according
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
// Revision 1.5  2003/11/18 14:27:39  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.4  2003/11/12 14:00:19  dkrajzew
// commets added; added parameter windows to all detectors
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <microsim/MS_E2_ZS_Collector.h>
#include <microsim/MSNet.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>
#include <utils/common/DoubleVector.h>
#include <helpers/ValueSource.h>
#include "GUIDetectorWrapper.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObjectStorage;
class GUILaneWrapper;
class GUI_E2_ZS_CollectorOverLanes;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUI_E2_ZS_Collector
 * The gui-version of the MS_E2_ZS_Collector.
 * Allows the building of a wrapper (also declared herein) which draws the
 * detector on the gl-canvas. Beside this, the method "amVisible" is
 * overridden to signalise that this detector is not used for simulation-
 * -internal reasons, but is placed over the simulation by the user.
 */
class GUI_E2_ZS_Collector : public MS_E2_ZS_Collector
{
public:
    /// Constructor
    GUI_E2_ZS_Collector(std::string id, //daraus ergibt sich der Filename
                        MSLane* lane,
                        MSUnit::Meters startPos,
                        MSUnit::Meters detLength,
                        bool visible = false,
                        MSUnit::Seconds haltingTimeThreshold = 1,
                        MSUnit::MetersPerSecond haltingSpeedThreshold =5.0/3.6,
                        MSUnit::Meters jamDistThreshold = 10,
                        MSUnit::Seconds deleteDataAfterSeconds = 1800 );

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

    /// This detector shall be shown
    bool amVisible() const {
        return myAmVisble;
    }

public:
    /**
     * @class GUI_E2_ZS_Collector::MyWrapper
     * A GUI_E2_ZS_Collector-visualiser
     */
    class MyWrapper : public GUIDetectorWrapper {
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

        /// Returns the boundery of the wrapped detector
        Boundery getBoundery() const;

        /// Draws the detector in full-geometry mode
        void drawGL_FG(double scale) const;

        /// Draws the detector in simple-geometry mode
        void drawGL_SG(double scale) const;

        /// Draws the detector in full-geometry mode
        GUIParameterTableWindow *getParameterWindow(
            GUIApplicationWindow &app, GUISUMOAbstractView &parent);

        /// returns the id of the object as known to microsim
        std::string microsimID() const;

        /// Needed to set the id
        friend class GUIGlObjectStorage;

        /// Returns the information whether this detector is still active
        bool active() const;

        /// Returns the wrapped detector's coordinates
        Position2D getPosition() const;

        /// Returns the detector itself
        GUI_E2_ZS_Collector &getLoop();

    protected:
        /// Builds a view within the parameter table if the according type is available
        void myMkExistingItem(GUIParameterTableWindow &ret,
            const std::string &name, MS_E2_ZS_Collector::DetType type);

    private:
        void myConstruct(GUI_E2_ZS_Collector &detector,
            GUILaneWrapper &wrapper);

    private:
        /// The wrapped detector
        GUI_E2_ZS_Collector &myDetector;

        /// The detector's boundery //!!!what about SG/FG
        Boundery myBoundery;

        /// The position in simple-geometry mode
        Position2D mySGPosition;

        /// The rotation in simple-geometry mode
        double mySGRotation;

        /// The lengt in simple-geometry mode
		double mySGLength;

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
        class MyValueRetriever : public ValueSource<double> {
        public:
            /// Constructor
            MyValueRetriever(GUI_E2_ZS_Collector &det,
                MS_E2_ZS_Collector::DetType type, size_t nSec)
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
            GUI_E2_ZS_Collector &myDetector;

            /// The type of the value to retrieve
            MS_E2_ZS_Collector::DetType myType;

            /// The aggregation interval
            size_t myNSec;
        };

    };

private:
    /// The information whether the detector itself shall be visible
    bool myAmVisble;
};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

//#ifndef DISABLE_INLINE
//#include "GUI_E2_ZS_Collector.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:



#ifndef GUIE3Collector_h
#define GUIE3Collector_h
//---------------------------------------------------------------------------//
//                        GUIE3Collector.h -
//  The gui-version of the MSE3Collector, together with the according
//   wrapper
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jan 2004
//  copyright            : (C) 2004 by Daniel Krajzewicz
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
// Revision 1.5  2004/07/02 08:40:42  dkrajzew
// changes in the detector drawer applied
//
// Revision 1.4  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.3  2004/02/10 07:07:13  dkrajzew
// debugging of network loading after a network failed to be loaded; memory leaks removal
//
// Revision 1.2  2004/02/05 16:30:59  dkrajzew
// multiplicate deletion of E3-detectors on application quit patched
//
// Revision 1.1  2004/01/26 06:59:37  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics; different detector visualistaion in dependence to his controller
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <vector>
#include <microsim/MSE3Collector.h>
#include "GUIDetectorWrapper.h"
#include <utils/geom/Position2DVector.h>
#include <helpers/ValueSource.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIE3Collector
 * The gui-version of the MSE3Collector.
 * Allows the building of a wrapper (also declared herein) which draws the
 * detector on the gl-canvas.
 */
class GUIE3Collector : public MSE3Collector {
public:
    /// Constructor
    GUIE3Collector(std::string id, Detector::CrossSections entries,
        Detector::CrossSections exits,
        MSUnit::Seconds haltingTimeThreshold = 1,
        MSUnit::MetersPerSecond haltingSpeedThreshold = 5.0 / 3.6,
        MSUnit::Seconds deleteDataAfterSeconds = 1800);

    /// Destructor
    ~GUIE3Collector();

    /// Returns the list of entry points
    const Detector::CrossSections &getEntries() const;

    /// Returns the list of exit points
    const Detector::CrossSections &getExits() const;

    /// Builds the wrapper
    GUIDetectorWrapper *buildDetectorWrapper(GUIGlObjectStorage &idStorage);

public:
    /// Adds a build detector to an internal list
    static void addBuild(GUIE3Collector *det);

    /// Clears the list of instances
    static void clearInstances();

    /// The list of build detectors
    typedef std::vector<GUIE3Collector*> InstanceVector;

    /// Returns a list of known e3-instances
    static const InstanceVector &getInstances();


    /**
     * @class GUIE3Collector::MyWrapper
     * A GUIE3Collector-visualiser
     */
    class MyWrapper : public GUIDetectorWrapper {
    public:
        /// Constructor
        MyWrapper(GUIE3Collector &detector,
            GUIGlObjectStorage &idStorage);

        /// Destrutor
        ~MyWrapper();

        /// Returns the boundery of the wrapped detector
        Boundery getBoundery() const;

        /// Draws the detector in full-geometry mode
        void drawGL_FG(double scale);

        /// Draws the detector in simple-geometry mode
        void drawGL_SG(double scale);

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
        GUIE3Collector &getDetector();

    public:
        struct SingleCrossingDefinition {
            /// The position in simple-geometry mode
            Position2D mySGPosition;
            /// The rotation in simple-geometry mode
            double mySGRotation;
            /// The position in full-geometry mode
            Position2D myFGPosition;
            /// The rotation in full-geometry mode
            double myFGRotation;
        };

    protected:
        /// Builds a view within the parameter table if the according type is available
        void myMkExistingItem(GUIParameterTableWindow &ret,
            const std::string &name, E3::DetType type);

        /// Builds the description about the position of the entry/exit point
        SingleCrossingDefinition buildDefinition(const MSCrossSection &section,
            bool exit);

        /// Draws a single entry/exit point
        void drawSingleCrossing(const Position2D &pos, double rot) const;

    private:
        /// The wrapped detector
        GUIE3Collector &myDetector;

        /// The detector's boundery //!!!what about SG/FG
        Boundery myBoundery;

        /// Definition of a list of cross (entry/exit-point) positions
        typedef std::vector<SingleCrossingDefinition> CrossingDefinitions;

        /// The list of entry positions
        CrossingDefinitions myEntryDefinitions;

        /// The list of exit positions
        CrossingDefinitions myExitDefinitions;

        /**
         * @class GUI_E2_ZS_Collector::MyWrapper::ValueRetriever
         * This class realises the retrieval of a certain value
         * with a certain interval specification from the detector
         */
        class MyValueRetriever : public ValueSource<double> {
        public:
            /// Constructor
            MyValueRetriever(GUIE3Collector &det,
                E3::DetType type, size_t nSec)
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
            GUIE3Collector &myDetector;

            /// The type of the value to retrieve
            E3::DetType myType;

            /// The aggregation interval
            size_t myNSec;
        };

    };

private:
    /// The list of e3-detectors known within the loaded simulation
    static InstanceVector myInstances;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

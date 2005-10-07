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
// Revision 1.11  2005/10/07 11:37:17  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/22 13:39:35  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 11:06:37  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/05/04 07:59:59  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2004/12/16 12:14:59  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.6  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include <vector>
#include <microsim/output/e3_detectors/MSE3Collector.h>
#include "GUIDetectorWrapper.h"
#include <utils/geom/Position2DVector.h>
#include <utils/helpers/ValueSource.h>


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
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        SUMOTime deleteDataAfterSeconds);

    /// Destructor
    ~GUIE3Collector();

    /// Returns the list of entry points
    const Detector::CrossSections &getEntries() const;

    /// Returns the list of exit points
    const Detector::CrossSections &getExits() const;

    /// Builds the wrapper
    GUIDetectorWrapper *buildDetectorWrapper(GUIGlObjectStorage &idStorage);

public:
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

        /// Returns the boundary of the wrapped detector
        Boundary getBoundary() const;

        /// Draws the detector in full-geometry mode
        void drawGL_FG(SUMOReal scale);

        /// Draws the detector in simple-geometry mode
        void drawGL_SG(SUMOReal scale);

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
        GUIE3Collector &getDetector();

    public:
        struct SingleCrossingDefinition {
            /// The position in simple-geometry mode
            Position2D mySGPosition;
            /// The rotation in simple-geometry mode
            SUMOReal mySGRotation;
            /// The position in full-geometry mode
            Position2D myFGPosition;
            /// The rotation in full-geometry mode
            SUMOReal myFGRotation;
        };

    protected:
        /// Builds a view within the parameter table if the according type is available
        void myMkExistingItem(GUIParameterTableWindow &ret,
            const std::string &name, E3::DetType type);

        /// Builds the description about the position of the entry/exit point
        SingleCrossingDefinition buildDefinition(const MSCrossSection &section,
            bool exit);

        /// Draws a single entry/exit point
        void drawSingleCrossing(const Position2D &pos, SUMOReal rot) const;

    private:
        /// The wrapped detector
        GUIE3Collector &myDetector;

        /// The detector's boundary //!!!what about SG/FG
        Boundary myBoundary;

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
        class MyValueRetriever : public ValueSource<SUMOReal> {
        public:
            /// Constructor
            MyValueRetriever(GUIE3Collector &det,
                E3::DetType type, size_t nSec)
                : myDetector(det), myType(type), myNSec(nSec) { }

            /// Destructor
            ~MyValueRetriever() { }

            /// Returns the current value
            SUMOReal getValue() const
                {
                    return myDetector.getAggregate(myType, (MSUnit::Seconds) myNSec);
                }

            /// Returns a copy of this instance
            ValueSource<SUMOReal> *copy() const {
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

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

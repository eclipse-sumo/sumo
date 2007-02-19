/****************************************************************************/
/// @file    GUIE3Collector.h
/// @author  Daniel Krajzewicz
/// @date    Jan 2004
/// @version $Id$
///
// The gui-version of the MSE3Collector, together with the according
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
#ifndef GUIE3Collector_h
#define GUIE3Collector_h
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

#include <string>
#include <vector>
#include <microsim/output/e3_detectors/MSE3Collector.h>
#include "GUIDetectorWrapper.h"
#include <utils/geom/Position2DVector.h>
#include <utils/helpers/ValueSource.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIE3Collector
 * The gui-version of the MSE3Collector.
 * Allows the building of a wrapper (also declared herein) which draws the
 * detector on the gl-canvas.
 */
class GUIE3Collector : public MSE3Collector
{
public:
    /// Constructor
    GUIE3Collector(const std::string &id,
                   const CrossSectionVector &entries, const CrossSectionVector &exits,
                   MSUnit::Seconds haltingTimeThreshold,
                   MSUnit::MetersPerSecond haltingSpeedThreshold,
                   SUMOTime deleteDataAfterSeconds);

    /// Destructor
    ~GUIE3Collector();

    /// Returns the list of entry points
    const CrossSectionVector &getEntries() const;

    /// Returns the list of exit points
    const CrossSectionVector &getExits() const;

    /// Builds the wrapper
    GUIDetectorWrapper *buildDetectorWrapper(GUIGlObjectStorage &idStorage);

public:
    /**
     * @class GUIE3Collector::MyWrapper
     * A GUIE3Collector-visualiser
     */
class MyWrapper : public GUIDetectorWrapper
    {
    public:
        /// Constructor
        MyWrapper(GUIE3Collector &detector,
                  GUIGlObjectStorage &idStorage);

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
        GUIE3Collector &getDetector();

    public:
        struct SingleCrossingDefinition
        {
            /// The position in full-geometry mode
            Position2D myFGPosition;
            /// The rotation in full-geometry mode
            SUMOReal myFGRotation;
        };

    protected:
        /// Builds a view within the parameter table if the according type is available
        void myMkExistingItem(GUIParameterTableWindow &ret,
                              const std::string &name, MSE3Collector::DetType type);

        /// Builds the description about the position of the entry/exit point
        SingleCrossingDefinition buildDefinition(const MSCrossSection &section,
                bool exit);

        /// Draws a single entry/exit point
        void drawSingleCrossing(const Position2D &pos, SUMOReal rot,
                                SUMOReal upscale) const;

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
    class MyValueRetriever : public ValueSource<SUMOReal>
        {
        public:
            /// Constructor
            MyValueRetriever(GUIE3Collector &det,
                             MSE3Collector::DetType type, size_t nSec)
                    : myDetector(det), myType(type), myNSec(nSec)
            { }

            /// Destructor
            ~MyValueRetriever()
            { }

            /// Returns the current value
            SUMOReal getValue() const
            {
                return myDetector.getAggregate(myType, (MSUnit::Seconds) myNSec);
            }

            /// Returns a copy of this instance
            ValueSource<SUMOReal> *copy() const
            {
                return new MyValueRetriever(myDetector, myType, myNSec);
            }

        private:
            /// The detctor to get the value from
            GUIE3Collector &myDetector;

            /// The type of the value to retrieve
            MSE3Collector::DetType myType;

            /// The aggregation interval
            size_t myNSec;
        };

    };

};


#endif

/****************************************************************************/


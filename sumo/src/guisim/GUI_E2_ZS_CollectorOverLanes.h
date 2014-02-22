/****************************************************************************/
/// @file    GUI_E2_ZS_CollectorOverLanes.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Okt 2003
/// @version $Id$
///
// The gui-version of a MS_E2_ZS_CollectorOverLanes.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <microsim/output/MS_E2_ZS_CollectorOverLanes.h>
#include <microsim/MSNet.h>
#include <utils/geom/Position.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/VectorHelper.h>
#include <utils/common/ValueSource.h>
#include "GUI_E2_ZS_Collector.h"
#include "GUIDetectorWrapper.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUI_E2_ZS_CollectorOverLanes
 * @brief The gui-version of a MS_E2_ZS_CollectorOverLanes.
 *
 * Allows the building of a wrapper (also declared herein) which draws the
 *  detector on the gl-canvas. Beside this, the method "amVisible" is
 *  overridden to signalise that this detector is not used for simulation-
 *  -internal reasons, but is placed over the simulation by the user.
 */
class GUI_E2_ZS_CollectorOverLanes : public MS_E2_ZS_CollectorOverLanes {
public:
    typedef std::vector<GUI_E2_ZS_Collector*> CollectorVector;

    /// Constructor
    GUI_E2_ZS_CollectorOverLanes(std::string id, DetectorUsage usage,
                                 MSLane* lane, SUMOReal startPos,
                                 SUMOTime haltingTimeThreshold,
                                 SUMOReal haltingSpeedThreshold,
                                 SUMOReal jamDistThreshold);

    /// Destructor
    ~GUI_E2_ZS_CollectorOverLanes();

    // invalid in fact, as collectors do not need a lane
    virtual GUIDetectorWrapper* buildDetectorGUIRepresentation();


protected:
    MSE2Collector* buildCollector(size_t c, size_t r,
                                  MSLane* l, SUMOReal start, SUMOReal end);


public:
    /**
     * @class GUI_E2_ZS_CollectorOverLanes::MyWrapper
     * A GUI_E2_ZS_CollectorOverLanes-visualiser
     */
    class MyWrapper : public GUIDetectorWrapper {
    public:
        /// Constructor
        MyWrapper(GUI_E2_ZS_CollectorOverLanes& detector,
                  const LaneDetMap& detectors);

        /// Destrutor
        ~MyWrapper();


        /// @name inherited from GUIGlObject
        //@{

        /** @brief Returns an own parameter window
         *
         * @param[in] app The application needed to build the parameter window
         * @param[in] parent The parent window needed to build the parameter window
         * @return The built parameter window
         * @see GUIGlObject::getParameterWindow
         */
        GUIParameterTableWindow* getParameterWindow(
            GUIMainWindow& app, GUISUMOAbstractView& parent);


        /** @brief Returns the boundary to which the view shall be centered in order to show the object
         *
         * @return The boundary the object is within
         * @see GUIGlObject::getCenteringBoundary
         */
        Boundary getCenteringBoundary() const;


        /** @brief Draws the object
         * @param[in] s The settings for the current view (may influence drawing)
         * @see GUIGlObject::drawGL
         */
        void drawGL(const GUIVisualizationSettings& s) const;
        //@}


        /// Returns the detector itself
        GUI_E2_ZS_CollectorOverLanes& getLoop();

    protected:
        /// Builds a view within the parameter table if the according type is available
        /*
        void myMkExistingItem(GUIParameterTableWindow &ret,
                              const std::string &name, E2::DetType type);
                              */

    private:
        /// The wrapped detector
        GUI_E2_ZS_CollectorOverLanes& myDetector;

        /// The detector's boundary
        Boundary myBoundary;

        std::vector<GUIDetectorWrapper*> mySubWrappers;

        /**
         * @class GUI_E2_ZS_CollectorOverLanes::MyWrapper::ValueRetriever
         * This class realises the retrieval of a certain value
         * with a certain interval specification from the detector
         */
        /*
        class MyValueRetriever : public ValueSource<SUMOReal>
        {
        public:
            /// Constructor
            MyValueRetriever(GUI_E2_ZS_CollectorOverLanes &det,
                             E2::DetType type, MSUnit::Seconds nSec)
                    : myDetector(det), myType(type), myNSec(nSec) { }

            /// Destructor
            ~MyValueRetriever() { }

            /// Returns the current value
            SUMOReal getValue() const {
                return myDetector.getAggregate(myType, myNSec);
            }

            /// Returns a copy of this instance
            ValueSource<SUMOReal> *copy() const {
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
        */
    private:
        /// @brief Invalidated copy constructor.
        MyWrapper(const MyWrapper&);

        /// @brief Invalidated assignment operator.
        MyWrapper& operator=(const MyWrapper&);

    };

};


#endif

/****************************************************************************/


/****************************************************************************/
/// @file    GUIE3Collector.h
/// @author  Daniel Krajzewicz
/// @date    Jan 2004
/// @version $Id$
///
// The gui-version of a MSE3Collector
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <microsim/output/MSE3Collector.h>
#include "GUIDetectorWrapper.h"
#include <utils/geom/PositionVector.h>
#include <utils/common/ValueSource.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIE3Collector
 * @brief The gui-version of the MSE3Collector.
 *
 * Allows the building of a wrapper (also declared herein) which draws the
 * detector on the gl-canvas.
 */
class GUIE3Collector : public MSE3Collector {
public:
    /// @brief Constructor
    GUIE3Collector(const std::string &id,
                   const CrossSectionVector &entries, const CrossSectionVector &exits,
                   SUMOReal haltingSpeedThreshold,
                   SUMOTime haltingTimeThreshold) throw();

    /// @brief Destructor
    ~GUIE3Collector() throw();


    /** @brief Returns the list of entry points
     * @return The list of entry points
     */
    const CrossSectionVector &getEntries() const;


    /** @brief Returns the list of exit points
     * @return The list of exit points
     */
    const CrossSectionVector &getExits() const;


    /** @brief Returns the wrapper for this detector
     * @return The wrapper representing the detector
     * @see MyWrapper
     */
    GUIDetectorWrapper *buildDetectorGUIRepresentation();


public:
    /**
     * @class GUIE3Collector::MyWrapper
     * A GUIE3Collector-visualiser
     */
    class MyWrapper : public GUIDetectorWrapper {
    public:
        /// @brief Constructor
        MyWrapper(GUIE3Collector &detector) throw();

        /// @brief Destrutor
        ~MyWrapper() throw();


        /// @name inherited from GUIGlObject
        //@{

        /** @brief Returns an own parameter window
         *
         * @param[in] app The application needed to build the parameter window
         * @param[in] parent The parent window needed to build the parameter window
         * @return The built parameter window
         * @see GUIGlObject::getParameterWindow
         */
        GUIParameterTableWindow *getParameterWindow(
            GUIMainWindow &app, GUISUMOAbstractView &parent) throw();


        /** @brief Returns the boundary to which the view shall be centered in order to show the object
         *
         * @return The boundary the object is within
         * @see GUIGlObject::getCenteringBoundary
         */
        Boundary getCenteringBoundary() const throw();


        /** @brief Draws the object
         * @param[in] s The settings for the current view (may influence drawing)
         * @see GUIGlObject::drawGL
         */
        void drawGL(const GUIVisualizationSettings &s) const throw();
        //@}


        /// @brief Returns the detector itself
        GUIE3Collector &getDetector();


    protected:
        /** @struct SingleCrossingDefinition
         * @brief Representation of a single crossing point
         */
        struct SingleCrossingDefinition {
            /// @brief The position
            Position myFGPosition;
            /// @brief The rotation
            SUMOReal myFGRotation;
        };

    protected:
        /// @brief Builds the description about the position of the entry/exit point
        SingleCrossingDefinition buildDefinition(const MSCrossSection &section);

        /// @brief Draws a single entry/exit point
        void drawSingleCrossing(const Position &pos, SUMOReal rot,
                                SUMOReal upscale) const;

    private:
        /// @brief The wrapped detector
        GUIE3Collector &myDetector;

        /// @brief The detector's boundary
        Boundary myBoundary;

        /// @brief Definition of a list of cross (entry/exit-point) positions
        typedef std::vector<SingleCrossingDefinition> CrossingDefinitions;

        /// @brief The list of entry positions
        CrossingDefinitions myEntryDefinitions;

        /// @brief The list of exit positions
        CrossingDefinitions myExitDefinitions;

    };

};


#endif

/****************************************************************************/


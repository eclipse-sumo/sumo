/****************************************************************************/
/// @file    GUIInstantInductLoop.h
/// @author  Daniel Krajzewicz
/// @date    Aug 2003
/// @version $Id$
///
// The gui-version of the MSInstantInductLoop
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIInstantInductLoop_h
#define GUIInstantInductLoop_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/foxtools/MFXMutex.h>
#include <microsim/output/MSInstantInductLoop.h>
#include <utils/geom/Position.h>
#include "GUIDetectorWrapper.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIInstantInductLoop
 * @brief The gui-version of the MSInstantInductLoop
 */
class GUIInstantInductLoop : public MSInstantInductLoop {
public:
    /**
     * @brief Constructor.
     * @param[in] id Unique id
     * @param[in] od The device to write to
     * @param[in] lane Lane where detector woks on.
     * @param[in] position Position of the detector within the lane.
     */
    GUIInstantInductLoop(const std::string& id, OutputDevice& od,
                         MSLane* const lane, SUMOReal positionInMeters, const std::string& vTypes);


    /// @brief Destructor
    ~GUIInstantInductLoop();



    /** @brief Returns this detector's visualisation-wrapper
     * @return The wrapper representing the detector
     */
    virtual GUIDetectorWrapper* buildDetectorGUIRepresentation();


public:
    /**
     * @class GUIInstantInductLoop::MyWrapper
     * @brief A MSInductLoop-visualiser
     */
    class MyWrapper : public GUIDetectorWrapper {
    public:
        /// @brief Constructor
        MyWrapper(GUIInstantInductLoop& detector, SUMOReal pos);

        /// @brief Destructor
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


        /// @brief Returns the detector itself
        GUIInstantInductLoop& getLoop();


    private:
        /// @brief The wrapped detector
        GUIInstantInductLoop& myDetector;

        /// @brief The detector's boundary
        Boundary myBoundary;

        /// @brief The position in full-geometry mode
        Position myFGPosition;

        /// @brief The rotation in full-geometry mode
        SUMOReal myFGRotation;

        /// @brief The position on the lane
        SUMOReal myPosition;

    private:
        /// @brief Invalidated copy constructor.
        MyWrapper(const MyWrapper&);

        /// @brief Invalidated assignment operator.
        MyWrapper& operator=(const MyWrapper&);

    };


    /// @brief Mutex preventing parallel read/write access to internal MSInductLoop state
    mutable MFXMutex myLock;

};


#endif

/****************************************************************************/


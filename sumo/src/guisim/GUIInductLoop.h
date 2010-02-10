/****************************************************************************/
/// @file    GUIInductLoop.h
/// @author  Daniel Krajzewicz
/// @date    Aug 2003
/// @version $Id$
///
// The gui-version of the MSInductLoop, together with the according
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIInductLoop_h
#define GUIInductLoop_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/output/MSInductLoop.h>
#include <utils/geom/Position2D.h>
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
 * @class GUIInductLoop
 * The gui-version of the MSInductLoop.
 * Allows the building of a wrapper (also declared herein) which draws the
 * detector on the gl-canvas.
 */
class GUIInductLoop : public MSInductLoop {
public:
    /**
     * @brief Constructor.
     *
     * Adds reminder to MSLane.
     *
     * @param id Unique id.
     * @param lane Lane where detector woks on.
     * @param position Position of the detector within the lane.
     */
    GUIInductLoop(const std::string &id, MSLane * const lane, SUMOReal position) throw();


    /// @brief Destructor
    ~GUIInductLoop() throw();


    /** @brief Returns this detector's visualisation-wrapper
        valid for gui-version only */
    virtual GUIDetectorWrapper *buildDetectorWrapper(
        GUIGlObjectStorage &idStorage, GUILaneWrapper &lane);

public:
    /**
     * @class GUIInductLoop::MyWrapper
     * A MSInductLoop-visualiser
     */
    class MyWrapper : public GUIDetectorWrapper {
    public:
        /// Constructor
        MyWrapper(GUIInductLoop &detector,
                  GUIGlObjectStorage &idStorage, GUILaneWrapper &wrapper,
                  SUMOReal pos) throw();

        /// Destructor
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


        /** @brief Returns the id of the object as known to microsim
         *
         * @return The id of the induction loop
         * @see GUIGlObject::getMicrosimID
         */
        const std::string &getMicrosimID() const throw();


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



        /// Returns the detector itself
        GUIInductLoop &getLoop();

    private:
        /// The wrapped detector
        GUIInductLoop &myDetector;

        /// The detector's boundary
        Boundary myBoundary;

        /// The position in full-geometry mode
        Position2D myFGPosition;

        /// The rotation in full-geometry mode
        SUMOReal myFGRotation;

        /// The position on the lane
        SUMOReal myPosition;

    private:
        /// @brief Invalidated copy constructor.
        MyWrapper(const MyWrapper&);

        /// @brief Invalidated assignment operator.
        MyWrapper& operator=(const MyWrapper&);

    };

};


#endif

/****************************************************************************/


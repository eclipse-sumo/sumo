/****************************************************************************/
/// @file    GUIMEInductLoop.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 21.07.2005
/// @version $Id$
///
// The gui-version of the MEInductLoop
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
#ifndef GUIMEInductLoop_h
#define GUIMEInductLoop_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <mesosim/MEInductLoop.h>
#include <utils/geom/Position.h>
#include <guisim/GUIDetectorWrapper.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUILane;
class MESegment;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIMEInductLoop
 * The gui-version of the MSInductLoop.
 * Allows the building of a wrapper (also declared herein) which draws the
 * detector on the gl-canvas.
 */
class GUIMEInductLoop : public MEInductLoop {
public:
    /// Construtor
    GUIMEInductLoop(const std::string& id, MESegment* s,
                    SUMOReal position, const std::string& vTypes);

    /// Destructor
    ~GUIMEInductLoop();

    /** @brief Returns this detector's visualisation-wrapper
     * @return The wrapper representing the detector
     */
    virtual GUIDetectorWrapper* buildDetectorGUIRepresentation();


public:
    /**
     * @class GUIMEInductLoop::MyWrapper
     * A MSInductLoop-visualiser
     */
    class MyWrapper : public GUIDetectorWrapper {
    public:
        /// Constructor
        MyWrapper(GUIMEInductLoop& detector,
                  SUMOReal pos);

        /// Destructor
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


        /** @brief Draws the object
         *
         * @param[in] s Current visualization settings
         * @see GUIGlObject::drawGL
         */
        void drawGL(const GUIVisualizationSettings& s) const;


        /** @brief Returns the boundary to which the view shall be centered in order to show the object
         *
         * @return The boundary the object is within
         * @see GUIGlObject::getCenteringBoundary
         */
        Boundary getCenteringBoundary() const;
        //@}


        /// Returns the detector itself
        GUIMEInductLoop& getLoop();

    private:
        /// The wrapped detector
        GUIMEInductLoop& myDetector;

        /// The detector's boundary
        Boundary myBoundary;

        /// The position in full-geometry mode
        Position myFGPosition;

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


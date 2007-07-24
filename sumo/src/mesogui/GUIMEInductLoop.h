/****************************************************************************/
/// @file    GUIMEInductLoop.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 21.07.2005
/// @version $Id: GUIMEInductLoop.h 96 2007-06-06 07:40:46Z behr_mi $
///
// Revision 1.8  2006/05/08 11:00:15  dksumo
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
#include <utils/geom/Position2D.h>
#include <guisim/GUIDetectorWrapper.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObjectStorage;
class GUILaneWrapper;
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
class GUIMEInductLoop : public MEInductLoop
{
public:
    /// Construtor
    GUIMEInductLoop(const std::string &id, MESegment * s,
                    SUMOReal position, SUMOTime deleteDataAfterSeconds);

    /// Destructor
    ~GUIMEInductLoop();

    /** @brief Returns this detector's visualisation-wrapper
        valid for gui-version only */
    virtual GUIDetectorWrapper *buildDetectorWrapper(
        GUIGlObjectStorage &idStorage, GUILaneWrapper &lane);

public:
    /**
     * @class GUIMEInductLoop::MyWrapper
     * A MSInductLoop-visualiser
     */
class MyWrapper : public GUIDetectorWrapper
    {
    public:
        /// Constructor
        MyWrapper(GUIMEInductLoop &detector,
                  GUIGlObjectStorage &idStorage, GUILaneWrapper &wrapper,
                  SUMOReal pos);

        /// Destructor
        ~MyWrapper();

        /// Returns the boundary of the wrapped detector
        Boundary getBoundary() const;

        /// Draws the detector in full-geometry mode
        void drawGL(SUMOReal scale, SUMOReal upscale);

        GUIParameterTableWindow *getParameterWindow(
            GUIMainWindow &app, GUISUMOAbstractView &parent);

        /// returns the id of the object as known to microsim
        const std::string &microsimID() const;

        /// Returns the information whether this detector is still active
        bool active() const;

        /// Returns the wrapped detector's coordinates
        Position2D getPosition() const;

        /// Returns the detector itself
        GUIMEInductLoop &getLoop();

    private:
        /// The wrapped detector
        GUIMEInductLoop &myDetector;

        /// The detector's boundary //!!!what about SG/FG
        Boundary myBoundary;

        /// The position in full-geometry mode
        Position2D myFGPosition;

        /// The rotation in full-geometry mode
        SUMOReal myFGRotation;

        /// The position in simple-geometry mode
        Position2D mySGPosition;

        /// The rotation in simple-geometry mode
        SUMOReal mySGRotation;

        /// The position on the lane
        SUMOReal myPosition;

    };

};


#endif

/****************************************************************************/


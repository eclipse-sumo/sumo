#ifndef GUIInductLoop_h
#define GUIInductLoop_h
//---------------------------------------------------------------------------//
//                        GUIInductLoop.cpp -
//  The gui-version of the MSInductLoop, together with the according
//   wrapper
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Aug 2003
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
// Revision 1.12  2004/12/16 12:14:59  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.11  2004/11/24 08:46:43  dkrajzew
// recent changes applied
//
// Revision 1.10  2004/07/02 08:42:14  dkrajzew
// changes in the detector drawer applied
//
// Revision 1.9  2004/03/19 12:57:54  dkrajzew
// porting to FOX
//
// Revision 1.8  2004/01/26 06:59:37  dkrajzew
// work on detectors: e3-detectors loading and visualisation;
//  variable offsets and lengths for lsa-detectors; coupling of detectors to
//  tl-logics;
//  different detector visualistaion in dependence to his controller
//
// Revision 1.7  2003/11/12 14:00:19  dkrajzew
// commets added; added parameter windows to all detectors
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <microsim/output/MSInductLoop.h>
#include <microsim/MSNet.h>
#include <utils/geom/Position2D.h>
#include "GUIDetectorWrapper.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUIGlObjectStorage;
class GUILaneWrapper;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIInductLoop
 * The gui-version of the MSInductLoop.
 * Allows the building of a wrapper (also declared herein) which draws the
 * detector on the gl-canvas.
 */
class GUIInductLoop : public MSInductLoop
{
public:
    /// Construtor
    GUIInductLoop(const std::string &id, MSLane* lane, double position,
        MSNet::Time deleteDataAfterSeconds);

    /// Destructor
    ~GUIInductLoop();

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
            double pos);

        /// Destructor
        ~MyWrapper();

        /// Returns the boundary of the wrapped detector
        Boundary getBoundary() const;

        /// Draws the detector in full-geometry mode
        void drawGL_FG(double scale);

        /// Draws the detector in simple-geometry mode
        void drawGL_SG(double scale);

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
        GUIInductLoop &getLoop();

    private:
        /// The wrapped detector
        GUIInductLoop &myDetector;

        /// The detector's boundary //!!!what about SG/FG
        Boundary myBoundary;

        /// The position in full-geometry mode
        Position2D myFGPosition;

        /// The rotation in full-geometry mode
        double myFGRotation;

        /// The position in simple-geometry mode
        Position2D mySGPosition;

        /// The rotation in simple-geometry mode
        double mySGRotation;

        /// The position on the lane
        double myPosition;

    };

};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

//#ifndef DISABLE_INLINE
//#include "GUIInductLoop.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:



#ifndef GUIVehicle_h
#define GUIVehicle_h
//---------------------------------------------------------------------------//
//                        GUIVehicle.h -
//  A MSVehicle extended by some values for usage within the gui
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.4  2003/04/14 08:27:18  dkrajzew
// new globject concept implemented
//
// Revision 1.3  2003/03/20 16:19:28  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.2  2003/02/07 10:39:17  dkrajzew
// updated
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <gui/GUIGlObject.h>
#include <microsim/MSVehicle.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * A visualisable MSVehicle. Extended by the possibility to retrieve names
 * of all available vehicles (static) and the possibility to retrieve the
 * color of the vehicle which is available in different forms allowing an
 * easier recognition of done actions such as lane changing.
 */
class GUIVehicle :
                public MSVehicle,
                public GUIGlObject {
public:
    /// destructor
    ~GUIVehicle();

    /// retunrs the names of all available vehicles
    static std::vector<std::string> getNames();

    /// returns the color of the vehicle defined in the xml-description
    const float *getDefinedColor() const;

    /** returns a random color based on the vehicle's name
        (should stay the same across simulations */
    const float *getRandomColor1() const;

    /** retunrs a random color
        (this second random color is fully randomly computed) */
    const float *getRandomColor2() const;

    /** returns a color that describes how long ago the vehicle has
        changed the lane (is white after a lane change and becomes darker
        with each timestep */
    int getPassedColor() const;

    /** returns white if the vehicle has changed the lane in the current step,
        othewise dark grey */
    const float *getLaneChangeColor2() const;

    /** @brief returns the number of steps waited
        A vehicle is meant to be "waiting" when it's speed is less than 0.1
        It is only computed for "critical" vehicles */
    long getWaitingTime() const;

    /** @brief Returns the next "periodical" vehicle with the same route
        We have to duplicate the vehicle if a further has to be emitted with
        the same settings */
    virtual MSVehicle *getNextPeriodical() const;


    /// GUINet is allowed to build vehicles
    friend class GUINet;
protected:
    /// Use this constructor only.
    GUIVehicle( GUIGlObjectStorage &idStorage,
        std::string id, MSRoute* route, MSNet::Time departTime,
        const MSVehicleType* type, size_t noMeanData,
        int repNo, int repOffset, float *defColor);

private:
    /// the color read from the XML-description
    float _definedColor[3];

    /// random color #1 (build from name)
    float _randomColor1[3];

    /// random color #2 (completely random)
    float _randomColor2[3];

    /// white
    static float _laneChangeColor1[3];

    /// dark grey
    static float _laneChangeColor2[3];
};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIVehicle.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


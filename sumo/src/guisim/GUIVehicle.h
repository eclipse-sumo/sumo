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
// Revision 1.16  2004/04/02 11:20:35  dkrajzew
// changes needed to visualise the selection status
//
// Revision 1.15  2004/03/19 12:57:55  dkrajzew
// porting to FOX
//
// Revision 1.14  2004/01/26 15:53:21  dkrajzew
// added some yet unset display variables
//
// Revision 1.13  2004/01/26 07:00:50  dkrajzew
// reinserted the building of repeating vehicles
//
// Revision 1.12  2003/12/11 06:24:55  dkrajzew
// implemented MSVehicleControl as the instance responsible for vehicles
//
// Revision 1.11  2003/11/20 13:06:30  dkrajzew
// loading and using of predefined vehicle colors added
//
// Revision 1.10  2003/10/22 07:07:06  dkrajzew
// patching of lane states on force vehicle removal added
//
// Revision 1.9  2003/08/04 11:35:52  dkrajzew
// only GUIVehicles need a color definition; process of building cars changed
//
// Revision 1.8  2003/07/30 08:54:14  dkrajzew
// the network is capable to display the networks state, now
//
// Revision 1.7  2003/06/05 11:40:28  dkrajzew
// class templates applied; documentation added
//
// Revision 1.6  2003/06/05 06:29:50  dkrajzew
// first tries to build under linux: warnings removed; moc-files included Makefiles added
//
// Revision 1.5  2003/05/20 09:26:57  dkrajzew
// data retrieval for new views added
//
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
#include <utils/gfx/RGBColor.h>
#include <microsim/MSVehicle.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class GUISUMOAbstractView;
class GUIGLObjectPopupMenu;


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

    /// returns the popup-menu for vehicles
    GUIGLObjectPopupMenu *getPopUpMenu(GUIApplicationWindow &app,
        GUISUMOAbstractView &parent);

    GUIParameterTableWindow *getParameterWindow(
        GUIApplicationWindow &app, GUISUMOAbstractView &parent);

    /// Returns the type of the object as coded in GUIGlObjectType
    GUIGlObjectType getType() const;

    /// returns the id of the object as known to microsim
    std::string microsimID() const;

    /** Returns the list of all known junctions as their names */
    static std::vector<std::string> getNames();

    /** Returns the list of all known junctions as their ids */
    static std::vector<size_t> getIDs();

    /// returns the color of the vehicle defined in the xml-description
    const RGBColor &getDefinedColor() const;

    /** returns a random color based on the vehicle's name
        (should stay the same across simulations */
    const RGBColor &getRandomColor1() const;

    /** retunrs a random color
        (this second random color is fully randomly computed) */
    const RGBColor &getRandomColor2() const;

    /** returns a color that describes how long ago the vehicle has
        changed the lane (is white after a lane change and becomes darker
        with each timestep */
    int getPassedColor() const;

    /** returns white if the vehicle has changed the lane in the current step,
        othewise dark grey */
    const RGBColor &getLaneChangeColor2() const;

    /** @brief returns the number of steps waited
        A vehicle is meant to be "waiting" when it's speed is less than 0.1
        It is only computed for "critical" vehicles
        The method return a size_t, now, as we assume a vehicle will not wait for
        longer than about 50 hours which still fits into a size_t when the simulation
        runs in ms */
//    size_t getWaitingTime() const;

    /** @brief Returns the next "periodical" vehicle with the same route
        We have to duplicate the vehicle if a further has to be emitted with
        the same settings */
    virtual MSVehicle *getNextPeriodical() const;


//    virtual size_t getTableParameterNo() const;

// GUINet is allowed to build vehicles
    friend class GUIVehicleControl;
	bool active() const;

	void setRemoved();

    int getRepetitionNo() const;
    int getPeriod() const;
    size_t getLastLaneChangeOffset() const;
    size_t getDesiredDepart() const;



protected:
    /// Use this constructor only.
    GUIVehicle( GUIGlObjectStorage &idStorage,
        std::string id, MSRoute* route, MSNet::Time departTime,
        const MSVehicleType* type, size_t noMeanData,
        int repNo, int repOffset, const RGBColor &color);
/*
    TableType getTableType(size_t pos) const;

    const char *getTableBeginValue(size_t pos) const;
*/

private:
    /// the color read from the XML-description
    RGBColor myDefinedColor;

    /// random color #1 (build from name)
    RGBColor _randomColor1;

    /// random color #2 (completely random)
    RGBColor _randomColor2;

    /// white
    static RGBColor _laneChangeColor1;

    /// dark grey
    static RGBColor _laneChangeColor2;

};

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "GUIVehicle.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


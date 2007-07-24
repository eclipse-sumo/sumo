/****************************************************************************/
/// @file    MEVehicle.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: MEVehicle.h 96 2007-06-06 07:40:46Z behr_mi $
///
// A vehicle from the mesoscopic point of view
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
#ifndef MEVehicle_h
#define MEVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/StdDefs.h>
#include <iostream>
#include <cassert>


// ===========================================================================
// class declarations
// ===========================================================================
class MESegment;
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MEVehicle
 * @brief A vehicle from the mesoscopic point of view
 */
class MEVehicle
{
public:
    /// Old constructor by Nils Eissfeldt
    MEVehicle(MESegment *s, SUMOReal t);

    /// SUMO constructor
    MEVehicle(MSVehicle *cor, MESegment *s, SUMOReal t);

    virtual ~MEVehicle()
    {}

    SUMOReal get_tEvent();
    MESegment *at_segment();

    void update_tEvent(SUMOReal t);

    void update_segment(MESegment *s);

    MSVehicle * const getSUMOVehicle() const;

    void setTLastEntry(SUMOReal t);

    SUMOReal getTLastEntry() const
    {
        return tLastEntry;
    }

    SUMOReal getNeededTime() const;


    SUMOReal me_length() const;

    bool inserted;

protected:
    MSVehicle *mySUMOVehicle;
    MESegment *seg;
    SUMOReal tEvent;
    SUMOReal tLastEntry;

};

#endif

/****************************************************************************/

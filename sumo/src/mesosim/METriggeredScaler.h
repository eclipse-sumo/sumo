/****************************************************************************/
/// @file    METriggeredScaler.h
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id: METriggeredScaler.h 102 2007-06-12 06:17:01Z behr_mi $
///
// Scales the flow on a given segment by a given factor
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
#ifndef METriggeredScaler_h
#define METriggeredScaler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_MESOSIM

#include <string>
#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MESegment;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class METriggeredScaler
 * @brief Scales the flow on a given segment by a given factor
 */
class METriggeredScaler :
            public MSTrigger
{
public:
    /** constructor */
    METriggeredScaler(const std::string &id,
                      MESegment *edge, SUMOReal scale);

    /** destructor */
    virtual ~METriggeredScaler();


    SUMOTime execute(SUMOTime currentTime);

protected:
    MESegment *mySegment;
    SUMOReal myRemovedNo;
    SUMOReal myAggregatedPassedQ;
    SUMOReal myScale;

};

#endif // HAVE_MESOSIM

#endif

/****************************************************************************/

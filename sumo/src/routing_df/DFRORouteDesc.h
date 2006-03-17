#ifndef DFRORouteDesc_h
#define DFRORouteDesc_h
/***************************************************************************
                          DFRORouteDesc.h
    A route within the DFROUTER
                             -------------------
    project              : SUMO
    begin                : Thu, 16.03.2006
    copyright            : (C) 2006 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.4  2006/03/17 09:04:26  dkrajzew
// class-documentation added/patched
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <utils/common/SUMOTime.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ROEdge;
class DFDetector;


/* =========================================================================
 * struct definitions
 * ======================================================================= */
/**
 * @struct DFRORouteDesc
 * @brief A route within the DFROUTER
 */
struct DFRORouteDesc {
    std::vector<ROEdge*> edges2Pass;
    std::string routename;
    float duration;
    float distance;
//	std::vector<std::pair<SUMOReal, SUMOReal> > myProbabilities;
	int passedNo;
	const ROEdge *endDetectorEdge;
	const ROEdge *lastDetectorEdge;
    float distance2Last;
    SUMOTime duration2Last;

    float overallProb;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

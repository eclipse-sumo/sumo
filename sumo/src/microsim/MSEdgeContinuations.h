/****************************************************************************/
/// @file    MSEdgeContinuations.h
/// @author  Daniel Krajzewicz
/// @date    unknown_date
/// @version $Id: $
///
// missing_desc
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
#ifndef MSEdgeContinuations_h
#define MSEdgeContinuations_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <vector>

class MSEdge;

class MSEdgeContinuations
{
public:
    MSEdgeContinuations();
    ~MSEdgeContinuations();
    void add(MSEdge *to, MSEdge *from);
    const std::vector<MSEdge*> &getInFrontOfEdge(const MSEdge &toEdge) const;
    bool hasFurther(const MSEdge &toEdge) const;

private:
    std::map<MSEdge*, std::vector<MSEdge*> > myContinuations;
};


#endif

/****************************************************************************/


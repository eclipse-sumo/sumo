/****************************************************************************/
/// @file    NBNodeShapeComputer.h
/// @author  unknown_author
/// @date    2004-01-12
/// @version $Id$
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
#ifndef NBNodeShapeComputer_h
#define NBNodeShapeComputer_h
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

#include <fstream>
#include <utils/geom/Position2DVector.h>

class NBNode;
class NBEdge;

class NBNodeShapeComputer
{
public:
    NBNodeShapeComputer(const NBNode &node, std::ofstream * const out);
    ~NBNodeShapeComputer();
    Position2DVector compute();

private:
    void addInternalGeometry();
    Position2DVector computeContinuationNodeShape(bool simpleContinuation);
    Position2DVector computeNodeShapeByCrosses();
    bool isSimpleContinuation(const NBNode &n) const;
private:
    const NBNode &myNode;
    std::ofstream * const myOut;

};


#endif

/****************************************************************************/


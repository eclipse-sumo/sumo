/****************************************************************************/
/// @file    GUIGeomShapeBuilder.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
//	»missingDescription«
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
#ifndef GUIGeomShapeBuilder_h
#define GUIGeomShapeBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <netload/NLGeomShapeBuilder.h>

class ShapeContainer;
class GUIGlObjectStorage;

class GUIGeomShapeBuilder : public NLGeomShapeBuilder
{
public:
    GUIGeomShapeBuilder(MSNet &net, GUIGlObjectStorage &idStorage);
    ~GUIGeomShapeBuilder();

    void polygonEnd(const Position2DVector &shape);

    void addPoint(const std::string &name, int layer, const std::string &type,
                  const RGBColor &c, SUMOReal x, SUMOReal y,
                  const std::string &lane, SUMOReal posOnLane);

protected:
    GUIGlObjectStorage &myIdStorage;

};


#endif

/****************************************************************************/


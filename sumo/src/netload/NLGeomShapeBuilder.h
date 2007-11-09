/****************************************************************************/
/// @file    NLGeomShapeBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Sep 2005
/// @version $Id$
///
// Builder for geometrical objects
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
#ifndef NLGeomShapeBuilder_h
#define NLGeomShapeBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/RGBColor.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ShapeContainer;
class MSNet;


#define INVALID_POSITION -1000000

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NLGeomShapeBuilder
 * This class builds geometrical shapes
 */
class NLGeomShapeBuilder
{
public:
    /// Constructor
    NLGeomShapeBuilder(MSNet &net);

    /// Destructor
    virtual ~NLGeomShapeBuilder();

    /** @brief Called when a polygon begins
        The values are stored in order to allocate the complete polygon after
        the shape has been parsed, too */
    virtual void polygonBegin(const std::string &name, int layer,
                              const std::string &type, const RGBColor &c, bool fill);

    /** @brief Ends the parsing of the polygon allocating it. */
    virtual void polygonEnd(const Position2DVector &shape);

    /// Adds the described PointOfInterest to the geometry container
    virtual void addPoint(const std::string &name, int layer,
                          const std::string &type, const RGBColor &c,
                          SUMOReal x, SUMOReal y, const std::string &lane, SUMOReal posOnLane);

    /// Builds the shape container
    ShapeContainer *buildShapeContainer() const;

protected:
    Position2D getPointPosition(SUMOReal x, SUMOReal y,
                                const std::string &laneID, SUMOReal posOnLane) const;

protected:
    /// The current polygon's name
    std::string myCurrentName;

    /// The current polygon's type
    std::string myCurrentType;

    /// The current polygon's color
    RGBColor myCurrentColor;

    /// The layer thepolygon shall be added to
    int myCurrentLayer;

    /// Information whether the polygon shall be filled
    bool myFillPoly;

    /// The shape container
    ShapeContainer &myShapeContainer;


};


#endif

/****************************************************************************/


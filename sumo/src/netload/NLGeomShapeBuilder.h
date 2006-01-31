#ifndef NLGeomShapeBuilder_h
#define NLGeomShapeBuilder_h
/***************************************************************************
                          NLGeomShapeBuilder.h
              Builder for geometrical objects
                             -------------------
    project              : SUMO
    begin                : Sep 2005
    copyright            : (C) 2005 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.6  2006/01/31 10:53:44  dkrajzew
// pois may be now placed on lane positions
//
// Revision 1.5  2005/11/09 06:32:46  dkrajzew
// problems on loading geometry items patched
//
// Revision 1.4  2005/10/10 12:11:33  dkrajzew
// debugging
//
// Revision 1.3  2005/10/07 11:41:49  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.2  2005/09/23 06:04:11  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.1  2005/09/15 12:04:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.2  2005/09/15 08:59:32  dksumo
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

#include <string>
#include <utils/gfx/RGBColor.h>
#include <utils/geom/Position2D.h>
#include <utils/geom/Position2DVector.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class ShapeContainer;
class MSNet;


#define INVALID_POSITION -1000000

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NLGeomShapeBuilder
 * This class builds geometrical shapes
 */
class NLGeomShapeBuilder {
public:
    /// Constructor
    NLGeomShapeBuilder(MSNet &net);

    /// Destructor
    ~NLGeomShapeBuilder();

    /** @brief Called when a polygon begins
        The values are stored in order to allocate the complete polygon after
        the shape has been parsed, too */
    virtual void polygonBegin(const std::string &name,
        const std::string &type, const RGBColor &c);

    /** @brief Ends the parsing of the polygon allocating it. */
    virtual void polygonEnd(const Position2DVector &shape);

    /// Adds the described PointOfInterest to the geometry container
    virtual void addPoint(const std::string &name,
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

    /// The shape container
    ShapeContainer &myShapeContainer;

};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

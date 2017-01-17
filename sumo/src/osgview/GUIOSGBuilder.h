/****************************************************************************/
/// @file    GUIOSGBuilder.h
/// @author  Daniel Krajzewicz
/// @date    19.01.2012
/// @version $Id$
///
// Builds OSG nodes from microsim objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIOSGBuilder_h
#define GUIOSGBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_OSG

#include <map>
#include <osg/ref_ptr>
#include "GUIOSGView.h"


// ===========================================================================
// class declarations
// ===========================================================================
namespace osg {
class Node;
class Group;
class PositionAttitudeTransform;
}
namespace osgUtil {
class Tessellator;
}
class MSVehicleType;
class MSEdge;
class GUIJunctionWrapper;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIOSGBuilder
 * @brief Builds OSG nodes from microsim objects
 */
class GUIOSGBuilder {
public:
    static osg::Group* buildOSGScene(osg::Node* const tlg, osg::Node* const tly, osg::Node* const tlr, osg::Node* const tlu);

    static void buildDecal(const GUISUMOAbstractView::Decal& d, osg::Group& addTo);

    static void buildLight(const GUISUMOAbstractView::Decal& d, osg::Group& addTo);

    static osg::PositionAttitudeTransform* getTrafficLight(const GUISUMOAbstractView::Decal& d, osg::Node* tl, const osg::Vec4& color, const SUMOReal size = 0.5);

    static GUIOSGView::OSGMovable buildMovable(const MSVehicleType& type);

private:
    static void buildOSGEdgeGeometry(const MSEdge& edge,
                                     osg::Group& addTo, osgUtil::Tessellator& tessellator);

    static void buildOSGJunctionGeometry(GUIJunctionWrapper& junction,
                                         osg::Group& addTo, osgUtil::Tessellator& tessellator);

    static void setShapeState(osg::ref_ptr<osg::ShapeDrawable> shape);

private:
    static std::map<std::string, osg::ref_ptr<osg::Node> > myCars;

};


#endif

#endif

/****************************************************************************/


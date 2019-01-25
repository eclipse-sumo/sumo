/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIOSGBuilder.h
/// @author  Daniel Krajzewicz
/// @date    19.01.2012
/// @version $Id$
///
// Builds OSG nodes from microsim objects
/****************************************************************************/
#ifndef GUIOSGBuilder_h
#define GUIOSGBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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

    static osg::PositionAttitudeTransform* getTrafficLight(const GUISUMOAbstractView::Decal& d, osg::Node* tl, const osg::Vec4& color, const double size = 0.5);

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


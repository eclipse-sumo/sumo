/****************************************************************************/
/// @file    GUIOSGBoundingBoxCalculator.h
/// @author  Gordon Thomlinson
/// @author  Michael Behrisch
/// @date    19.01.2012
/// @version $Id$
///
// Calculates the bounding box of an osg node
// original source: http://www.vis-sim.com/osg/code/osgcode_bbox1.htm
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
#ifndef GUIOSGBoundingBoxCalculator_h
#define GUIOSGBoundingBoxCalculator_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_OSG

#include <osg/NodeVisitor>
#include <osg/BoundingBox>
#include <osg/BoundingSphere>
#include <osg/MatrixTransform>
#include <osg/Billboard>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIOSGBoundingBoxCalculator
 *  usage example:
 *    GUIOSGBoundingBoxCalculator bboxCalc;
 *    node->accept(bboxCalc);
 *    osg::BoundingBox boxExtents = bboxCalc.getBoundingBox();
 * @brief Builds an OSG view
 */
class GUIOSGBoundingBoxCalculator : public osg::NodeVisitor {
public:
    GUIOSGBoundingBoxCalculator() : NodeVisitor(NodeVisitor::TRAVERSE_ALL_CHILDREN) {
        myTransformMatrix.makeIdentity();
    }

    virtual ~GUIOSGBoundingBoxCalculator() {}

    void apply(osg::Geode& geode) {
        osg::BoundingBox bbox;
        for (int i = 0; i < (int)geode.getNumDrawables(); ++i) {
#if OSG_MIN_VERSION_REQUIRED(3,4,0)
            bbox.expandBy(geode.getDrawable(i)->getBoundingBox());
#else
            bbox.expandBy(geode.getDrawable(i)->getBound());
#endif
        }
        osg::BoundingBox bboxTrans;
        for (int i = 0; i < 8; ++i) {
            osg::Vec3 xvec = bbox.corner(i) * myTransformMatrix;
            bboxTrans.expandBy(xvec);
        }
        myBoundingBox.expandBy(bboxTrans);
        traverse(geode);
    }

    void apply(osg::MatrixTransform& node) {
        myTransformMatrix *= node.getMatrix();
        traverse(node);
    }

    void apply(osg::Billboard& node) {
        traverse(node);
    }

    osg::BoundingBox& getBoundingBox() {
        return myBoundingBox;
    }


protected:
    osg::BoundingBox myBoundingBox;          // the overall resultant bounding box
    osg::Matrix      myTransformMatrix;      // the current transform matrix


};

#endif

#endif

/****************************************************************************/
/// @file    SUMORTree.h
/// @author  Daniel Krajzewicz
/// @date    27.10.2008
/// @version $Id$
///
// A RT-tree for efficient storing of SUMO's GL-objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMORTree_h
#define SUMORTree_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIVisualizationSettings.h>
#include <utils/geom/Boundary.h>
#include <utils/foxtools/MFXMutex.h>

#include "RTree.h"


#define GUI_RTREE_QUAL RTree<GUIGlObject*, GUIGlObject, float, 2, GUIVisualizationSettings>

// specialized implementation for speedup and avoiding warnings

template<>
inline float GUI_RTREE_QUAL::RectSphericalVolume(Rect* a_rect) {
  ASSERT(a_rect);
  const float extent0 = a_rect->m_max[0] - a_rect->m_min[0];
  const float extent1 = a_rect->m_max[1] - a_rect->m_min[1];
  return .78539816f * (extent0 * extent0 + extent1 * extent1);
}

template<>
inline GUI_RTREE_QUAL::Rect GUI_RTREE_QUAL::CombineRect(Rect* a_rectA, Rect* a_rectB) {
    ASSERT(a_rectA && a_rectB);
    Rect newRect;
    newRect.m_min[0] = rtree_min(a_rectA->m_min[0], a_rectB->m_min[0]);
    newRect.m_max[0] = rtree_max(a_rectA->m_max[0], a_rectB->m_max[0]);
    newRect.m_min[1] = rtree_min(a_rectA->m_min[1], a_rectB->m_min[1]);
    newRect.m_max[1] = rtree_max(a_rectA->m_max[1], a_rectB->m_max[1]);
    return newRect;
}


// ===========================================================================
// class definitions
// ===========================================================================
/** @class SUMORTree
 * @brief A RT-tree for efficient storing of SUMO's GL-objects
 * 
 * This class specialises the used RT-tree implementation from "rttree.h" and
 *  extends it by a mutex for avoiding parallel change and traversal of the tree.
 */
class SUMORTree : private GUI_RTREE_QUAL, public Boundary
{
public:
    /// @brief Constructor
    SUMORTree() : GUI_RTREE_QUAL(&GUIGlObject::drawGL) {
    }


    /// @brief Destructor
    virtual ~SUMORTree() {
    }


    /** @brief Insert entry
     * @param a_min Min of bounding rect
     * @param a_max Max of bounding rect
     * @param a_dataId Positive Id of data.  Maybe zero, but negative numbers not allowed.
     * @see RTree::Insert
     */
    virtual void Insert(const float a_min[2], const float a_max[2], GUIGlObject* a_dataId) {
        AbstractMutex::ScopedLocker locker(myLock);
        GUI_RTREE_QUAL::Insert(a_min, a_max, a_dataId);
    }


    /** @brief Remove entry
     * @param a_min Min of bounding rect
     * @param a_max Max of bounding rect
     * @param a_dataId Positive Id of data.  Maybe zero, but negative numbers not allowed.
     * @see RTree::Remove
     */
    virtual void Remove(const float a_min[2], const float a_max[2], GUIGlObject* a_dataId) {
        AbstractMutex::ScopedLocker locker(myLock);
        GUI_RTREE_QUAL::Remove(a_min, a_max, a_dataId);
    }


    /** @brief Find all within search rectangle
     * @param a_min Min of search bounding rect
     * @param a_max Max of search bounding rect
     * @param a_searchResult Search result array.  Caller should set grow size. Function will reset, not append to array.
     * @param a_resultCallback Callback function to return result.  Callback should return 'true' to continue searching
     * @param a_context User context to pass as parameter to a_resultCallback
     * @return Returns the number of entries found
     * @see RTree::Search
     */
    virtual int Search(const float a_min[2], const float a_max[2], const GUIVisualizationSettings& c) const {
        AbstractMutex::ScopedLocker locker(myLock);
        return GUI_RTREE_QUAL::Search(a_min, a_max, c);
    }


    /** @brief Adds an additional object (detector/shape/trigger) for visualisation
     * @param[in] o The object to add
     */
    void addAdditionalGLObject(GUIGlObject *o) {
        AbstractMutex::ScopedLocker locker(myLock);
        Boundary b = o->getCenteringBoundary();
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        Insert(cmin, cmax, o);
    }


    /** @brief Removes an additional object (detector/shape/trigger) from being visualised
     * @param[in] o The object to remove
     */
    void removeAdditionalGLObject(GUIGlObject *o) {
        AbstractMutex::ScopedLocker locker(myLock);
        Boundary b = o->getCenteringBoundary();
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        Remove(cmin, cmax, o);
    }


protected:
    /// @brief A mutex avoiding parallel change and traversal of the tree
    mutable MFXMutex myLock;

};


#endif

/****************************************************************************/


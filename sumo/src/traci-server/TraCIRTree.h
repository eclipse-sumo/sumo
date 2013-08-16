/****************************************************************************/
/// @file    TraCIRTree.h
/// @author  Daniel Krajzewicz
/// @date    27.10.2008
/// @version $Id$
///
// A RT-tree for efficient storing of SUMO's GL-objects
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCIRTree_h
#define TraCIRTree_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <set>
#include <foreign/rtree/RTree.h>
#include <utils/common/Named.h>

//#include "RTree.h"


// specialized implementation for speedup and avoiding warnings
template<>
inline float RTree<Named*, Named, float, 2, Named::StoringVisitor, float, 8, 4>::RectSphericalVolume(Rect* a_rect) {
    ASSERT(a_rect);
    const float extent0 = a_rect->m_max[0] - a_rect->m_min[0];
    const float extent1 = a_rect->m_max[1] - a_rect->m_min[1];
    return .78539816f * (extent0 * extent0 + extent1 * extent1);
}


// ===========================================================================
// class definitions
// ===========================================================================
/** @class TraCIRTree
 * @brief A RT-tree for efficient storing of SUMO's GL-objects
 *
 * This class specialises the used RT-tree implementation from "rttree.h" and
 *  extends it by a mutex for avoiding parallel change and traversal of the tree.
 */
class TraCIRTree : private RTree<Named*, Named, float, 2, Named::StoringVisitor >, public Boundary {
public:
    /// @brief Constructor
    TraCIRTree()
        : RTree<Named*, Named, float, 2, Named::StoringVisitor, float>(&Named::addTo) {
    }


    /// @brief Destructor
    ~TraCIRTree() {
    }


    /** @brief Insert entry
     * @param a_min Min of bounding rect
     * @param a_max Max of bounding rect
     * @param a_dataId Positive Id of data.  Maybe zero, but negative numbers not allowed.
     * @see RTree::Insert
     */
    void Insert(const float a_min[2], const float a_max[2], Named* a_dataId) {
        //AbstractMutex::ScopedLocker locker(myLock);
        RTree<Named*, Named, float, 2, Named::StoringVisitor, float>::Insert(a_min, a_max, a_dataId);
    }


    /** @brief Remove entry
     * @param a_min Min of bounding rect
     * @param a_max Max of bounding rect
     * @param a_dataId Positive Id of data.  Maybe zero, but negative numbers not allowed.
     * @see RTree::Remove
     */
    void Remove(const float a_min[2], const float a_max[2], Named* a_dataId) {
        //AbstractMutex::ScopedLocker locker(myLock);
        RTree<Named*, Named, float, 2, Named::StoringVisitor, float>::Remove(a_min, a_max, a_dataId);
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
    int Search(const float a_min[2], const float a_max[2], const Named::StoringVisitor& c) {
        //AbstractMutex::ScopedLocker locker(myLock);
        return RTree<Named*, Named, float, 2, Named::StoringVisitor, float>::Search(a_min, a_max, c);
    }


    /** @brief Adds an additional object (detector/shape/trigger) for visualisation
     * @param[in] o The object to add
     * @param[in] b The object's boundary
     */
    void addObject(Named* o, Boundary& b) {
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        Insert(cmin, cmax, o);
    }


    /** @brief Adds an additional object (detector/shape/trigger) for visualisation
     * @param[in] o The object to add
     * @param[in] p The object's position
     */
    void addObject(Named* o, Position& p) {
        Boundary b;
        b.add(p);
        addObject(o, b);
    }


    /** @brief Removes an additional object (detector/shape/trigger) from being visualised
     * @param[in] o The object to remove
     */
    void removeAdditionalGLObject(Named* o, Boundary& b) {
        const float cmin[2] = {(float) b.xmin(), (float) b.ymin()};
        const float cmax[2] = {(float) b.xmax(), (float) b.ymax()};
        Remove(cmin, cmax, o);
    }


protected:
    /// @brief A mutex avoiding parallel change and traversal of the tree
    //MFXMutex myLock;

};


#endif

/****************************************************************************/


/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    DynamicPolygon.h
/// @author  Leonhard Luecken
/// @date    Apr 2019
/// @version $Id$
///
// A polygon, which holds a timeSpan for displaying dynamic properties
/****************************************************************************/


#ifndef UTILS_SHAPES_DYNAMICPOLYGON_H_
#define UTILS_SHAPES_DYNAMICPOLYGON_H_

#include <memory>
#include "SUMOPolygon.h"

class SUMOTrafficObject;

class DynamicPolygon: public SUMOPolygon {
public:
    /**
     * @brief Constructor that takes a SUMOPolygon and adds timelines for the properties to be modified dynamically.
     * @param p Polygon to be modified
     * @param pos Initial position of the polygon
     * @param trackedObject A tracked object
     * @param timeSpan Anchor time points
     *        For animated polygons: assumed to have a size >= 2, and start at timeSpan[0]=0,
     *        such that timeSpan[i+1] >= timeSpan[i])
     *        If no animation is desired, give timeSpan == nullptr
     * @param ...Span property timelines (assumed to be either nullptr, or of size equal to timeSpan (in case of animated poly))
     */
    DynamicPolygon(SUMOPolygon* p, std::shared_ptr<Position> pos,
            SUMOTrafficObject* trackedObject,
            std::shared_ptr<std::vector<double> > timeSpan,
            std::shared_ptr<std::vector<double> > alphaSpan);
    virtual ~DynamicPolygon();


//    /**
//     * @brief Moves the polygon to the given coordinates
//     * @param pos New position of the Polygon
//     * @note The movement is executed in a relative fashion, i.e.,
//     *       by updating the shape's position by the increment between
//     *       the given and the previously stored position of the polygon.
//     */
//    void moveTo(const Position& pos);

private:

     /// @brief Updates the polygon according to its timeSpan and follows the tracked object
     /// @param t Current sumo time step (in ms.) when this is called
    SUMOTime update(SUMOTime t);

    /// @brief Sets the alpha value for the shape's color
    void setAlpha(double alpha);

    /// @brief Schedules the next update for this instance in TS seconds
    void scheduleUpdate();

    /// @brief Schedules the removal of this instance
    void scheduleRemoval();

    /// @brief An object tracked by the shape, deletion by caller
    /// @todo  Ensure deletion of the polygon as soon as the pointer looses validity
    SUMOTrafficObject* myTrackedObject;

    /// @brief Previously known position for the tracked object
    std::shared_ptr<Position> myTrackedPos;

    /// @brief Current time
    double myCurrentTime;

    /// @brief The last time the animation has been updated
    SUMOTime myLastUpdateTime;

    /// @brief Whether this polygon is animated, i.e., whether
    ///        timelines should be used to control properties.
    bool animated;

    /// @brief Whether this polygon tracks an object
    bool tracking;

    /// @brief Time points corresponding to the anchor values of the dynamic properties
    /// @note  Assumed to have a size >= 2, and start at timeSpan[0]=0, such that timeSpan[i+1] >= timeSpan[i]
    std::shared_ptr<std::vector<double>> myTimeSpan;

    /// @brief Pointer to the next time points in timeSpan
    std::vector<double>::const_iterator myPrevTime;
    std::vector<double>::const_iterator myNextTime;

    /// @brief Alpha values corresponding to
    std::shared_ptr<std::vector<double>> myAlphaSpan;
    /// @brief Pointer to the next alpha points in alphaSpan
    std::vector<double>::const_iterator myPrevAlpha;
    std::vector<double>::const_iterator myNextAlpha;


    /// @name Commands sent to the EventControl (used for cleanup)
    /// @{
    /// @brief Command scheduled for regular updates of the animation
    /// @note  Must be descheduled in destructor (or at removal)
    WrappingCommand<DynamicPolygon>* myUpdateCommand;
    /// @}

};

#endif /* UTILS_SHAPES_DYNAMICPOLYGON_H_ */

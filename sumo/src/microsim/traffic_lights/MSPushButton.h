/****************************************************************************/
/// @file    MSPushButton.h
/// @author  Federico Caselli
/// @date    May 2015
/// @version $Id$
///
// The class for a PushButton
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#ifndef SRC_MICROSIM_TRAFFIC_LIGHTS_MSPUSHBUTTON_H_
#define SRC_MICROSIM_TRAFFIC_LIGHTS_MSPUSHBUTTON_H_

#include <vector>
#include <map>
#include <string>

class MSEdge;
class MSPhaseDefinition;

/**
 * Abstract push button class
 */
class MSPushButton {
public:
    virtual ~MSPushButton();

    /**
     * @brief Checks if the the pushbutton has been pressed
     * @return true if pressed, false otherwise
     */
    virtual bool isActivated() const = 0;

    /**
     * @brief Checks if any pushbutton in the vector is active
     * @return True if at least one pushbutton is active, false otherwise
     */
    static bool anyActive(const std::vector<MSPushButton*>&);
protected:
    /**
     * Protected constructor
     * @param[in] edge: the edge where the push button is located
     * @param[in] crossingEdge: the crossing controlled by the push button
     */
    MSPushButton(const MSEdge* edge, const MSEdge* crossingEdge);
    const MSEdge* m_edge;
    const MSEdge* m_crossingEdge;
};

/**
 * Pedestrian push button
 */
class MSPedestrianPushButton: MSPushButton {
public:
    /**
     * MSPedestrianPushButton constructor
     * @param[in] edge: the edge where the push button is located. Must be a walking area.
     * @param[in] crossingEdge: the crossing controlled by the push button. Must be a crossing.
     */
    MSPedestrianPushButton(const MSEdge* walkingEdge, const MSEdge* crossingEdge);
    virtual ~MSPedestrianPushButton() {
    }

    ///@brief  abstract methods inherited from PedestrianState
    ///@{
    bool isActivated() const;
    ///@}

    /**
     * @brief Static method with the same behavior of isActivated
     * @brief Checks if the the pushbutton has been pressed for a particular crossing from a edge.
     * @return true if pressed, false otherwise
     */
    static bool isActiveForEdge(const MSEdge* walkingEdge, const MSEdge* crossing);

    /**
     * @brief Static method to check if the push button is active on both side of the road
     * @param[in] A crossing edge
     * @return true if pressed, false otherwise
     */
    static bool isActiveOnAnySideOfTheRoad(const MSEdge* crossing);

    /**
     * @brief Loads all the pushbuttons for all the controlled lanes of a stage
     * @param[in] A phase definition
     * @return A list of pushbuttons
     */
    static std::vector<MSPushButton*> loadPushButtons(const MSPhaseDefinition*);
private:
//		Map edge id -> list of crossing edges that crosses it
    static std::map<std::string, std::vector<std::string> > m_crossingEdgeMap;
    static bool m_crossingEdgeMapLoaded;
//		Load the crossingEdgeMap
    static void loadCrossingEdgeMap();
};

#endif /* SRC_MICROSIM_TRAFFIC_LIGHTS_MSPUSHBUTTON_H_ */

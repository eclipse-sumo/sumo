/****************************************************************************/
/// @file    NBLoadedSUMOTLDef.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id$
///
// A complete traffic light logic loaded from a sumo-net. (opted to reimplement
// since NBLoadedTLDef is quite vissim specific)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBLoadedSUMOTLDef_h
#define NBLoadedSUMOTLDef_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <set>
#include "NBNode.h"
#include "NBEdgeCont.h"
#include "NBTrafficLightDefinition.h"
#include "NBTrafficLightLogic.h"
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBLoadedSUMOTLDef
 * @brief A loaded (complete) traffic light logic
 */
class NBLoadedSUMOTLDef : public NBTrafficLightDefinition {
public:

    /** @brief Constructor
     * @param[in] id The id of the tls
     * @param[in] programID The programID for the computed logic
     * @param[in] offset The offset for the computed logic
     */
    NBLoadedSUMOTLDef(const std::string &id, const std::string &programID, SUMOTime offset) throw();


    /// @brief Destructor
    ~NBLoadedSUMOTLDef() throw();

    /** @brief Informs edges about being controlled by a tls
     * @param[in] ec The container of edges
     */
    void setTLControllingInformation(const NBEdgeCont &ec) const throw();

    /** @brief Replaces occurences of the removed edge in incoming/outgoing edges of all definitions
     * @param[in] removed The removed edge
     * @param[in] incoming The edges to use instead if an incoming edge was removed
     * @param[in] outgoing The edges to use instead if an outgoing edge was removed
     */
    void remapRemoved(NBEdge *removed,
                              const EdgeVector &incoming, const EdgeVector &outgoing) throw();


    /** @brief Replaces a removed edge/lane
     * @param[in] removed The edge to replace
     * @param[in] removedLane The lane of this edge to replace
     * @param[in] by The edge to insert instead
     * @param[in] byLane This edge's lane to insert instead
     */
    void replaceRemoved(NBEdge *removed, int removedLane,
                                NBEdge *by, int byLane) throw();


    /** @brief Adds a phase to the logic
     * the new phase is inserted at the end of the list of already added phases
     * @param[in] duration The duration of the phase to add in SECONDS!
     * @param[in] state The state definition of a tls phase
     */
    void addPhase(SUMOTime duration, const std::string &state);


    /** @brief Adds a connection and immediately informs the edges
     */
    void addConnection(NBEdge *from, NBEdge *to, int fromLane, int toLane, int linkno);


    /** @brief Returns the ProgramID */
    const std::string & getProgramID() const {
        return myTLLogic->getProgramID();
    };

protected:
    /** @brief Computes the traffic light logic finally in dependence to the type
     * @param[in] ec The edge container
     * @param[in] brakingTime Duration a vehicle needs for braking in front of the tls
     * @return The computed logic
     */
    NBTrafficLightLogic *myCompute(const NBEdgeCont &ec,
                                           unsigned int brakingTime) throw();

private:

    /** @brief phases are added directly to myTLLogic which is then returned in myCompute() */
    NBTrafficLightLogic *myTLLogic;


};


#endif

/****************************************************************************/


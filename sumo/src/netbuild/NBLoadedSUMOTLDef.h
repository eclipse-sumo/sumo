/****************************************************************************/
/// @file    NBLoadedSUMOTLDef.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mar 2011
/// @version $Id$
///
// A complete traffic light logic loaded from a sumo-net. (opted to reimplement
// since NBLoadedTLDef is quite vissim specific)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2011-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
     * @param[in] type The algorithm type for the computed logic
     */
    NBLoadedSUMOTLDef(const std::string& id, const std::string& programID, SUMOTime offset, TrafficLightType type);

    /** @brief Constructor that copies from an existing definition and its computed logic (used by NETEDIT)
     * @param[in] def The definition to copy
     * @param[in] logic The computed logic of the given def
     */
    NBLoadedSUMOTLDef(NBTrafficLightDefinition* def, NBTrafficLightLogic* logic);


    /// @brief Destructor
    ~NBLoadedSUMOTLDef();

    /** @brief Informs edges about being controlled by a tls
     * @param[in] ec The container of edges
     */
    void setTLControllingInformation(const NBEdgeCont& ec) const;

    /** @brief Replaces occurences of the removed edge in incoming/outgoing edges of all definitions
     * @param[in] removed The removed edge
     * @param[in] incoming The edges to use instead if an incoming edge was removed
     * @param[in] outgoing The edges to use instead if an outgoing edge was removed
     */
    void remapRemoved(NBEdge* removed,
                      const EdgeVector& incoming, const EdgeVector& outgoing);


    /** @brief Replaces a removed edge/lane
     * @param[in] removed The edge to replace
     * @param[in] removedLane The lane of this edge to replace
     * @param[in] by The edge to insert instead
     * @param[in] byLane This edge's lane to insert instead
     */
    void replaceRemoved(NBEdge* removed, int removedLane,
                        NBEdge* by, int byLane);

    /// @brief patches signal plans by modifying lane indices
    void shiftTLConnectionLaneIndex(NBEdge* edge, int offset);

    /** @brief Adds a phase to the logic
     * the new phase is inserted at the end of the list of already added phases
     * @param[in] duration The duration of the phase to add in SECONDS!
     * @param[in] state The state definition of a tls phase
     */
    void addPhase(SUMOTime duration, const std::string& state);


    /** @brief Adds a connection and immediately informs the edges
     */
    void addConnection(NBEdge* from, NBEdge* to, int fromLane, int toLane, int linkIndex);


    /** @brief removes the given connection from the traffic light
     * if recontruct=true, reconstructs the logic and informs the edges for immediate use in NETEDIT
     * @note: tlIndex is not necessarily unique. we need the whole connection data here
     */
    void removeConnection(const NBConnection& conn, bool reconstruct = true);

    /** @brief Returns the internal logic
     */
    NBTrafficLightLogic* getLogic() {
        return myTLLogic;
    }

    /** @brief Sets the offset of this tls
     * @param[in] offset The offset of this cycle
     */
    void setOffset(SUMOTime offset);

protected:
    /** @brief Collects the links participating in this traffic light
     *    (only if not previously loaded)
     */
    void collectLinks();


    /** @brief Computes the traffic light logic finally in dependence to the type
     * @param[in] ec The edge container
     * @param[in] brakingTime Duration a vehicle needs for braking in front of the tls
     * @return The computed logic
     */
    NBTrafficLightLogic* myCompute(const NBEdgeCont& ec,
                                   unsigned int brakingTime);

    bool amInvalid() const;

private:

    /** @brief phases are added directly to myTLLogic which is then returned in myCompute() */
    NBTrafficLightLogic* myTLLogic;

    /// @brief The original nodes for which the loaded logic is valid
    std::set<NBNode*> myOriginalNodes;

    /** @brief Informs edges about being controlled by a tls */
    void setTLControllingInformation() const;

    /// @brief repair the plan if controlled nodes received pedestrian crossings
    void patchIfCrossingsAdded();

    /// @brief set of edges with shifted lane indices (to avoid shifting twice)
    std::set<NBEdge*> myShifted;

private:
    /// @brief class for identifying connections
    class connection_equal {
    public:
        /// constructor
        connection_equal(const NBConnection& c) : myC(c) {}

        bool operator()(const NBConnection& c) const {
            return c.getFrom() == myC.getFrom() && c.getTo() == myC.getTo() &&
                   c.getFromLane() == myC.getFromLane() && c.getToLane() == myC.getToLane();
        }
    private:
        const NBConnection& myC;
    private:
        /// @brief invalidated assignment operator
        connection_equal& operator=(const connection_equal& s);

    };

};


#endif

/****************************************************************************/


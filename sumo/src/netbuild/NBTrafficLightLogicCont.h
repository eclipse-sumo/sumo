/****************************************************************************/
/// @file    NBTrafficLightLogicCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for traffic light definitions and built programs
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBTrafficLightLogicCont_h
#define NBTrafficLightLogicCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include "NBTrafficLightLogicVector.h"
#include "NBTrafficLightDefinition.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class NBEdgeCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBTrafficLightLogicCont
 * @brief A container for traffic light definitions and built programs
 *
 * This container class holds definitions of traffic light logics during
 *  the loading of the network. After all information has been loaded, these
 *  definitions are used to build the traffic light logics.
 *
 * The built traffic light logics are kept stored within this container during 
 *  their building and written to the network file at the end.
 *
 * @see NBTrafficLightDefinition
 * @see NBTrafficLightLogicVector
 */
class NBTrafficLightLogicCont
{
public:
    /// @brief Constructor
    NBTrafficLightLogicCont() throw();

    /// @brief Destructor
    ~NBTrafficLightLogicCont() throw();


    /** @brief Initialises the storage by applying given options
     *
     * Options, mainly setting offsets, are parsed
     *  and the according internal variables are set.
     *
     * @param[in] oc The options container to read options from
     * @todo Recheck exceptions
     */
    void applyOptions(OptionsCont &oc);


    /** @brief Adds a logic definition to the dictionary
     *
     * "true" is returned if the logic is accepted - no logic with the same
     *  name exists within this container. If another logic with the same name 
     *  exists, false is returned.
     *
     * @param[in] logic The logic to add
     * @return Whether the logic was valid (no logic with the same id is already known)
     */
    bool insert(NBTrafficLightDefinition *logic) throw();


    /** @brief Computes the traffic light logics using the stored definitions and stores the results
     * 
     * Goes through all stored definitions and calls "NBTrafficLightDefinition::compute"
     *  for each. Stores the result using "insert".
     *
     * @param[in] ec The edge container used during the computation
     * @param[in] oc Options used during the computation
     * @see NBTrafficLightDefinition::compute
     */
    void computeLogics(NBEdgeCont &ec, OptionsCont &oc) throw();


    /** @brief Saves all known logics into the given stream
     *
     * Calls "writeXML" for each of the stored NBTrafficLightLogicVector.
     *
     * @param[in] into The stream to write the logics into
     * @exception IOError (not yet implemented)
     * @see NBTrafficLightLogicVector::writeXML
     */
    void writeXML(OutputDevice &into) throw(IOError);


    /** @brief Destroys all stored definitions and logics
     */
    void clear() throw();


    /** @brief Replaces occurences of the removed edge in incoming/outgoing edges of all definitions
     *
     * @param[in] removed The removed edge
     * @param[in] incoming The edges to use instead if an incoming edge was removed
     * @param[in] outgoing The edges to use instead if an outgoing edge was removed
     * @todo Recheck usage
     */
    void remapRemoved(NBEdge *removed,
                      const EdgeVector &incoming, const EdgeVector &outgoing) throw();


    /** @brief Replaces occurences of the removed edge/lane in all definitions by the given edge
     *
     * @param[in] removed The removed edge
     * @param[in] removed The removed lane
     * @param[in] by The edge to use instead
     * @param[in] byLane The lane to use instead
     * @todo Recheck usage
     */
    void replaceRemoved(NBEdge *removed, int removedLane,
                        NBEdge *by, int byLane) throw();


    /** @brief Returns the named definition
     *
     * @param[in] id The id of the definition to return
     * @return The named definition, 0 if it is not known
     */
    NBTrafficLightDefinition *getDefinition(const std::string &id) const throw();


    /** @brief Informs the edges about being controlled by a tls
     *
     * Goes through all definition, calling eachs "setParticipantsInformation" method.
     * Goes through all definition, calling eachs "setTLControllingInformation" method.
     *
     * @param[in] ec The ede control to set information into
     * @see NBTrafficLightDefinition::setParticipantsInformation
     * @see NBTrafficLightDefinition::setTLControllingInformation
     */
    void setTLControllingInformation(const NBEdgeCont &ec) throw();


private:
    /** @brief Inserts a built logic into the container
     */
    bool insert(const std::string &id,
                NBTrafficLightLogicVector *logics) throw();


private:
    /// @brief Definition of the container type for tl-ids to previously computed logics
    typedef std::map<std::string, NBTrafficLightLogicVector*> ComputedContType;

    /// @brief The container for previously computed tl-logics
    ComputedContType myComputed;

    /// @brief Definition of the container type for tl-ids to their definitions
    typedef std::map<std::string, NBTrafficLightDefinition*> DefinitionContType;

    /// @brief The container for tl-ids to their definitions
    DefinitionContType myDefinitions;

    /// @brief List of tls which shall have an offset of T/2
    std::vector<std::string> myHalfOffsetTLS;

    /// @brief List of tls which shall have an offset of T/2
    std::vector<std::string> myQuarterOffsetTLS;


};


#endif

/****************************************************************************/


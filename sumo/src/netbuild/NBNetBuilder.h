/****************************************************************************/
/// @file    NBNetBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Instance responsible for building networks
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBNetBuilder_h
#define NBNetBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include "NBEdgeCont.h"
#include "NBTypeCont.h"
#include "NBNodeCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBJunctionLogicCont.h"
#include "NBDistrictCont.h"
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OptionsCont;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBNetBuilder
 * @brief Instance responsible for building networks
 */
class NBNetBuilder {
public:
    /// @brief Constructor
    NBNetBuilder() throw();

    /// @brief Destructor
    ~NBNetBuilder() throw();


    /** @brief Initialises the storage by applying given options
     *
     * Options, mainly steering the acceptance of edges, are parsed
     *  and the according internal variables are set.
     *
     * @param[in] oc The options container to read options from
     * @exception ProcessError If something fails (message is included)
     */
    void applyOptions(OptionsCont &oc) throw(ProcessError);


    /** @brief Builds a network using the loaded structures and saves it
     *
     * @exception IOError (not yet implemented)
     */
    void buildLoaded() throw(IOError);



    /// @name Retrieval of subcontainers
    /// @{

    /** @brief Returns the edge container
     * @return The edge container (reference)
     */
    NBEdgeCont &getEdgeCont() throw() {
        return myEdgeCont;
    }


    /** @brief Returns the node container
     * @return The node container (reference)
     */
    NBNodeCont &getNodeCont() throw() {
        return myNodeCont;
    }


    /** @brief Returns the type container
     * @return The type container (reference)
     */
    NBTypeCont &getTypeCont() throw() {
        return myTypeCont;
    }


    /** @brief Returns the traffic light logics container
     * @return The traffic light logics container (reference)
     */
    NBTrafficLightLogicCont &getTLLogicCont() throw() {
        return myTLLCont;
    }


    /** @brief Returns the junction logics container
     * @return The junction logics container (reference)
     */
    NBJunctionLogicCont &getJunctionLogicCont() throw() {
        return myJunctionLogicCont;
    }


    /** @brief Returns the districts container
     * @return The districts container (reference)
     */
    NBDistrictCont &getDistrictCont() throw() {
        return myDistrictCont;
    }
    /// @}



    /** @brief Adds net building options into the given options container
     *
     * @param[in] oc The options container to add net building options to
     */
    static void insertNetBuildOptions(OptionsCont &oc);


protected:
    /**
     * @brief Performs the network building steps
     *
     * Performs subsequently the building steps.
     *
     * @param[in] oc Container that contains options for building
     * @exception ProcessError (recheck)
     */
    void compute(OptionsCont &oc) throw(ProcessError);


    /** @brief Writes information about the currently processed step
     *
     * @param[in, out] step The current building step (incremented)
     * @param[in] Brief description about what is done in this step
     */
    void inform(int &step, const std::string &about) throw();



    /// @name Output methods
    /// @{

    /** @brief Saves the generated network
     *
     * Instances the network is made of are written by called methods from
     *  sub-containers.
     *
     * @param[in] device The device to write the network into
     * @param[in] oc Options needed to know what shall be saved (mainly whether internal links shall be saved)
     * @exception IOError (not yet implemented)
     * @see NBDistrictCont::writeXML
     * @see NBEdgeCont::writeXMLStep1
     * @see NBJunctionLogicCont::writeXML
     * @see NBTrafficLightLogicCont::writeXML
     * @see NBNodeCont::writeXML
     * @see NBNodeCont::writeXMLInternalNodes
     * @see NBEdgeCont::writeXMLStep2
     * @see NBNodeCont::writeXMLInternalSuccInfos
     */
    void save(OutputDevice &device, OptionsCont &oc) throw(IOError);

    /// @}


protected:
    /// @brief The used container for edges
    NBEdgeCont myEdgeCont;

    /// @brief The used container for nodes
    NBNodeCont myNodeCont;

    /// @brief The used container for street types
    NBTypeCont myTypeCont;

    /// @brief The used container for traffic light logics
    NBTrafficLightLogicCont myTLLCont;

    /// @brief The used container for junction logics
    NBJunctionLogicCont myJunctionLogicCont;

    /// @brief The used container for districts
    NBDistrictCont myDistrictCont;


private:
    /// @brief invalidated copy constructor
    NBNetBuilder(const NBNetBuilder &s);

    /// @brief invalidated assignment operator
    NBNetBuilder &operator=(const NBNetBuilder &s);

};


#endif

/****************************************************************************/


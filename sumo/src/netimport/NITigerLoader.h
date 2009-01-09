/****************************************************************************/
/// @file    NITigerLoader.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 29 Jun 2004
/// @version $Id$
///
// Importer for TIGER networks
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
#ifndef NITigerLoader_h
#define NITigerLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <utils/geom/Position2DVector.h>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class declaration
// ===========================================================================
class OptionsCont;
class NBNode;


// ===========================================================================
// class declaration
// ===========================================================================
/**
 * @class NITigerLoader
 * @brief Importer for TIGER networks
 */
class NITigerLoader
{
public:
    /** @brief Constructor
     * @param[in, filled] ec The edge control to insert loaded edges into
     * @param[in, filled] nc The node control to insert loaded nodes into
     * @param[in] file The name of the parsed file
     * @param[in] tryIgnoreNodePositions Whether node positions shall not be added to the geometry
     */
    NITigerLoader(NBEdgeCont &ec, NBNodeCont &nc,
                  const std::string &file,
                  bool tryIgnoreNodePositions) throw();


    /// @brief Destructor
    ~NITigerLoader() throw();

    /** @brief Parses the description
     * @exception ProcessError if somethng fails
     */
    void load(OptionsCont &options) throw(ProcessError);

protected:
    /** @brief Converts the given shape
     * @param[in] sv The string that describes the shape
     * @return The converted shape
     * @exception ProcessError If the description is malicious
     */
    Position2DVector convertShape(const std::vector<std::string> &sv) throw(ProcessError);


    /** @brief Returns the node at the given position
     *
     * Tries to get and return a previously built node that is on the given position.
     * If no such node exists, builds one at the position, tries
     *  to add it into the node container and returns it. If the node
     *  could not been added, a ProcessError is throw.
     * @param[in] p The position of the node
     * @return The (previously) built node at this position
     * @exception ProcessError if a node was built but could not been added
     */
    NBNode *getNode(const Position2D &p) const throw(ProcessError);


    /** @brief Returns the type decribed by the given string
     * @param[in] sv The string that described the type
     * @return The converted type
     * @exception ProcessError if the type could not been determined
     */
    std::string getType(const std::vector<std::string> &sv) const throw(ProcessError);


    /** @brief Returns the speed for the given type
     * @param[in] type The type of the edge
     * @return The speed resulting from the type
     * @exception ProcessError if the type is not known or no speed is associated
     */
    SUMOReal getSpeed(const std::string &type) const throw(ProcessError);


    /** @brief Returns the lane number for the given type
     * @param[in] type The type of the edge
     * @return The lane number resulting from the type
     * @exception ProcessError if the type is not known or no lane number is associated
     */
    int getLaneNo(const std::string &type) const throw(ProcessError);


protected:
    /// @brief The name of the parsed file
    std::string myFileName;

    /// @brief The name of the parsed file
    bool myWasSet;

    /// @brief The edge container to fill
    NBEdgeCont &myEdgeCont;

    /// @brief The node container to fill
    NBNodeCont &myNodeCont;

    /// @brief Whether node positions shall not be added to the edge's geometry
    bool myTryIgnoreNodePositions;

    /// @brief Running index for node id computation
    mutable int myRunningNodeIndex;


private:
    /// @brief Invalidated copy constructor.
    NITigerLoader(const NITigerLoader&);

    /// @brief Invalidated assignment operator.
    NITigerLoader& operator=(const NITigerLoader&);

};


#endif

/****************************************************************************/


/****************************************************************************/
/// @file    NIVissimDistrictConnection.h
/// @author  Daniel Krajzewicz
/// @date    End of 2002
/// @version $Id$
///
// An edge imported from Vissim together for a container for
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
#ifndef NIVissimDistrictConnection_h
#define NIVissimDistrictConnection_h


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
#include <utils/geom/Position2D.h>
#include <utils/common/VectorHelper.h>


class NBDistrictCont;
class NBEdgeCont;


// ===========================================================================
// class definitions
// ===========================================================================
class NIVissimDistrictConnection {
public:
    /// Contructor
    NIVissimDistrictConnection(int id, const std::string &name,
                               const IntVector &districts, const DoubleVector &percentages,
                               int edgeid, SUMOReal position,
                               const std::vector<std::pair<int, int> > &assignedVehicles);

    // Destructor
    ~NIVissimDistrictConnection();

    /** @brief Returns the position
        The position yields from the edge geometry and the place the connection is plaed at */
    Position2D geomPosition() const;

    /// Returns the id of the connection
    int getID() const {
        return myID;
    }

    /// Returns the position of the connection at the edge
    SUMOReal getPosition() const {
        return myPosition;
    }

    SUMOReal getMeanSpeed(/*NBDistribution &dc*/) const;

public:
    /// Inserts the connection into the dictionary after building it
    static bool dictionary(int id, const std::string &name,
                           const IntVector &districts, const DoubleVector &percentages,
                           int edgeid, SUMOReal position,
                           const std::vector<std::pair<int, int> > &assignedVehicles);

    /// Inserts the build connection to the dictionary
    static bool dictionary(int id, NIVissimDistrictConnection *o);

    /// Returns the named dictionary
    static NIVissimDistrictConnection *dictionary(int id);

    /// Builds the nodes that belong to a district
    static void dict_BuildDistrictNodes(NBDistrictCont &dc,
                                        NBNodeCont &nc);

    /// Builds the districts
    static void dict_BuildDistricts(NBDistrictCont &dc,
                                    NBEdgeCont &ec, NBNodeCont &nc/*, NBDistribution &distc*/);

    /** @brief Returns the connection to a district placed at the given node
        Yep, there onyl should be one, there is no need to build a single edge as connection between two parking places */
    static NIVissimDistrictConnection *dict_findForEdge(int edgeid);

    /// Clears the dictionary
    static void clearDict();

    static void dict_BuildDistrictConnections();

    static void dict_CheckEdgeEnds();


private:
    void checkEdgeEnd();
    SUMOReal getRealSpeed(/*NBDistribution &dc, */int distNo) const;

private:
    /// The id of the connections
    int myID;

    /// The name of the connections
    std::string myName;

    /// The connected districts
    IntVector myDistricts;

    /// Definition of a map of how many vehicles should leave to a certain district
    typedef std::map<int, SUMOReal> DistrictPercentages;

    /// A map how many vehicles (key, amount) should leave to a district (key)
    DistrictPercentages myPercentages;

    /// The id of the connected edge
    int myEdgeID;

    /// The position on the edge
    SUMOReal myPosition;

    /// The vehicles using this connection
    std::vector<std::pair<int, int> > myAssignedVehicles;

private:
    /// Definition of a dictionary of district connections
    typedef std::map<int, NIVissimDistrictConnection*> DictType;

    /// District connection dictionary
    static DictType myDict;

    /// Map from ditricts to connections
    static std::map<int, IntVector> myDistrictsConnections;

};


#endif

/****************************************************************************/


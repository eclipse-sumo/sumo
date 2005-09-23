#ifndef NIVissimDistrictConnection_h
#define NIVissimDistrictConnection_h
/***************************************************************************
                          NIVissimDistrictConnection.h
			  An edge imported from Vissim together for a container for
              these instances
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : End of 2002
    copyright            : (C) 2002 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.9  2005/09/23 06:02:57  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.7  2004/01/28 12:39:23  dkrajzew
// work on reading and setting speeds in vissim-networks
//
// Revision 1.6  2003/10/27 10:51:55  dkrajzew
// edges speed setting implemented (only on an edges begin)
//
// Revision 1.5  2003/09/23 14:16:37  dkrajzew
// further work on vissim-import
//
// Revision 1.4  2003/03/31 06:15:49  dkrajzew
// further work on vissim-import
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <map>
#include <string>
#include <utils/geom/Position2D.h>
#include <utils/common/IntVector.h>
#include <utils/common/DoubleVector.h>


class NBDistrictCont;
class NBEdgeCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
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

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

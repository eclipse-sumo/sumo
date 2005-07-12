#ifndef NBJoinedEdgesMap_h
#define NBJoinedEdgesMap_h
//                        NBJoinedEdgesMap.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.3  2005/07/12 12:32:47  dkrajzew
// code style adapted; guessing of ramps and unregulated near districts implemented; debugging
//
// Revision 1.4  2005/07/04 12:07:40  dksumo
// output of lengths added to map output
//
// Revision 1.3  2005/05/30 08:17:45  dksumo
// comments added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <vector>
#include <map>
#include <string>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBEdgeCont;


/* =========================================================================
 * class definitions
 * ======================================================================= */
class NBJoinedEdgesMap {
public:
	/// Constructor
    NBJoinedEdgesMap();

	/// Destructor
    ~NBJoinedEdgesMap();

    /// Initialises the map using the list of edge names
    void init(NBEdgeCont &ec);

    /// Informs the map that two edges have been joined
    void appended(const std::string &to, const std::string &what);

    /// Outputs the list of joined edges
    friend std::ostream &operator<<(std::ostream &os,
        const NBJoinedEdgesMap &map);

private:
    /// def. of a list of edges that make up an edge
    typedef std::vector<std::string> MappedEdgesVector;

    /// def. of a map of edge names to lists of previous edges the current edge is made of
    typedef std::map<std::string, MappedEdgesVector> JoinedEdgesMap;

    /// Map of edge names to lists of previous edges the current edge is made of
    JoinedEdgesMap myMap;

    std::map<std::string, float> myLengths;

};


extern NBJoinedEdgesMap gJoinedEdges;


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

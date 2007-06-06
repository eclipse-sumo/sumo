/****************************************************************************/
/// @file    MapEdges.h
/// @author  unknown_author
/// @date    Jun 2005
/// @version $Id$
///
// A tool application to map between edges from two different networks
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
#ifndef MapEdges_h
#define MapEdges_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utils/geom/Position2DVector.h>


// ===========================================================================
// class definitions
// ===========================================================================

class MapEdges
{


public:
    /// Constructor
    MapEdges();

    /// Constructor
    MapEdges(const char *netA, const char *netB);

    /// Destructor
    ~MapEdges();

    class Junction
    {
    public:
        explicit Junction(const std::string nid)
        {
            id = nid;
        }
        ~Junction();
        std::string id;
        Position2D pos;
    };


    /// load net-file and save the Junctions into a dictionnary 1 or 2
    void load(void);
    void loadNet(const char *net, int dic);

    /// set the junctions names
    void setJunctionA(std::string a, std::string b, std::string c);
    void setJunctionB(std::string a, std::string b, std::string c);

    // convert junction's positions in new positions
    void convertA(void);
    void convertB(void);

    void result(const std::string &file);

protected:
    /// the Name of the net-file-A
    const char *net_a;

    /// the Name of the net-file-B
    const char *net_b;

    /// Junctions name NetA
    std::string juncA1;
    std::string juncA2;
    std::string juncA3;

    /// Junction name NetB
    std::string juncB1;
    std::string juncB2;
    std::string juncB3;


    /// Definition of the dictionary type
    typedef std::map<std::string, Junction*> DictTypeJunction;

    /// The dictionaries
    static DictTypeJunction myJunctionDictA;
    static DictTypeJunction myJunctionDictB;

    static std::map<std::string, std::string> myEdge2JunctionAMap;
    static std::map<std::string, std::string> myEdge2JunctionBMap;

};


#endif

/****************************************************************************/


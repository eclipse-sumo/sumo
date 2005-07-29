#ifndef Intersection_h
#define Intersection_h
//---------------------------------------------------------------------------//
//                        Intersection.h -
//  
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Jun 2005
//  copyright            : (C) 2005 by Danilo Boyom
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : danilo.tete-boyom@dlr.com
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log:  $

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream> 
#include <fstream> 
#include <string> 
#include <vector>
#include <map>
#include <utils/geom/Position2DVector.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */

class Intersection  
{
  

public:
	 /// Constructor
    Intersection();

    /// Constructor
	Intersection(const char *net, const char *polygons);

    /// Destructor
    ~Intersection();

	class Lane{
	public:
		explicit Lane(const std::string nid){
			id = nid;
		}
		~Lane();
		Position2DVector posi;
		std::string id;
		int length;
	};
	class Polygon{
	public:
		explicit Polygon(const std::string nid){
			id = nid;
		}
		~Polygon();
		Position2DVector posi;
		std::string id;
	};
   
    /// load net-file and save the Position intos a dictionnary
    void loadNet();

    /// load polygon-file file and save the Positions into a dictionnary
	void loadPolygon();

    // compare all value to find the intersection point
	// write results into a file 
    void compare(void);

	// gibt wie oft ein char in ein string vorkommt
    int getNumberOf(std::string str);

	public:

    /// the Name of the net-file
    const char *net;

    /// the type of the polygon-file
    const char *polygons;

	/// Definition of the dictionary type
    typedef std::map<std::string, Lane*> DictTypeLane;
	typedef std::map<std::string, Polygon*> DictTypePolygon;

    /// The dictionary
    static DictTypeLane myLaneDict;
    static DictTypePolygon myPolyDict;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

#ifndef NetCompare_h
#define NetCompare_h
//---------------------------------------------------------------------------//
//                        NetCompare.h -
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

class NetCompare  
{
  

public:
	 /// Constructor
    NetCompare();

    /// Constructor
	NetCompare(const char *netA, const char *netB );

    /// Destructor
    ~NetCompare();

	class Junction{
	public:
		explicit Junction(const std::string nid){
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

	void result(const char *output);

	// return the min or max values
	double minValue(double a, double b, double c);
	double maxValue(double a, double b, double c);

	public:

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

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

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
namespace
{
    const char rcsid[] =
    "$Id: $";
}
// $Log: $
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "Intersection.h"
#include <iostream> 
#include <fstream> 
#include <string> 
#include <map>
#include <stdlib.h>
#include <direct.h>
#include <utils/geom/Position2DVector.h>

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

Intersection::DictTypeLane Intersection::myLaneDict;
Intersection::DictTypePolygon Intersection::myPolyDict;

/* =========================================================================
 * member definitions
 * ======================================================================= */
//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

Intersection::Intersection(const char *nnet, const char *npolygons) 
        : net(nnet),  polygons(npolygons)
{
}

Intersection::~Intersection()
{    
}

//////////////////////////////////////////////////////////////////////
// Definitions of the Methods
//////////////////////////////////////////////////////////////////////

/// load net-file and save the Position into a dictionnary
void
Intersection::loadNet(void)
{   
	char buffer[_MAX_PATH];
	getcwd(buffer,_MAX_PATH);
	cout<<"Current directory is "<<buffer<<endl;

	ifstream out(net);
	
    if (!out) { 
      cerr << "cannot open file: " << net <<endl; 
      exit(-1); 
	} 

	std::string buff;
    cout<<"=======================first loading Lanes==============================="<<endl;
	int l = 0;
	while(!out.eof()) {
		getline(out,buff);
		if(buff.find("<lane id=")!=string::npos){
		l = l + 1;
		std::string id = buff.substr(buff.find("=")+2,buff.find(" d")-buff.find("=")-3);
		Intersection::Lane *lane = new Lane(id);
       	std::string rest = buff.substr(buff.find(">")+1,buff.find("</")-buff.find(">")-1);
        cout<<l<<". Lane ID = "<<id<<endl;
        int j = getNumberOf(rest);
        
		for(int k = 0; k<j+1; k++){
		    std::string  pos1 = rest.substr(0,rest.find(","));
		    std::string  pos2 = rest.substr(rest.find(",")+1,rest.find(" ")-rest.find(","));
		    rest = rest.substr(rest.find(" ")+1,rest.length());
			Position2D  *pos = new Position2D(atoi(pos1.c_str()),atoi(pos2.c_str()));
            lane->posi.push_back(*pos);
			cout <<"       Position: x = "<<pos1<<" y = "<<pos2<<endl;
			}
        myLaneDict[id] = lane;
		}

	}
	out.close();
}

/// load polygon-file file and save the Positions into a dictionnary
void
Intersection::loadPolygon(void)
{  
	char buffer[_MAX_PATH];
	getcwd(buffer,_MAX_PATH);

	ifstream out(polygons);
	
    if (!out) { 
      cerr << "cannot open file: " << net <<endl; 
      exit(-1); 
	} 

	std::string buff;
	int l = 0;
    cout<<endl<<"========================second loading Polygons============================="<<endl;	
	while(!out.eof()) {
		getline(out,buff);
		if(buff.find("<poly name=")!=string::npos){
			l = l +1 ;
		std::string id = buff.substr(buff.find("=")+2,buff.find(" t")-buff.find("=")-3);
		Intersection::Polygon *poly = new Polygon(id);
       	std::string rest = buff.substr(buff.find(">")+1,buff.find("</")-buff.find(">")-1);
        cout<<l<<". Polygon ID = "<<id<<endl;
        int j = getNumberOf(rest);
        
		for(int k = 0; k<j+1; k++){
		    std::string  pos1 = rest.substr(0,rest.find(","));
		    std::string  pos2 = rest.substr(rest.find(",")+1,rest.find(" ")-rest.find(","));
		    rest = rest.substr(rest.find(" ")+1,rest.length());
			const Position2D  *pos = new Position2D(atoi(pos1.c_str()),atoi(pos2.c_str()));
            poly->posi.push_back(*pos);
			cout <<"       Position: x = "<<pos1<<" y = "<<pos2<<endl;
			}
        // !!! patch due to false implementation of intersectsAtLengths!!!
        poly->posi.push_back(poly->posi.at(0));
        myPolyDict[id] = poly;
		}

	}
	out.close();
}

/// compare all value to find the intersection point
/// write results in a file 
void
Intersection::compare(void)
{
   cout<<endl<<"======================results========================================="<<endl;
   for(DictTypePolygon::iterator i=myPolyDict.begin(); i!=myPolyDict.end(); i++) {
	    for(DictTypeLane::iterator j=myLaneDict.begin(); j!=myLaneDict.end(); j++) {
			if(((*j).second->posi).intersects((*i).second->posi)){
				cout<<"  Schnittpunkt zwischen Polygon "<<(*i).second->id<<" und Lane "<<(*j).second->id <<endl;
                DoubleVector intersectionPositions = 
                    (*j).second->posi.intersectsAtLengths((*i).second->posi);
                for(DoubleVector::iterator k=intersectionPositions.begin(); k!=intersectionPositions.end(); ++k) {
                    Position2D pos = (*j).second->posi.positionAtLengthPosition(*k);
                    cout << "     At lane pos " << (*k) << " (" << pos.x() << ", " << pos.y() << ")" << endl;
                }
			}
		}
    }



}
// gibt wie oft ein char in einr string vorkommt
int 
Intersection::getNumberOf(std::string str){
    int i,j = 0; 
	for(i = 0; i < str.length(); i++){
        if (str.at(i)==' '){
			j = j + 1;
		}
	}
	return j;

}

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int main(int argc, char** argv) 
{ 
	/*
	if (argc<3) { 
    cerr << "Bitte einen Dateinamen als" 
         << " Argument angeben!" << endl; 
      return -1; 
    }
	*/

	Intersection *app = new Intersection("due2s.net.xml","poly.xml");//argv[1],argv[2]);
	app->loadNet();
	app->loadPolygon();
	app->compare();
	return 0;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
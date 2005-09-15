//---------------------------------------------------------------------------//
//                        IncludedRoutes.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : July 2005
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
    "$Id$";
}
// $Log$
// Revision 1.1  2005/09/15 12:09:27  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2005/09/09 12:53:16  dksumo
// tools added
//
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "IncludedRoutes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stdlib.h>
#include <direct.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

IncludedRoutes::DictTypeRoute IncludedRoutes::myRouteDict1;
IncludedRoutes::DictTypeRoute IncludedRoutes::myRouteDict2;

/* =========================================================================
 * member definitions
 * ======================================================================= */
//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

IncludedRoutes::IncludedRoutes(const char *route1, const char *route2)
        : route_1(route1),  route_2(route2)
{
}

IncludedRoutes::~IncludedRoutes()
{
}

//////////////////////////////////////////////////////////////////////
// Definitions of the Methods
//////////////////////////////////////////////////////////////////////

/// load route-file and save the routes into a Dictionary
void
IncludedRoutes::loadRoutes(const int a)
{
	char buffer[_MAX_PATH];
	getcwd(buffer,_MAX_PATH);
	cout<<"Current directory is "<<buffer<<endl;

    const char * datei;

	if(a == 1){
		datei = route_1;
	}else{
		datei = route_2;
	}

	ifstream out(datei);

    if (!out) {
      cerr << "cannot open file: " << datei <<endl;
      exit(-1);
	}

	std::string buff;
    cout<<"=======================first loading Routes==============================="<<endl;
	int l = 0;
	while(!out.eof()) {
		getline(out,buff);
		if(buff.find("<route id=")!=string::npos){
			l = l + 1;
			std::string id = buff.substr(buff.find("=")+2,buff.find(">")-buff.find("=")-3);

       		std::string rest = buff.substr(buff.find(">")+1,buff.find("</")-buff.find(">")-1);
			cout<<l<<". route ID = "<<id<<endl;
			cout <<"        "<<rest<<endl;
			if(a == 1){
				myRouteDict1[id] = rest;
			}else{
				myRouteDict2[id] = rest;
			}
		}

	}
	out.close();
}



/// compare all value to find the IncludedRoutes point
/// write results in a file
void
IncludedRoutes::result(const char *output)
{
   cout<<endl<<"======================results========================================="<<endl;
   ofstream out(output);
   for(DictTypeRoute::iterator i=myRouteDict1.begin(); i!=myRouteDict1.end(); i++) {
	    std::string in = "Route1(" + (*i).first +")";
	    for(DictTypeRoute::iterator j=myRouteDict2.begin(); j!=myRouteDict2.end(); j++) {
			if((*j).second.find((*i).second)!=string::npos){
				in = in + ":Route2("+(*j).first +")";
			}
		}
      out<<in<<endl;
	  cout<<in<<endl;
    }
   out.close();

}

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int main(int argc, char** argv)
{

	if (argc!=4) {
    cerr << " syntax error please use:"
		 << " IncludedRoutes <route-file1> <route-file2> <output-file> "<< endl;
      return -1;
    }


	IncludedRoutes *app = new IncludedRoutes(argv[1],argv[2]);
	app->loadRoutes(1);
	app->loadRoutes(2);
	app->result(argv[3]);
	return 0;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

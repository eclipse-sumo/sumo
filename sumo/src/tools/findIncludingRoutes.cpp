/****************************************************************************/
/// @file    findIncludingRoutes.cpp
/// @author  unknown_author
/// @date    July 2005
/// @version $Id$
///
// }
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
// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "IncludedRoutes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stdlib.h>
#include <direct.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

IncludedRoutes::DictTypeRoute IncludedRoutes::myRouteDict1;
IncludedRoutes::DictTypeRoute IncludedRoutes::myRouteDict2;

// ===========================================================================
// member definitions
// ===========================================================================
//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

IncludedRoutes::IncludedRoutes(const char *route1, const char *route2)
        : route_1(route1),  route_2(route2)
{}

IncludedRoutes::~IncludedRoutes()
{}

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

    if (a == 1) {
        datei = route_1;
    } else {
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
    while (!out.eof()) {
        getline(out,buff);
        if (buff.find("<route id=")!=string::npos) {
            l = l + 1;
            std::string id = buff.substr(buff.find("=")+2,buff.find(">")-buff.find("=")-3);

            std::string rest = buff.substr(buff.find(">")+1,buff.find("</")-buff.find(">")-1);
            cout<<l<<". route ID = "<<id<<endl;
            cout <<"        "<<rest<<endl;
            if (a == 1) {
                myRouteDict1[id] = rest;
            } else {
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
    for (DictTypeRoute::iterator i=myRouteDict1.begin(); i!=myRouteDict1.end(); i++) {
        std::string in = "Route1(" + (*i).first +")";
        for (DictTypeRoute::iterator j=myRouteDict2.begin(); j!=myRouteDict2.end(); j++) {
            if ((*j).second.find((*i).second)!=string::npos) {
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



/****************************************************************************/


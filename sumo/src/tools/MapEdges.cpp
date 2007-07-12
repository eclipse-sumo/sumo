/****************************************************************************/
/// @file    MapEdges.cpp
/// @author  unknown_author
/// @date    Jun 2005
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MapEdges.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stdlib.h>
#include <direct.h>
#include <math.h>
#include <utils/common/StdDefs.h>
#include <utils/importio/LineReader.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static variables
// ===========================================================================
MapEdges::DictTypeJunction MapEdges::myJunctionDictA;
MapEdges::DictTypeJunction MapEdges::myJunctionDictB;
std::map<std::string, std::string> MapEdges::myEdge2JunctionAMap;
std::map<std::string, std::string> MapEdges::myEdge2JunctionBMap;

std::vector<std::string> myBlacklist;


// ===========================================================================
// member definitions
// ===========================================================================
//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

MapEdges::MapEdges(const char *netA, const char *netB)
        : net_a(netA), net_b(netB)
{}


MapEdges::~MapEdges()
{}


//////////////////////////////////////////////////////////////////////
// Definitions of the Methods
//////////////////////////////////////////////////////////////////////
/// load net-file and save the Position into a dictionnary
void
MapEdges::load(void)
{
    cout << "Parsing network#1 ('" << net_a << "')..." << endl;
    loadNet(net_a,1);
    cout << "done." << endl;
    cout << "Parsing network#1 ('" << net_b << "')..." << endl;
    loadNet(net_b,2);
    cout << "done." << endl;

}


string
getAttr(string from, string attrName)
{
    size_t beg = from.find(attrName);
    beg = from.find("\"", beg);
    size_t end = from.find("\"", beg+1);
    return from.substr(beg+1, end-beg-1);
}


void
MapEdges::loadNet(const char *net, int dic)
{
    char buffer[myMAX_PATH];
    getcwd(buffer,myMAX_PATH);
    ifstream out(net);
    if (!out) {
        cerr << "cannot open file: " << net << endl;
        exit(-1);
    }

    std::string buff;
    int l = 0;
    while (!out.eof()) {
        getline(out,buff);
        if (buff.find("<junction id=")!=string::npos) {
            l = l + 1;
            std::string id = buff.substr(buff.find("=")+2,buff.find(" t")-buff.find("=")-3);
            MapEdges::Junction *junction = new Junction(id);
            std::string rest = buff.substr(buff.find("x=")+2,buff.find(">")-buff.find("x="));

            std::string  pos1 = rest.substr(1,rest.find(" ")-2);
            std::string  pos2 = rest.substr(rest.find("y=")+3,rest.find(">")-rest.find("y=")-4);

            Position2D pos(atof(pos1.c_str()),atof(pos2.c_str()));
            junction->pos = pos;

            if (dic == 1) {
                myJunctionDictA[id] = junction;
            } else {
                myJunctionDictB[id] = junction;
            }
        }
        if (buff.find("<edge id=")!=string::npos) {
            string id = getAttr(buff, "id");
            // skip edges within the blacklist
            if (find(myBlacklist.begin(), myBlacklist.end(), id)!=myBlacklist.end()) {
                continue;
            }
            // !!! no ramp-hack!!!
            if (id.find("-Added")!=string::npos) {
                continue;
            }
            string from = getAttr(buff, " From");
            if (dic == 1) {
                myEdge2JunctionAMap[id] = from;
            } else {
                myEdge2JunctionBMap[from] = id;
            }


        }

    }
    out.close();
}


void
MapEdges::setJunctionA(std::string a,std::string b,std::string c)
{
    juncA1 = a;
    juncA2 = b;
    juncA3 = c;
}

void
MapEdges::setJunctionB(std::string a,std::string b,std::string c)
{
    juncB1 = a;
    juncB2 = b;
    juncB3 = c;

}

void
MapEdges::convertA(void)
{
    cout << "Resetting positions for first network" << endl;
    DictTypeJunction::iterator i;

    i = myJunctionDictA.find(juncA1);
    if (i==myJunctionDictA.end()) {
        cerr << "Could not find junction '" << juncA1 << "'!" << endl;
        throw 1;
    }
    Junction *j1 = (*i).second;

    i = myJunctionDictA.find(juncA2);
    if (i==myJunctionDictA.end()) {
        cerr << "Could not find junction '" << juncA2 << "'!" << endl;
        throw 1;
    }
    Junction *j2 = (*i).second;

    i = myJunctionDictA.find(juncA3);
    if (i==myJunctionDictA.end()) {
        cerr << "Could not find junction '" << juncA3 << "'!" << endl;
        throw 1;
    }
    Junction *j3 = (*i).second;

    SUMOReal xmin = MIN3(j1->pos.x(),j2->pos.x(),j3->pos.x());
    SUMOReal xmax = MAX3(j1->pos.x(),j2->pos.x(),j3->pos.x());
    SUMOReal xw   = xmax - xmin ;
    cout << "first network sizes " << endl;
    cout << " (xmin, xmax, width):" <<  xmin << ", " << xmax << ", " << xw << endl;

    SUMOReal ymin = MIN3(j1->pos.y(),j2->pos.y(),j3->pos.y());
    SUMOReal ymax = MAX3(j1->pos.y(),j2->pos.y(),j3->pos.y());
    SUMOReal yw   = ymax - ymin ;
    cout << " (ymin, ymax, height):" <<  ymin << ", " << ymax << ", " << yw << endl;

    for (DictTypeJunction::iterator j=myJunctionDictA.begin(); j!=myJunctionDictA.end(); j++) {
        SUMOReal nx = ((*j).second->pos.x() -xmin)/xw;
        SUMOReal ny = ((*j).second->pos.y() -ymin)/yw;
        ((*j).second->pos).set(nx,ny);
    }
    cout << "Finished conversion the first network." << endl << endl;
}

/// compare all value to find the MapEdges point
/// write results in a file
void
MapEdges::convertB(void)
{
    cout << "Resetting positions for second network" << endl;

    DictTypeJunction::iterator i;
    i = myJunctionDictB.find(juncB1);
    if (i==myJunctionDictB.end()) {
        cout << "Could not find junction '" << juncB1 << "'!" << endl;
        throw 1;
    }
    Junction *j1 = (*i).second;

    i = myJunctionDictB.find(juncB2);
    if (i==myJunctionDictB.end()) {
        cout << "Could not find junction '" << juncB2 << "'!" << endl;
        throw 1;
    }
    Junction *j2 = (*i).second;

    i = myJunctionDictB.find(juncB3);
    if (i==myJunctionDictB.end()) {
        cout << "Could not find junction '" << juncB3 << "'!" << endl;
        throw 1;
    }
    Junction *j3 = (*i).second;

    SUMOReal xmin = MIN3(j1->pos.x(),j2->pos.x(),j3->pos.x());
    SUMOReal xmax = MAX3(j1->pos.x(),j2->pos.x(),j3->pos.x());
    SUMOReal xw   = xmax - xmin ;
    cout << "second network sizes " << endl;
    cout << " (xmin, xmax, width):" <<  xmin << ", " << xmax << ", " << xw << endl;

    SUMOReal ymin = MIN3(j1->pos.y(),j2->pos.y(),j3->pos.y());
    SUMOReal ymax = MAX3(j1->pos.y(),j2->pos.y(),j3->pos.y());
    SUMOReal yw   = ymax - ymin ;
    cout << " (ymin, ymax, height):" <<  ymin << ", " << ymax << ", " << yw << endl;

    for (DictTypeJunction::iterator j=myJunctionDictB.begin(); j!=myJunctionDictB.end(); j++) {
        SUMOReal nx = ((*j).second->pos.x() -xmin)/xw;
        SUMOReal ny = ((*j).second->pos.y() -ymin)/yw;
        ((*j).second->pos).set(nx,ny);
    }
    cout << "Finished conversion the first network." << endl << endl;
}


void
MapEdges::result(const std::string &file)
{
    ofstream out(file.c_str());
    if (!out.good()) {
        cerr << "Could not open '" << file << "'." << endl;
        return;
    }
    for (std::map<std::string, std::string>::iterator i=myEdge2JunctionAMap.begin(); i!=myEdge2JunctionAMap.end(); i++) {
        SUMOReal minAbstand = 77777;
        std::string id = "";
        string nodeID = (*i).second;
        Position2D posA = myJunctionDictA[nodeID]->pos;
        for (DictTypeJunction::iterator j=myJunctionDictB.begin(); j!=myJunctionDictB.end(); j++) {
            // skip empty nodes
            if (myEdge2JunctionBMap.find((*j).second->id)==myEdge2JunctionBMap.end()) {
                continue;
            }
            if (myEdge2JunctionBMap[(*j).second->id].size()==0) {
                continue;
            }
            //
            SUMOReal X = pow(posA.x() - (*j).second->pos.x(),2);
            SUMOReal Y = pow(posA.y() - (*j).second->pos.y(),2);
            SUMOReal nabstand = sqrt(X+Y);
            if (nabstand < minAbstand) {
                minAbstand = nabstand;
                id = myEdge2JunctionBMap[(*j).second->id];
            }
        }
        out<< (*i).first << ";" << id <<endl;
        cout <<(*i).first <<";"<< id <<endl;
    }
    out.close();
}


void
loadBlacklist(const std::string &file)
{
    LineReader lr(file);
    if (!lr.good()) {
        cerr << "Could not open blacklist ''.";
        exit(1);
    }
    while (lr.hasMore()) {
        string id = lr.readLine();
        if (id.length()>0&&id[0]!='#') {
            myBlacklist.push_back(id);
        }
    }
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int main(int argc, char** argv)
{
    if (argc<9) {
        cerr << "Syntax-Error!" << endl;
        cerr << "Syntax: MapEdges <NET_A> <NET_B> <JUNCTION_A1> <JUNCTION_A2> <JUNCTION_A3> \\"
        << endl
        << "   <JUNCTION_B1> <JUNCTION_B2> <JUNCTION_B3> <OUTPUT_FILE> \\"
        << endl
        << "   [<BLACKLIST>]" << endl;
        return -1;
    }

    if (argc==10) {
        loadBlacklist(argv[10]);
    }

    MapEdges *app = new MapEdges(argv[1], argv[2]);
    app->setJunctionA(argv[3], argv[4], argv[5]);
    app->setJunctionB(argv[6], argv[7], argv[8]);
    app->load();
    app->convertA();
    app->convertB();
    app->result(argv[9]);
    return 0;
}



/****************************************************************************/


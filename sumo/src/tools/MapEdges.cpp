//---------------------------------------------------------------------------//
//                        MapEdges.h -
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
    "$Id$";
}
// $Log$
// Revision 1.3  2005/10/07 11:42:59  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.2  2005/09/23 06:05:18  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MapEdges.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stdlib.h>
#include <direct.h>
#include <math.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

MapEdges::DictTypeJunction MapEdges::myJunctionDictA;
MapEdges::DictTypeJunction MapEdges::myJunctionDictB;
std::map<std::string, std::string> MapEdges::myEdge2JunctionAMap;
std::map<std::string, std::string> MapEdges::myEdge2JunctionBMap;

/* =========================================================================
 * member definitions
 * ======================================================================= */
//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

MapEdges::MapEdges(const char *netA, const char *netB)
        : net_a(netA), net_b(netB)
{
}

MapEdges::~MapEdges()
{
}

//////////////////////////////////////////////////////////////////////
// Definitions of the Methods
//////////////////////////////////////////////////////////////////////

/// load net-file and save the Position into a dictionnary
void
MapEdges::load(void)
{
	loadNet(net_a,1);
	loadNet(net_b,2);

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
	char buffer[_MAX_PATH];
	getcwd(buffer,_MAX_PATH);
	cout<<"Current directory is "<<buffer<<endl;

	ifstream out(net);

    if (!out) {
      cerr << "cannot open file: " << net <<endl;
      exit(-1);
	}

	std::string buff;
    cout<<endl<<"=====================Loading "<<net<< "============================="<<endl;
	int l = 0;
	while(!out.eof()) {
		getline(out,buff);
		if(buff.find("<junction id=")!=string::npos){
			l = l + 1;
			std::string id = buff.substr(buff.find("=")+2,buff.find(" t")-buff.find("=")-3);
			MapEdges::Junction *junction = new Junction(id);
       		std::string rest = buff.substr(buff.find("x=")+2,buff.find(">")-buff.find("x="));
			cout<<l<<". Junction ID = "<<id<<endl;

			std::string  pos1 = rest.substr(1,rest.find(" ")-2);
			std::string  pos2 = rest.substr(rest.find("y=")+3,rest.find(">")-rest.find("y=")-4);

			Position2D pos(atof(pos1.c_str()),atof(pos2.c_str()));
			junction->pos = pos;
			cout <<"       Position: x = "<<pos1<<" y = "<<pos2<<endl;

			if (dic == 1){
				myJunctionDictA[id] = junction;
			}else{
				myJunctionDictB[id] = junction;
			}
		}
        if(buff.find("<edge id=")!=string::npos) {
            string id = getAttr(buff, "id");
            string from = getAttr(buff, " From");
			if (dic == 1){
				myEdge2JunctionAMap[id] = from;
			}else{
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
	cout <<"==============convertA=================="<<endl;

	DictTypeJunction::iterator i;
	i = myJunctionDictA.find(juncA1);
    if(i==myJunctionDictA.end()) {
        cout << "Could not find junction '" << juncA1 << "'!" << endl;
        throw 1;
    }
	Junction *j1 = (*i).second;

	i = myJunctionDictA.find(juncA2);
    if(i==myJunctionDictA.end()) {
        cout << "Could not find junction '" << juncA2 << "'!" << endl;
        throw 1;
    }
	Junction *j2 = (*i).second;

	i = myJunctionDictA.find(juncA3);
    if(i==myJunctionDictA.end()) {
        cout << "Could not find junction '" << juncA3 << "'!" << endl;
        throw 1;
    }
	Junction *j3 = (*i).second;

	SUMOReal xmin = minValue(j1->pos.x(),j2->pos.x(),j3->pos.x());
	SUMOReal xmax = maxValue(j1->pos.x(),j2->pos.x(),j3->pos.x());
	SUMOReal xw   = xmax - xmin ;
	cout <<" xminValue "<<xmin<<endl;
	cout <<" xmaxValue "<<xmax<<endl;
    cout <<" xwidth "<<xw<<endl;

    SUMOReal ymin = minValue(j1->pos.y(),j2->pos.y(),j3->pos.y());
	SUMOReal ymax = maxValue(j1->pos.y(),j2->pos.y(),j3->pos.y());
	SUMOReal yw   = ymax - ymin ;
    cout <<" yminValue "<<ymin<<endl;
	cout <<" ymaxValue "<<ymax<<endl;
    cout <<" xwidth "<<xw<<endl;

	for(DictTypeJunction::iterator j=myJunctionDictA.begin(); j!=myJunctionDictA.end(); j++) {
		cout<<"----------------------------------------------"<<endl;
		cout <<"Junction ID = "<<(*j).second->id<<endl;
		cout <<"     alte x= "<< (*j).second->pos.x() <<" alte y= "<<(*j).second->pos.y()<<endl;
		SUMOReal nx = ((*j).second->pos.x() -xmin)/xw;
		SUMOReal ny = ((*j).second->pos.y() -ymin)/yw;
        cout <<"     neue x = "<< nx <<" neue y = "<< ny<<endl;
        ((*j).second->pos).set(nx,ny);
	}


}

/// compare all value to find the MapEdges point
/// write results in a file
void
MapEdges::convertB(void)
{
	cout <<"===============convertB=================="<<endl;

	DictTypeJunction::iterator i;
	i = myJunctionDictB.find(juncB1);
    if(i==myJunctionDictB.end()) {
        cout << "Could not find junction '" << juncB1 << "'!" << endl;
        throw 1;
    }
	Junction *j1 = (*i).second;

	i = myJunctionDictB.find(juncB2);
    if(i==myJunctionDictB.end()) {
        cout << "Could not find junction '" << juncB2 << "'!" << endl;
        throw 1;
    }
	Junction *j2 = (*i).second;

	i = myJunctionDictB.find(juncB3);
    if(i==myJunctionDictB.end()) {
        cout << "Could not find junction '" << juncB3 << "'!" << endl;
        throw 1;
    }
	Junction *j3 = (*i).second;

	SUMOReal xmin = minValue(j1->pos.x(),j2->pos.x(),j3->pos.x());
	SUMOReal xmax = maxValue(j1->pos.x(),j2->pos.x(),j3->pos.x());
	SUMOReal xw   = xmax - xmin ;
	cout <<" xmin "<<xmin<<endl;
	cout <<" xmax "<<xmax<<endl;
    cout <<" xwidth "<<xw<<endl;

    SUMOReal ymin = minValue(j1->pos.y(),j2->pos.y(),j3->pos.y());
	SUMOReal ymax = maxValue(j1->pos.y(),j2->pos.y(),j3->pos.y());
	SUMOReal yw   = ymax - ymin ;
    cout <<" ymin "<<ymin<<endl;
	cout <<" ymax "<<ymax<<endl;
    cout <<" ywidth "<<yw<<endl;

	for(DictTypeJunction::iterator j=myJunctionDictB.begin(); j!=myJunctionDictB.end(); j++) {
		cout<<"-----------------------------------------"<<endl;
		cout <<"Junction ID = "<<(*j).second->id<<endl;
		cout <<"       alte x "<< (*j).second->pos.x() <<" alte y "<<(*j).second->pos.y()<<endl;
		SUMOReal nx = ((*j).second->pos.x() -xmin)/xw;
		SUMOReal ny = ((*j).second->pos.y() -ymin)/yw;
        cout <<"       neue x= "<< nx <<" alte y= "<< ny<<endl;
        ((*j).second->pos).set(nx,ny);
	}

}

void
MapEdges::result(void){
	ofstream out("result.txt");
	for(std::map<std::string, std::string>::iterator i=myEdge2JunctionAMap.begin(); i!=myEdge2JunctionAMap.end(); i++) {
		 SUMOReal minAbstand = 77777;
		 std::string id = "";
         string nodeID = (*i).second;
         Position2D posA = myJunctionDictA[nodeID]->pos;
		 for(DictTypeJunction::iterator j=myJunctionDictB.begin(); j!=myJunctionDictB.end(); j++) {
			 SUMOReal X = pow(posA.x() - (*j).second->pos.x(),2);
			 SUMOReal Y = pow(posA.y() - (*j).second->pos.y(),2);
			 SUMOReal nabstand = sqrt(X+Y);
			 if(nabstand < minAbstand ){
				 minAbstand = nabstand;
				 id = myEdge2JunctionBMap[(*j).second->id];
			 }
		 }
		 out<< (*i).first << ";" << id <<endl;
		 cout <<(*i).first <<"    Aequivalent zu    "<< id <<endl;
	}
	out.close();
}

SUMOReal
MapEdges::maxValue(SUMOReal a, SUMOReal b, SUMOReal c)
{
	if(a>=b){
		if(a>=c){
			return a;
		}else{
			return c;
		}
	}else{
		if(b>=c){
			return b;
		}else{
			return c;
		}
	}

}

SUMOReal
MapEdges::minValue(SUMOReal a, SUMOReal b, SUMOReal c)
{
	if(a<=b){
		if(a<=c){
			return a;
		}else{
			return c;
		}
	}else{
		if(b<=c){
			return b;
		}else{
			return c;
		}
	}
}

/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int main(int argc, char** argv)
{
	/*
	if (argc<8) {
    cerr << " syntax error please use:
	     <<   MapEdges <netA> <netB> <junctionA1> <junctionA2> <junctionA3> "
         << "            <junctionB1> <junctionB2> <junctionB3> " << endl;
      return -1;
    }
	*/

	MapEdges *app = new MapEdges("ivvnet.net.xml", "wjt2005.net.xml");//argv[1],argv[2]);
//	app->setJunctionA("53127760", "568052308", "53073921"); //argv[2],argv[3] argv[4]);
	app->setJunctionA("66072613", "27049614", "140446"); //argv[5],argv[6] argv[7]);
	app->setJunctionB("53084179", "53035727", "52768669"); //argv[2],argv[3] argv[4]);
	app->load();
	app->convertA();
	app->convertB();
	app->result();
	return 0;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

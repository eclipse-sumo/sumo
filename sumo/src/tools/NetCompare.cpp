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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "NetCompare.h"
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

NetCompare::DictTypeJunction NetCompare::myJunctionDictA;
NetCompare::DictTypeJunction NetCompare::myJunctionDictB;

/* =========================================================================
 * member definitions
 * ======================================================================= */
//////////////////////////////////////////////////////////////////////
// Constructor/Destructor
//////////////////////////////////////////////////////////////////////

NetCompare::NetCompare(const char *netA, const char *netB)
        : net_a(netA), net_b(netB)
{
}

NetCompare::~NetCompare()
{
}

//////////////////////////////////////////////////////////////////////
// Definitions of the Methods
//////////////////////////////////////////////////////////////////////

/// load net-file and save the Position into a dictionnary
void
NetCompare::load(void)
{
	loadNet(net_a,1);
	loadNet(net_b,2);

}


void
NetCompare::loadNet(const char *net, int dic)
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
			NetCompare::Junction *junction = new Junction(id);
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

	}
	out.close();
}


void
NetCompare::setJunctionA(std::string a,std::string b,std::string c)
{
	juncA1 = a;
	juncA2 = b;
	juncA3 = c;
}

void
NetCompare::setJunctionB(std::string a,std::string b,std::string c)
{
	juncB1 = a;
	juncB2 = b;
	juncB3 = c;

}

void
NetCompare::convertA(void)
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

	double xmin = minValue(j1->pos.x(),j2->pos.x(),j3->pos.x());
	double xmax = maxValue(j1->pos.x(),j2->pos.x(),j3->pos.x());
	double xw   = xmax - xmin ;
	cout <<" xminValue "<<xmin<<endl;
	cout <<" xmaxValue "<<xmax<<endl;
    cout <<" xwidth "<<xw<<endl;

    double ymin = minValue(j1->pos.y(),j2->pos.y(),j3->pos.y());
	double ymax = maxValue(j1->pos.y(),j2->pos.y(),j3->pos.y());
	double yw   = ymax - ymin ;
    cout <<" yminValue "<<ymin<<endl;
	cout <<" ymaxValue "<<ymax<<endl;
    cout <<" ywidth "<<yw<<endl;

	for(DictTypeJunction::iterator j=myJunctionDictA.begin(); j!=myJunctionDictA.end(); j++) {
		cout<<"----------------------------------------------"<<endl;
		cout <<"Junction ID = "<<(*j).second->id<<endl;
		cout <<"     alte x= "<< (*j).second->pos.x() <<" alte y= "<<(*j).second->pos.y()<<endl;
		double nx = ((*j).second->pos.x() -xmin)/xw;
		double ny = ((*j).second->pos.y() -ymin)/yw;
        cout <<"     neue x = "<< nx <<" neue y = "<< ny<<endl;
        ((*j).second->pos).set(nx,ny);
	}


}

/// compare all value to find the NetCompare point
/// write results in a file
void
NetCompare::convertB(void)
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

	double xmin = minValue(j1->pos.x(),j2->pos.x(),j3->pos.x());
	double xmax = maxValue(j1->pos.x(),j2->pos.x(),j3->pos.x());
	double xw   = xmax - xmin ;
	cout <<" xmin "<<xmin<<endl;
	cout <<" xmax "<<xmax<<endl;
    cout <<" xwidth "<<xw<<endl;

    double ymin = minValue(j1->pos.y(),j2->pos.y(),j3->pos.y());
	double ymax = maxValue(j1->pos.y(),j2->pos.y(),j3->pos.y());
	double yw   = ymax - ymin ;
    cout <<" ymin "<<ymin<<endl;
	cout <<" ymax "<<ymax<<endl;
    cout <<" ywidth "<<yw<<endl;

	for(DictTypeJunction::iterator j=myJunctionDictB.begin(); j!=myJunctionDictB.end(); j++) {
		cout<<"-----------------------------------------"<<endl;
		cout <<"Junction ID = "<<(*j).second->id<<endl;
		cout <<"       alte x "<< (*j).second->pos.x() <<" alte y "<<(*j).second->pos.y()<<endl;
		double nx = ((*j).second->pos.x() -xmin)/xw;
		double ny = ((*j).second->pos.y() -ymin)/yw;
        cout <<"       neue x= "<< nx <<" alte y= "<< ny<<endl;
        ((*j).second->pos).set(nx,ny);
	}

}

void
NetCompare::result(const char *output){
	ofstream out(output);
	for(DictTypeJunction::iterator i=myJunctionDictA.begin(); i!=myJunctionDictA.end(); i++) {
		 double minAbstand = 77777;
		 std::string id = "";
		 for(DictTypeJunction::iterator j=myJunctionDictB.begin(); j!=myJunctionDictB.end(); j++) {
			 double X = pow((*i).second->pos.x() - (*j).second->pos.x(),2);
			 double Y = pow((*i).second->pos.y() - (*j).second->pos.y(),2);
			 double nabstand = sqrt(X+Y);
			 if(nabstand < minAbstand ){
				 minAbstand = nabstand;
				 id = (*j).second->id;
			 }
		 }
		 out<<(*i).second->id << ":" << id <<endl;
		 cout <<(*i).second->id <<"    Aequivalent zu    "<< id <<endl;
	}
	out.close();
}

double
NetCompare::maxValue(double a, double b, double c)
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

double
NetCompare::minValue(double a, double b, double c)
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

	if (argc!=7) {
    cerr << " syntax error please use:"
		 << " NetCompare <netA> <netB> <junctionA1>:<junctionB1> "
         << "            <junctionA2>:<junctionB2> <junctionA3>:<junctionB3> <output-file>" << endl;
      return -1;
    }

	std::string arg3 = argv[3];
	std::string arg4 = argv[4];
	std::string arg5 = argv[5];

	NetCompare *app = new NetCompare(argv[1],argv[2]);

	app->setJunctionA(arg3.substr(0,arg3.find(":")),
		              arg4.substr(0,arg4.find(":")),
		              arg5.substr(0,arg5.find(":")));
	app->setJunctionB(arg3.substr(arg3.find(":")+1, arg3.length() - arg3.find(":")-1),
		              arg4.substr(arg4.find(":")+1, arg4.length() - arg4.find(":")-1),
					  arg5.substr(arg5.find(":")+1, arg5.length() - arg5.find(":")-1));
	app->load();
	app->convertA();
	app->convertB();
	app->result(argv[6]);
	return 0;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
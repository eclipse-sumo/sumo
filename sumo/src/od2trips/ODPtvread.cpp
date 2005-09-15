/***************************************************************************
                         ODPtvread.cpp

	 reads OD input data (*.fma) from file in ptv format

			 usage		 : ODPtvread(filename, &data, &size, &total_cars,
												&start, &finish, &factor)
                             -------------------
    project              : SUMO		 :
	subproject           : OD2TRIPS
    begin                : Tue, 25 March 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Peter Mieth
    email                : Peter.Mieth@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.9  2005/09/15 12:04:48  dkrajzew
// LARGE CODE RECHECK
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include "ODmatrix.h"
#include <utils/importio/LineReader.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/convert/TplConvert.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
std::ostream&
operator<< (std::ostream& os, const OD_IN& od)
{
    os << od.from << '\t'
        << od.to << '\t'
        << od.how_many ;
    return (os);
}


void
ODPtvread (string OD_filename,vector<OD_IN>& od_inp, long *maxele,
           long *total_cars, SUMOTime *start, SUMOTime *finish, float *factor)
{
	std::string cLine;
	string help;
	int ferror = 0;
	int maximum=0;
    int i;
    LineReader lr(OD_filename);
	if (!lr.good()) {
        MsgHandler::getErrorInstance()->inform(
            string("Could not open ") + OD_filename + string("."));
		throw ProcessError();
	}
	*maxele=0;
	*total_cars=0;
	int maxdist,j;
    string type = lr.readLine();
    string line;
    // read Verkehrsmittelkennung
    bool found = false;
    while(!found) {
        line = lr.readLine();
        if(line.find('*')==string::npos) {
            found = true;
        }
    }
    // read ZeitInterval
    found = false;
    while(!found) {
        line = lr.readLine();
        if(line.find('*')==string::npos) {
            found = true;
        }
    }
    istringstream t1(line);
    float start_ini, finish_ini;
	t1 >> start_ini >> finish_ini;
	*start = SUMOTime(start_ini);
	*finish = SUMOTime(finish_ini);
	float rest = 6000 * (start_ini - *start);
	*start = (SUMOTime) (3600. * *start + rest);
	rest = 6000 * (finish_ini - *finish);
	*finish = (SUMOTime) (3600. * *finish + rest);
    // read Faktor
    found = false;
    while(!found) {
        line = lr.readLine();
        if(line.find('*')==string::npos) {
            found = true;
        }
    }
    istringstream t2(line);
	t2 >> *factor;
    // read Anzahl Bezirke
    found = false;
    while(!found) {
        line = lr.readLine();
        if(line.find('*')==string::npos) {
            found = true;
        }
    }
    istringstream t3(line);
	t3 >> maxdist;
    // read BezirksNummern
    found = false;
    while(!found) {
        line = lr.readLine();
        if(line.find('*')==string::npos) {
            found = true;
        }
    }
	string *index= new string [maxdist];
    i = 0;
    while(found) {
        line = StringUtils::prune(line);
        StringTokenizer st(line, StringTokenizer::WHITECHARS);
        while(st.hasNext()) {
            *(index+i) = st.next();
            i++;
        }
        line = lr.readLine();
        if(line.find('*')!=string::npos) {
            found = false;
        }
    }
    // read in values
    for(i=0;i<maxdist;i++) {
        found = false;
        while(!found) {
            line = lr.readLine();
            if(line.find('*')==string::npos) {
                found = true;
            }
        }
		OD_IN tmp;
		tmp.from=*(index+i);
        j=0;
        while(found) {
            line = StringUtils::prune(line);
            StringTokenizer st(line, StringTokenizer::WHITECHARS);
            while(st.hasNext()) {
                tmp.to=*(index+j);
                tmp.how_many = TplConvert<char>::_2int(st.next().c_str()); // !!! 2SUMOTime
				od_inp.push_back(tmp);
				*total_cars=*total_cars+tmp.how_many;
				*maxele=*maxele+1;
                j++;
            }
            line = lr.readLine();
            if(line.find('*')!=string::npos) {
                found = false;
            }
        }
	}
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



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
# include <iostream>
# include <sstream>
# include <fstream>
# include <vector>
# include <cstdlib>
# include <ctime>
# include <cmath>
# include <string>
# include <utils/common/UtilExceptions.h>
# include "ODmatrix.h"

std::ostream& operator<< (std::ostream& os, const OD_IN& od) {
  os << od.from << '\t'
     << od.to << '\t'
     << od.how_many ;
  return (os);
}

using namespace std;

int ODPtvread (string OD_filename,vector<OD_IN>& od_inp, long *maxele,
			long *total_cars, long *start, long *finish, float *factor)
	{
	std::string cLine;
	string help;
	int ferror = 0;
	int maximum=0;
	std::ifstream fsSrc (OD_filename.c_str ());
	if (!fsSrc) {
		std::cerr << "Error: Could not open " << OD_filename << "." << std::endl;
		throw ProcessError();
	}
	*maxele=0;
	*total_cars=0;
	int maxdist,j;
	for(int i=0;i<4;i++) (getline (fsSrc, cLine)); // read dummy lines
    float start_ini, finish_ini;
	// read begin and end and convert to seconds
	fsSrc >> start_ini >> finish_ini;
	*start = long int(start_ini);
	*finish = long int(finish_ini);
	float rest = 6000 * (start_ini - *start);
	*start = 3600 * *start + rest;
	rest = 6000 * (finish_ini - *finish);
	*finish = 3600 * *finish + rest;
	getline (fsSrc, cLine);
	getline (fsSrc, cLine);
	fsSrc >> *factor;
	for(i=0;i<8;i++) (getline (fsSrc, cLine)); // read dummy lines
	fsSrc >> maxdist;
	string a;
	for(i=0;i<2;i++) getline (fsSrc, cLine);
	string *index= new string [maxdist];
	for(i=0;i<maxdist;i++) {
		fsSrc >> *(index+i);
	}
	//for(i=0;i<2;i++) (getline (fsSrc, cLine));
	string bez="Summe";
	for(i=0;i<maxdist;i++) {
		while (cLine.find(bez) == -1) {
			getline (fsSrc, cLine);
		}
		OD_IN tmp;
		tmp.from=*(index+i);
		for(j=0;j<maxdist;j++) {
			tmp.to=*(index+j);
			fsSrc >> tmp.how_many;
			if(tmp.how_many > 0) {
				od_inp.push_back(tmp);
				*total_cars=*total_cars+tmp.how_many;
				*maxele=*maxele+1;
			}
		}
		getline (fsSrc, cLine);
	}
	fsSrc.close ();
	return (ferror);
}

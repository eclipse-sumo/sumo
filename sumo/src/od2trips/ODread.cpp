/***************************************************************************
                         Odread.cpp

	 reads OD input data from file

			 usage		 : Odread(filename, data, size, totalcars) 

                             -------------------
    project              : SUMO		 : 
	subproject           : OD2TRIPS
    begin                : Tue, 10 September 2002
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
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

int ODread (string OD_filename,vector<OD_IN>& od_in, int *maxele, int *total_cars) 
	{
	std::string cLine;
	int ferror = 0;
	int n = 0;
	int maximum=0;
	std::ifstream fsSrc (OD_filename.c_str ());
	if (!fsSrc) {
		std::cerr << "Error: Could not open " << OD_filename << "." << std::endl;
		throw ProcessError();
	}
	n = 0;
	for(int i=0;i<11;i++) (getline (fsSrc, cLine)); // read 11 dummy lines
	while (getline (fsSrc, cLine)) {
		std::istringstream isFLine (cLine);
		if(n > *maxele-1) {
			cout << "resize vector!";
			throw ProcessError();
//		od_in.resize (2*od_in.size());
//		maxele=2*maxele;
		}
        OD_IN tmp;
		if (!(isFLine >> tmp.from 
		>> tmp.to 
		>> tmp.how_many))
		{
			std::cerr << "Invalid data set encountered: " << cLine << std::endl;
			throw ProcessError();
		}
        od_in.push_back(tmp);
		*total_cars=*total_cars+tmp.how_many;
		if(tmp.how_many > maximum) maximum=tmp.how_many;
		++n;
	}
	*maxele=n;
	fsSrc.close ();
	return (ferror);
}

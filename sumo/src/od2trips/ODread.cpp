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
/* =========================================================================
 * included modules
 * ======================================================================= */
# include <iostream>
# include <sstream>
# include <fstream>
# include <vector>
# include <cstdlib>
# include <ctime>
# include <cmath>
# include <string>
# include <utils/common/UtilExceptions.h>
# include <utils/common/MsgHandler.h>
# include "ODmatrix.h"

//std::ostream& operator<< (std::ostream& os, const OD_IN& od) {
//  os << od.from << '\t'
//     << od.to << '\t'
//     << od.how_many ;
//  return (os);
//}

using namespace std;

int ODread (string OD_filename,vector<OD_IN>& od_in, long *maxele,
			long *total_cars, long *start, long *finish, float *factor)
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
	for(int i=0;i<2;i++) (getline (fsSrc, cLine)); // read dummy lines
    float start_ini, finish_ini, period;
	// read begin and end and convert to seconds
	fsSrc >> start_ini >> finish_ini;
	*start = int(start_ini);
	*finish = int(finish_ini);
	float rest = 6000 * (start_ini - *start);
	*start = 3600 * *start + rest;
	rest = 6000 * (finish_ini - *finish);
	*finish = 3600 * *finish + rest;
	for(i=0;i<2;i++) (getline (fsSrc, cLine));
	fsSrc >> *factor;
	for(i=0;i<7;i++) (getline (fsSrc, cLine)); // read dummy lines
	while (getline (fsSrc, cLine)) {
		std::istringstream isFLine (cLine);
		if(n > *maxele-1) {
            MsgHandler::getErrorInstance()->inform("resize vector!");
			throw ProcessError();
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ODread.icc"
//#endif

// Local Variables:
// mode:C++
// End:



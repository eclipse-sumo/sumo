/***************************************************************************
                         Odwrite.cpp

	 writes final OD data to output file

			 usage		 : Odwrite(filename, data, size)

                             -------------------
    project              : SUMO		 :
	subproject           : OD2TRIPS
    begin                : Thu, 12 September 2002
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
# include "ODmatrix.h"

using namespace std;

int ODwrite (string OD_outfile, vector<OD_OUT>& od_out, int total_cars)

	{
	int ferror = 0;
	std::ofstream fsSrc (OD_outfile.c_str ());
	if (!fsSrc) {
		std::cerr << "Could not open " << OD_outfile << "." << std::endl;
		throw ProcessError();
	}
    fsSrc << "<tripdefs>" << endl;
	for(int i=0;i<total_cars;i++)
	{
		fsSrc << "   <tripdef id=\"" << i << "\"" << " " << "depart=\"" << od_out[i].time << "\"" <<" ";
		fsSrc << "from=\"" << od_out[i].from << "\"" << " ";
		fsSrc << "to=\"" << od_out[i].to << "\"" << " " << "type=\"0\"/>" << endl;
	}
    fsSrc << "</tripdefs>" << endl;
	fsSrc.close ();
	return (ferror);
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ODwrite.icc"
//#endif

// Local Variables:
// mode:C++
// End:



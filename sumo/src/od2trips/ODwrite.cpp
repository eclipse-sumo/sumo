/***************************************************************************
                         Odwrite.cpp

	 writes final OD data to output file

			 usage		 : Odwrite(filename, data, size)

                             -------------------
    project              : SUMO		 :
	subproject           : OD2TRIPS
    begin                : Thu, 12 September 2002
	modified			 : Thu, 25 March 2003, INVENT purposes
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
# include <utils/options/OptionsSubSys.h>
# include <utils/options/OptionsCont.h>
#include <od2trips/ODDistrictCont.h>
# include "ODmatrix.h"

using namespace std;

int
ODWrite (string OD_outfile, vector<OD_OUT>& od_out, long int total_cars,
         ODDistrictCont &districts)

	{
	int ferror = 0;
	std::ofstream fsSrc (OD_outfile.c_str ());
	if (!fsSrc) {
        MsgHandler::getErrorInstance()->inform(
            string("Could not open ") + OD_outfile + string("."));
		throw ProcessError();
	}
    fsSrc << "<tripdefs>" << endl;
	for(long int i=0;i<total_cars;i++)
	{
		fsSrc << "   <tripdef id=\"" << i << "\"" << " " << "depart=\"" << od_out[i].time << "\"" <<" ";
		fsSrc << "from=\"" << districts.getRandomSourceFromDistrict(od_out[i].from) << "\"" << " ";
		fsSrc << "to=\"" << districts.getRandomSinkFromDistrict(od_out[i].to) << "\"" << " " << "type=\"";
		fsSrc << od_out[i].type << "\"";
        if(!OptionsSubSys::getOptions().getBool("no-color")) {
            double red = districts.getDistrictColor(od_out[i].from);
            double blue = districts.getDistrictColor(od_out[i].to);
            double green = (red + blue) / 2.0;
            fsSrc << " color=\"" << red << "," << green << "," << blue << "\"";
        }
        fsSrc << "/>"<< endl;
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



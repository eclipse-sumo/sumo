/***************************************************************************
                         ODInpread.cpp

	 reads *.inp meta data from *inp file (VISSIM)

			 usage		 : ODInpread(filename, &infiles, &content, &max_infiles)

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
# include <list>
# include <utils/common/UtilExceptions.h>
# include "ODmatrix.h"

using namespace std;

void ODInpread (string OD_filename,string infiles[MAX_INFILES],
				content content[MAX_CONTENT],int *max_infiles)
	{
	std::string cLine;
	char bez1[]="ZUSAMMENSETZUNG";
	char bez2[]="MATRIXDATEI ";
	char bez3[]="BEWERTUNGSDATEI";
	char bez4[]="NAME";
	char bez5[]="ANTEIL";
	char bez6[]="FAHRZEUGTYP";
	char delim1[]="\"";
	char delim2[]="L";
	char delim3[]="V";
	int ferror = 0;
	char datin[MAX_LINELENGTH];

	std::ifstream fsSrc (OD_filename.c_str ());

	if (!fsSrc) {
		std::cerr << "Error: Could not open " << OD_filename << "." << std::endl;
		throw ProcessError();
	}

	int count=0;
	int pos1;
	char *a;
	char *b;
	char *c;
	int fin1=1;
	int index=-1;
	int count2=0;
	while (fsSrc.getline (datin,MAX_LINELENGTH)) {
		if ((c=strstr(datin, bez3)) != NULL) {
			fin1=0;
			*max_infiles=count;
		}
		if (((a=strstr(datin, bez1)) != NULL) && fin1)  {
			pos1=a-datin+16;
			content[count].id=atoi(datin+pos1); // read district ids
			b=strtok(datin,delim1);
			b=strtok(NULL,delim1);
			infiles[count]=b;
			count++;
		}
		if (((a=strstr(datin, bez1)) != NULL) && fin1==0)  {
			if ((b=strstr(datin, bez4)) != NULL) {
				count2=0;
				pos1=a-datin+16;
				index=atoi(datin+pos1);
			}
		}
		if (((a=strstr(datin, bez6)) != NULL) && index>0)  {
			pos1=a-datin+12;
			int typ=atoi(datin+pos1);
			b=strtok(datin,delim2);
			b=strtok(NULL,delim3);
			float anteil;
			anteil=atof(b);
			for (int i=0;i<*max_infiles;i++) { // read content for district-id index
				if (content[i].id==index) {
					content[i].cartype[count2]=typ;
					content[i].fraction[count2]=anteil;
					content[i].max=count2+1;
				}
			}
			count2++;
		}
		if ((strstr(datin, bez6)==NULL) &&
			(strstr(datin, bez1)== NULL)) index=-1;

	}
	fsSrc.close ();
	//return (ferror);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ODread.icc"
//#endif

// Local Variables:
// mode:C++
// End:



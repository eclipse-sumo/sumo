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
	//char bez3[]="BEWERTUNGSDATEI";
	char bez4[]="NAME";
	char bez5[]="ANTEIL";
	char bez6[]="FAHRZEUGTYP";
	char delim1[]="\"";
	char delim2[]="L";
	char delim3[]="V";
	int ferror = 0;
	char datin[MAX_LINELENGTH];
	struct content_tmp {
		int id;
		int max;
		int	cartype[MAX_CARTYPES];
		float fraction[MAX_CARTYPES];
	};
	content_tmp content_tmp[MAX_CONTENT];

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
	int i,j,k;
	int index=-1;
	int count2=0;
	int count3=-1;
	*max_infiles=0;
	while (fsSrc.getline (datin,MAX_LINELENGTH)) {
		if (((a=strstr(datin, bez1)) != NULL) && (strstr(datin, bez2)) != NULL)  {
			pos1=a-datin+16;
			content[*max_infiles].id=atoi(datin+pos1); // read district ids
			b=strtok(datin,delim1);
			b=strtok(NULL,delim1);
			infiles[*max_infiles]=b;
			(*max_infiles)++;
		}
		if (((a=strstr(datin, bez1)) != NULL) && (strstr(datin, bez4)) != NULL) {
			count2=0;
			count3++;
			pos1=a-datin+16;
			index=atoi(datin+pos1);
		}
		// read ZUSAMMENSETZUNG
		if (((a=strstr(datin, bez6)) != NULL) && (strstr(datin, bez5)) != NULL)  {
			pos1=a-datin+12;
			int typ=atoi(datin+pos1);
			b=strtok(datin,delim2);
			b=strtok(NULL,delim3);
			float anteil;
			anteil=atof(b);
			content_tmp[count3].id=index;
			content_tmp[count3].cartype[count2]=typ;
			content_tmp[count3].fraction[count2]=anteil;
			count2=count2+1;
			content_tmp[count3].max=count2;
		}
	}
	 // rearrange ZUSAMMENSETZUNG
	for (i=0;i<(*max_infiles);i++) {
		for (j=0;j<count3+1;j++) {
			if (content[i].id==content_tmp[j].id) {
				content[i].max=content_tmp[j].max;
				for (k=0;k<content_tmp[j].max;k++) {
					content[i].cartype[k]=content_tmp[j].cartype[k];
					content[i].fraction[k]=content_tmp[j].fraction[k];
				}
			}
		}
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



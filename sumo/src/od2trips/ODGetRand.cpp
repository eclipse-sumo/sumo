/***************************************************************************
                         Get_rand.cpp

	 This subroutine returns <number> of random elements from
	 input vector input

			 usage		 : Get_rand(outputnumber, size, input, output, ini)
			 outputnumber: number of randoms to return from input (int)
			 size		 : total number of input numbers to chose from (int)
			 input		 : input numbers to chose from (int)
			 output		 : random output numbers (int)
			 init		 : initialize random generator with time (true/false)
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
# include <cstdlib>
# include <ctime>
# include <cmath>
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
# include <utils/convert/ToString.h>
# include "ODmatrix.h"

using namespace std;

int Get_rand (int number,int maxele, int start, int *input, int *output, bool ini)
	{
	int i, check, newele, index, initial, k;
	double rand_space;

	initial=number;

	if(number==0)
	{
        MsgHandler::getErrorInstance()->inform(
            "No elements requiered, nothing to do");
		throw ProcessError();
	}
    for(k=0;k<maxele;k++) {
        *(input+k)=k;
    }
	//int number2=number;
	if(number>=maxele)
	{
        number = maxele-1;
        MsgHandler::getWarningInstance()->inform(
            string("More random numbers requiered as elements!")
            + string("\n")
            + toString<int>(initial-number) + string(" cars get lost"));
	    for(k=0;k<maxele;k++) *(output+k)=k+start;
		return (number);
	}
	check=0;
	newele=maxele;
	while (check < number)
	{
		rand_space = 1. * (newele-1) / RAND_MAX;
		index = (int) (rand() * rand_space);
		if(index==maxele) index--;
		*(output+check) = *(input+index)+start;
		++check;
		for (i=index;i<newele-1;i++)
			{
				*(input+i)=*(input+i+1);
				if(	*(input+i)<0) {
                    MsgHandler::getErrorInstance()->inform(
                        string("was faul at ") + toString<int>(i));
					throw ProcessError();
				}
			}
		--newele;

	}
	return (number);
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ODGetRand.icc"
//#endif

// Local Variables:
// mode:C++
// End:



/***************************************************************************
                          STRConvert.cpp
			  Some string conversion methods
                             -------------------
    project              : SUMO
    begin                : Mon, 15 Apr 2002
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
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
     const char rcsid[] = "$Id$";
}
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include <climits>
#include "STRConvert.h"
#include <utils/common/UtilExceptions.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
int
STRConvert::_2int(const string &inp)
{
    unsigned length = inp.length();
    if(length==0)
        throw EmptyData();
    int val = 0;
    size_t i = 0;
    if(((char) inp.at(i))=='+'||((char) inp.at(i))=='-')
        i++;
    for(; i<length; i++) {
        val = val * 10;
        if(((char) inp.at(i))>'9' || ((char) inp.at(i))<'0')
            throw NumberFormatException();
        else {
            val = val + ((char) inp.at(i) - 48);
        }
    }
    if(((char) inp[0])=='-')
        val = val * -1;
    return val;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
//


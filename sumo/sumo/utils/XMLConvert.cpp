/***************************************************************************
                          XMLConvert.cpp
			  Some static methods which help while working
			  with XML
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
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
// $Log$
// Revision 1.2  2002/05/14 04:55:40  dkrajzew
// Unexisting files are now catched independent to the Xerces-error mechanism; error report generation moved to XMLConvert
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.3  2002/03/20 12:07:24  dkrajzew
// limits.h changed to climits
//
// Revision 2.2  2002/03/20 09:28:29  dkrajzew
// Nonexistance of <limits>-bug fixed
//
// Revision 2.1  2002/03/20 08:23:17  dkrajzew
// XERCES-builtin functions for string parsing replaced by own methods
//
// Revision 2.0  2002/02/14 14:43:29  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:21  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// ------------------------------------------
// moved to 'utils'
// ------------------------------------------
// Revision 1.3  2001/12/06 13:38:05  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <sstream>
#include <iostream>
#include <climits>
#include <sax/AttributeList.hpp>
#include "XMLConvert.h"
#include <util/XMLString.hpp>
#include "XMLBuildingExceptions.h"
#include <sax/SAXException.hpp>
#include <sax/SAXParseException.hpp>

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
char *
XMLConvert::_2char(const XMLCh *inp) 
{
    if(inp==0) 
        throw XMLUngivenParameterException();
    size_t end = 0;
    for(; inp[end]!=0; end++);
    if(end==0) 
        throw XMLUngivenParameterException();
    char *ret = new char[end+1];
    size_t i = 0;
    for(; inp[i]!=0; i++)
        ret[i] = (char) inp[i];
    ret[i] = 0;
    return ret;
}

char *
XMLConvert::_2char(const XMLCh *inp, const unsigned int length) 
{
    if(inp==0||length==0) 
        throw XMLUngivenParameterException();
    char *ret = new char[length+1];
    size_t i=0;
    for(; inp[i]!=0&&i<length; i++)
        ret[i] = (char) inp[i];
    ret[i] = 0;
    return ret;
}

string 
XMLConvert::_2str(const XMLCh *inp) 
{
    char *cstr = _2char(inp);
    string str(cstr);
    delete[] cstr;
    return str;
}

string 
XMLConvert::_2str(const XMLCh *inp, const unsigned int length) 
{
    char *cstr = _2char(inp, length);
    string str(cstr);
    delete[] cstr;
    return str;
}

long 
XMLConvert::_2long(const XMLCh *inp) 
{
    return _2long(inp, INT_MAX);
}

long 
XMLConvert::_2long(const XMLCh *inp, const unsigned int length) 
{
    if(inp==0) 
        throw XMLUngivenParameterException();
    if(inp[0]==0||length==0)
        throw XMLUngivenParameterException();
    long val = 0;
    size_t i = 0;
    if(((char) inp[i])=='+'||((char) inp[i])=='-')
        i++;
    for(; inp[i]!=0&&i<length; i++) {
        val = val * 10;
        if(((char) inp[i])>'9' || ((char) inp[i])<'0')
            throw XMLNumericFormatException();
        else {
            val = val + ((char) inp[i] - 48);
        }
    }
    if(((char) inp[0])=='-')
        val = val * -1;
    return val;
}

int 
XMLConvert::_2int(const XMLCh *inp) 
{
    return _2int(inp, INT_MAX);
}

int 
XMLConvert::_2int(const XMLCh *inp, const unsigned int length) 
{
    if(inp==0) 
        throw XMLUngivenParameterException();
    if(inp[0]==0)
        throw XMLUngivenParameterException();
    int val = 0;
    size_t i = 0;
    if(((char) inp[i])=='+'||((char) inp[i])=='-')
        i++;
    for(; inp[i]!=0&&i<length; i++) {
        val = val * 10;
        if(((char) inp[i])>'9' || ((char) inp[i])<'0')
            throw XMLNumericFormatException();
        else {
            val = val + ((char) inp[i] - 48);
        }
    }
    if(((char) inp[0])=='-')
        val = val * -1;
    return val;
}

float 
XMLConvert::_2float(const XMLCh *inp) 
{
    return _2float(inp, INT_MAX);
}

float 
XMLConvert::_2float(const XMLCh *inp, const unsigned int length) 
{
    if(inp==0) 
        throw XMLUngivenParameterException();
    if(inp[0]==0)
        throw XMLUngivenParameterException();
    float val = 0;
    size_t i = 0;
    // overread sign
    if(((char) inp[i])=='+'||((char) inp[i])=='-')
        i++;
    // parse digits before the '.'
    for(; ((char) inp[i])!=0&&((char) inp[i])!='.'&&i<length; i++) {
        val = val * 10;
        if(((char) inp[i])>'9' || ((char) inp[i])<'0')
            throw XMLNumericFormatException();
        else {
            val = val + ((char) inp[i] - 48);
        }
    }
    // set sign
    if(((char) inp[0])=='-')
        val = val * -1;
    // parse digits after dot if any
    if(((char) inp[i])==0||i>=length)
        return val;
    i++;
    float div = 10;
    for(; ((char) inp[i])!=0&&i<length; i++) {
        if(((char) inp[i])>'9' || ((char) inp[i])<'0')
            throw XMLNumericFormatException();
        else {
            val = val + (float) ((char) inp[i] - 48) / div;
            div = div * 10;
        }
    }
    return val;
}

bool 
XMLConvert::_2bool(const XMLCh *inp) 
{
  if(inp==0) return false;
  return ( ((char) inp[0])=='1' || 
           ((char) inp[0])=='t' || 
           ((char) inp[0])=='x' || 
           ((char) inp[0])=='*' || 
           ((char) inp[0])=='T' );
}

string
XMLConvert::buildErrorMessage(const std::string &file, const string &type, 
			      const SAXParseException& exception) {
    ostringstream buf;
    buf << type << endl;
    buf << XMLConvert::_2str(exception.getMessage()) << endl;
    buf << " In file: " << file << endl;
    buf << " At line/column " << exception.getLineNumber()+1 << '/'
          << exception.getColumnNumber();
    return buf.str();
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "XMLConvert.icc"
//#endif

// Local Variables:
// mode:C++
// End:

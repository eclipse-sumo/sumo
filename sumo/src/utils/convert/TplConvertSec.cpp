/***************************************************************************
                          TplConvertSec.h
                          Some conversion methods (from strings to other)
                             -------------------
    begin                : Sun, 09 Jun 2002
    copyright            : (C) 2002 by Daniel Krajzewicz
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
    Attention!!!
    As one of few, this module is under the
        Lesser GNU General Public Licence
    *********************************************************************
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
 ***************************************************************************/
// $Log$
// Revision 1.1  2002/10/16 15:03:56  dkrajzew
// initial commit for classes that perform string conversions
//
// Revision 1.4  2002/06/21 10:47:47  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.3  2002/06/11 15:58:26  dkrajzew
// windows eol removed
//
// Revision 1.2  2002/06/10 07:40:22  dkrajzew
// documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include <utils/common/UtilExceptions.h>
#include "TplConvert.h"
#include "TplConvertSec.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
template<class E>
std::string TplConvertSec<E>::_2strSec(const E * const data,
                                       const string &def) {
    return _2strSec(data, TplConvert<E>::getLength(data), def);
}

template<class E>
std::string TplConvertSec<E>::_2strSec(const E * const data, int length,
                                       const string &def) {
    if(data==0||length==0) {
      return def;
    }
    return TplConvert<E>::_2str(data, length);
}

template<class E>
int TplConvertSec<E>::_2intSec(const E * const data, int def) {
    return _2intSec(data, INT_MAX, def);
}

template<class E>
int TplConvertSec<E>::_2intSec(const E * const data, int length, int def) {
    if(data==0||length==0||data[0]==0) {
        return def;
    }
    return TplConvert<E>::_2int(data, length);
}

template<class E>
long TplConvertSec<E>::_2longSec(const E * const data, long def) {
    return _2longSec(data, INT_MAX, def);
}

template<class E>
long TplConvertSec<E>::_2longSec(const E * const data, int length, long def) {
    if(data==0||length==0||data[0]==0) {
        return def;
    }
    return TplConvert<E>::_2long(data, length);
}

template<class E>
float TplConvertSec<E>::_2floatSec(const E * const data, float def) {
    return _2floatSec(data, INT_MAX, def);
}

template<class E>
float TplConvertSec<E>::_2floatSec(const E * const data, int length, float def) {
    if(data==0||length==0||data[0]==0) {
        return def;
    }
    return TplConvert<E>::_2float(data, length);
}

template<class E>
bool TplConvertSec<E>::_2boolSec(const E * const data, bool def) {
   return _2boolSec(data, 1, def);
}

template<class E>
bool TplConvertSec<E>::_2boolSec(const E * const data, int length, bool def) {
    if(data==0||length==0||data[0]==0) {
        return def;
    }
    return TplConvert<E>::_2bool(data, length);
}

template<class E>
char *TplConvertSec<E>::_2charpSec(const E * const data, char *def) {
    return _2charpSec(data, TplConvert<E>::getLength(data), def);
}

template<class E>
char *TplConvertSec<E>::_2charpSec(const E * const data, int length, char *def) {
    if(data==0||length==0) {
        return copy(def);
    }
    return TplConvert<E>::_2charp(data, length);
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "TplConvertSec.icc"
//#endif

// Local Variables:
// mode:C++
// End:


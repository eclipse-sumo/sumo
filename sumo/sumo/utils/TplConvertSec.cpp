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
// Revision 1.4  2002/06/21 10:50:24  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.3  2002/06/11 14:38:23  dkrajzew
// windows eol removed
//
// Revision 1.2  2002/06/11 13:43:36  dkrajzew
// Windows eol removed
//
// Revision 1.1  2002/06/10 08:33:22  dkrajzew
// Parsing of strings into other data formats generelized; Options now recognize false numeric values; documentation added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include "UtilExceptions.h"
#include "TplConvertSec.h"
#include "TplConvert.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
template<class E>
std::string TplConvertSec<E>::_2strSec(const E * const data, string def) {
    return _2strSec(data, TplConvert<E>::getLength(data), def);
}

template<class E>
std::string TplConvertSec<E>::_2strSec(const E * const data, int length, string def) {
   if(data==0||length==0)
      return def;
   string str;
   str.reserve(length);
   for(int i=0; i<length; i++)
      str = str + (char) data[i];
   return str;
}

template<class E>
int TplConvertSec<E>::_2intSec(const E * const data, int def) {
   return _2intSec(data, INT_MAX, def);
}

template<class E>
int TplConvertSec<E>::_2intSec(const E * const data, int length, int def) {
   if(data==0||length==0)
      return def;
   int val = 0;
   int i=0;
   for(; i<length&&data[i]!=0; i++) {
      val = val * 10;
      char akt = (char) data[i];
      if(akt<'0'||akt>'9')
         throw NumberFormatException();
      val = val + akt - 48;
   }
   if(i==0)
      return def;
   return val;
}

template<class E>
long TplConvertSec<E>::_2longSec(const E * const data, long def) {
   return _2longSec(data, INT_MAX, def);
}

template<class E>
long TplConvertSec<E>::_2longSec(const E * const data, int length, long def) {
   if(data==0||length==0)
      return def;
   long ret = 0;
   int i = 0;
   for(; i<length&&data[i]!=0; i++) {
      ret = ret * 10;
      char akt = (char) data[i];
      if(akt<'0'||akt>'9')
         throw NumberFormatException();
      ret = ret + akt - 48;
   }
   if(i==0)
      return def;
   return ret;
}

template<class E>
float TplConvertSec<E>::_2floatSec(const E * const data, float def) {
   return _2floatSec(data, INT_MAX, def);
}

template<class E>
float TplConvertSec<E>::_2floatSec(const E * const data, int length, float def) {
   if(data==0||length==0)
      return def;
   float ret = 0;
   int i=0;
   for(; i<length&&data[i]!=0&&data[i]!='.'; i++) {
      ret = ret * 10;
      char akt = (char) data[i];
      if(akt<'0'||akt>'9')
         throw NumberFormatException();
      ret = ret + akt - 48;
   }
   if((char) data[i]!='.') {
      if(i==0)
         return def;
      return ret;
   }
   int div = 10;
   for(i=0; i<length&&data[i]!=0&&data[i]!='.'; i++) {
      char akt = (char) data[i];
      if(akt<'0'||akt>'9')
         throw NumberFormatException();
      ret = ret + (akt - 48) / div;
      div = div * 10;
   }
   if(i==0)
      return def;
   return ret;
}

template<class E>
bool TplConvertSec<E>::_2boolSec(const E * const data, bool def) {
   return _2boolSec(data, 1, def);
}

template<class E>
bool TplConvertSec<E>::_2boolSec(const E * const data, int length, bool def) {
   if(data==0||length==0)
      return def;
   char akt = (char) data[0];
   return akt=='1' || akt=='x' || akt=='t' || akt=='T';
}

template<class E>
char *TplConvertSec<E>::_2charpSec(const E * const data, char *def) {
   return _2charpSec(data, getLength(data), def);
}

template<class E>
char *TplConvertSec<E>::_2charpSec(const E * const data, int length, char *def) {
   if(data==0||length==0)
      return copy(def);
   char *ret = new char[length+1];
   int i = 0;
   for(; i<length; i++)
      ret[i] = data[i];
   ret[i] = 0;
   return ret;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "TplConvertSec.icc"
//#endif

// Local Variables:
// mode:C++
// End:


/***************************************************************************
                          TplConvert.cpp
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
#include "TplConvert.h"
#include <climits>

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
template<class E>
std::string TplConvert<E>::_2str(const E * const data) {
   return _2str(data, getLength(data));
}

template<class E>
std::string TplConvert<E>::_2str(const E * const data, int length) {
   if(length==0) throw EmptyData();
   string str;
   str.reserve(length);
   for(int i=0; i<length; i++)
      str = str + (char) data[i];
   return str;
}

template<class E>
int TplConvert<E>::_2int(const E * const data) {
   return _2int(data, INT_MAX);
}

template<class E>
int TplConvert<E>::_2int(const E * const data, int length) {
   if(data==0||length==0) throw EmptyData();
   int sgn = 1;
   int i=0;
   if(data[0]=='+')
       i++;
   if(data[0]=='-') {
       i++;
       sgn = -1;
   }
   int val = 0;
   for(; i<length&&data[i]!=0; i++) {
      val = val * 10;
      char akt = (char) data[i];
      if(akt<'0'||akt>'9')
         throw NumberFormatException();
      val = val + akt - 48;
   }
   if(i==0)
      throw EmptyData();
   return val * sgn;
}

template<class E>
long TplConvert<E>::_2long(const E * const data) {
   return _2long(data, INT_MAX);
}

template<class E>
long TplConvert<E>::_2long(const E * const data, int length) {
   if(data==0||length==0) throw EmptyData();
   long sgn = 1;
   int i=0;
   if(data[0]=='+')
       i++;
   if(data[0]=='-') {
       i++;
       sgn = -1;
   }
   long ret = 0;
   for(; i<length&&data[i]!=0; i++) {
      ret = ret * 10;
      char akt = (char) data[i];
      if(akt<'0'||akt>'9')
         throw NumberFormatException();
      ret = ret + akt - 48;
   }
   if(i==0)
      throw EmptyData();
   return ret * sgn;
}

template<class E>
float TplConvert<E>::_2float(const E * const data) {
   return _2float(data, INT_MAX);
}

template<class E>
float TplConvert<E>::_2float(const E * const data, int length) {
   if(data==0||length==0) throw EmptyData();
   float ret = 0;
   int i = 0;
   float sgn = 1;
   if(data[0]=='+')
       i++;
   if(data[0]=='-') {
       i++;
       sgn = -1;
   }
   for(; i<length&&data[i]!=0&&data[i]!='.'; i++) {
      ret = ret * 10;
      char akt = (char) data[i];
      if(akt<'0'||akt>'9')
         throw NumberFormatException();
      ret = ret + akt - 48;
   }
   if((char) data[i]!='.') {
      if(i==0)
         throw EmptyData();
      return ret * sgn;
   }
   float div = 10;
   i++;
   for(; i<length&&data[i]!=0&&data[i]!='.'; i++) {
      char akt = (char) data[i];
      if(akt<'0'||akt>'9')
         throw NumberFormatException();
      ret = ret + ((float) (akt - 48)) / div;
      div = div * 10;
   }
   return ret * sgn;
}

template<class E>
bool TplConvert<E>::_2bool(const E * const data) {
   return _2bool(data, 1);
}

template<class E>
bool TplConvert<E>::_2bool(const E * const data, int length) {
   if(data==0||length==0) throw EmptyData();
   char akt = (char) data[0];
   return akt=='1' || akt=='x' || akt=='t' || akt=='T';
}

template<class E>
char *TplConvert<E>::_2charp(const E * const data) {
   return _2charp(data, getLength(data));
}

template<class E>
char *TplConvert<E>::_2charp(const E * const data, int length) {
   if(length==0) throw EmptyData();
   char *ret = new char[length+1];
   int i = 0;
   for(; i<length; i++)
      ret[i] = data[i];
   ret[i] = 0;
   return ret;
}

template<class E>
E *TplConvert<E>::duplicate(const E * const s) {
   int i=0;
   for(;s[i]!=0; i++);
   char *ret = new E[i+1];
   for(i=0; s[i]!=0; i++)
      ret[i] = s[i];
   ret[i] = 0;
   return ret;
}

template<class E>
size_t TplConvert<E>::getLength(const E * const data) {
   if(data==0)
      return 0;
   size_t i = 0;
   for(; data[i]!=0; i++);
   return i;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "TplConvert.icc"
//#endif

// Local Variables:
// mode:C++
// End:

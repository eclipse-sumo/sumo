#ifndef TplConvSec_h
#define TplConvSec_h
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
// Revision 1.2  2002/06/11 13:43:37  dkrajzew
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

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * TplConvertSec
 * The same as TplConvert, but every method
 */
template<class E>
class TplConvertSec {
public:
   // conversion methods not throwing an exeption without a length
    /** converts a 0-terminated char-type array into std::string
        returns the default value if the data is empty */
    static std::string _2strSec(const E * const data, std::string def="");

    /** converts a 0-terminated char-type array into the integer value described by it
        returns the default value if the data is empty */
    static int _2intSec(const E * const data, int def=-1);

    /** converts a 0-terminated char-type array into the long value described by it
        returns the default value if the data is empty */
    static long _2longSec(const E * const data, long def=-1);

    /** converts a 0-terminated char-type array into the float value described by it
        returns the default value if the data is empty */
    static float _2floatSec(const E * const data, float def=-1);

    /** converts a 0-terminated char-type array into the float value described by it
        returns true when the first char is one of the following: '1', 'x', 't', 'T'
        returns the default value if the data is empty */
    static bool _2boolSec(const E * const data, bool def=false);

    /** converts a 0-terminated char-type array into a 0-terminated 0-terminated c-char-string
        returns the default value if the data is empty */
    static char *_2charpSec(const E * const data, char *def=0);


    // conversion not throwing an exception methods with a length
    /** converts a char-type array into std::string considering the given length
        returns the default value if the data is empty */
    static std::string _2strSec(const E * const data, int length, std::string def="");

    /** converts a char-type array into the integer value described by it considering the given length
        returns the default value if the data is empty */
    static int _2intSec(const E * const data, int length, int def=-1);

    /** converts a char-type array into the long value described by it considering the given length
        returns the default value if the data is empty */
    static long _2longSec(const E * const data, int length, long def=-1);

    /** converts a char-type array into the float value described by it considering the given length
        returns the default value if the data is empty */
    static float _2floatSec(const E * const data, int length, float def=-1);

    /** converts a char-type array into the float value described by it considering the given length
        returns the default value if the data is empty */
    static bool _2boolSec(const E * const data, int length, bool def=false);

    /** converts a char-type array into a 0-terminated 0-terminated c-char-string considering the given length
        returns the default value if the data is empty */
    static char *_2charpSec(const E * const data, int length, char *def=0);
};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#include "TplConvertSec.cpp"
#endif // EXTERNAL_TEMPLATE_DEFINITION

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "TplConvertSec.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


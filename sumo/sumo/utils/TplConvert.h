#ifndef TplConvert_h
#define TplConvert_h
/***************************************************************************
                          TplConvert.h
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
// Revision 1.3  2002/06/11 14:38:22  dkrajzew
// windows eol removed
//
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
 * TplConvert
 * Some methods that convert 0-terminated char-arrays of any type into
 * other values (numerical/boolean) or into std::strings
 */
template<class E>
class TplConvert {
public:
    // conversion methods without a length
    /** converts a 0-terminated char-type array into std::string
        throws an EmptyData - exception if the given string is empty */
    static std::string _2str(const E * const data);

    /** converts a 0-terminated char-type array into the integer value described by it
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does not contain an integer */
    static int _2int(const E * const data);

    /** converts a 0-terminated char-type array into the long value described by it
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does not contain a long */
    static long _2long(const E * const data);

    /** converts a 0-terminated char-type array into the float value described by it
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does not contain a float */
    static float _2float(const E * const data);

    /** converts a 0-terminated char-type array into the float value described by it
        returns true when the first char is one of the following: '1', 'x', 't', 'T'
        throws an EmptyData - exception if the given string is empty */
    static bool _2bool(const E * const data);

    /** converts a 0-terminated char-type array into a 0-terminated 0-terminated c-char-string
        throws an EmptyData - exception if the given string is empty */
    static char *_2charp(const E * const data);


    // conversion methods with a length
    /** converts a char-type array into std::string considering the given length
        throws an EmptyData - exception if the given string is empty */
    static std::string _2str(const E * const data, int length);

    /** converts a char-type array into the integer value described by it considering the given length
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does not contain an integer */
    static int _2int(const E * const data, int length);

    /** converts a char-type array into the long value described by it considering the given length
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does not contain a long */
    static long _2long(const E * const data, int length);

    /** converts a char-type array into the float value described by it considering the given length
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does not contain a float */
    static float _2float(const E * const data, int length);

    /** converts a char-type array into the float value described by it considering the given length
        returns true when the first char is one of the following: '1', 'x', 't', 'T'
        throws an EmptyData - exception if the given string is empty */
    static bool _2bool(const E * const data, int length);

    /** converts a char-type array into a 0-terminated 0-terminated c-char-string considering the given length
        throws an EmptyData - exception if the given string is empty */
    static char *_2charp(const E * const data, int length);


    /** duplicates the given string */
    static E *duplicate(const E * const s);

    /** returns the length of the string (the position of the 0-character) */
    static size_t getLength(const E * const data);
};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#include "TplConvert.cpp"
#endif // EXTERNAL_TEMPLATE_DEFINITION

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "TplConvert.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


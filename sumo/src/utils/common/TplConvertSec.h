/****************************************************************************/
/// @file    TplConvertSec.h
/// @author  Daniel Krajzewicz
/// @date    Sun, 09 Jun 2002
/// @version $Id$
///
// Some conversion methods (from strings to other)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TplConvertSec_h
#define TplConvertSec_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include "TplConvert.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * TplConvertSec
 * The same as TplConvert, but more secure, meaning throwing less exceptions
 * as methods do retrieve a value that is used when nothing to convert is
 * supplied. Still, some methods do throw exceptions when f.e. something
 * that should be a number is not.
 */
template<class E>
class TplConvertSec
{
public:
    // conversion methods not throwing an exeption without a length
    /** converts a 0-terminated char-type array into std::string
        returns the default value if the data is empty */
    static std::string _2strSec(const E * const data,
                                const std::string &def) {
        return _2strSec(data, TplConvert<E>::getLength(data), def);
    }


    /** converts a 0-terminated char-type array into the integer value
            described by it
        returns the default value if the data is empty */
    static int _2intSec(const E * const data, int def) {
        return _2intSec(data, INT_MAX, def);
    }


    /** converts a 0-terminated char-type array into the long value
            described by it
        returns the default value if the data is empty */
    static long _2longSec(const E * const data, long def) {
        return _2longSec(data, INT_MAX, def);
    }


    /** converts a 0-terminated char-type array into the SUMOReal value
            described by it
        returns the default value if the data is empty */
    static SUMOReal _2SUMORealSec(const E * const data, SUMOReal def) {
        return _2SUMORealSec(data, INT_MAX, def);
    }


    /** converts a 0-terminated char-type array into the SUMOReal value
            described by it
        returns true when the first char is one of the following: '1', 'x', 't', 'T'
        returns the default value if the data is empty */
    static bool _2boolSec(const E * const data, bool def) {
        return _2boolSec(data, 1, def);
    }


    /** converts a 0-terminated char-type array into a 0-terminated
            0-terminated c-char-string
        returns the default value if the data is empty */
    static char *_2charpSec(const E * const data, char *def) {
        return _2charpSec(data, TplConvert<E>::getLength(data), def);
    }


    // conversion not throwing an exception methods with a length
    /** converts a char-type array into std::string considering
            the given length
        returns the default value if the data is empty */
    static std::string _2strSec(const E * const data, int length,
                                const std::string &def) {
        if (data==0||length==0) {
            return def;
        }
        return TplConvert<E>::_2str(data, length);
    }


    /** converts a char-type array into the integer value described
            by it considering the given length
        returns the default value if the data is empty */
    static int _2intSec(const E * const data, int length, int def) {
        if (data==0||length==0||data[0]==0) {
            return def;
        }
        return TplConvert<E>::_2int(data, length);
    }


    /** converts a char-type array into the long value described
            by it considering the given length
        returns the default value if the data is empty */
    static long _2longSec(const E * const data, int length, long def) {
        if (data==0||length==0||data[0]==0) {
            return def;
        }
        return TplConvert<E>::_2long(data, length);
    }


    /** converts a char-type array into the SUMOReal value described
            by it considering the given length
        returns the default value if the data is empty */
    static SUMOReal _2SUMORealSec(const E * const data, int length, SUMOReal def) {
        if (data==0||length==0||data[0]==0) {
            return def;
        }
        return TplConvert<E>::_2SUMOReal(data, length);
    }


    /** converts a char-type array into the SUMOReal value described
            by it considering the given length
        returns the default value if the data is empty */
    static bool _2boolSec(const E * const data, int length, bool def) {
        if (data==0||length==0||data[0]==0) {
            return def;
        }
        return TplConvert<E>::_2bool(data, length);
    }


    /** converts a char-type array into a 0-terminated 0-terminated
            c-char-string considering the given length
        returns the default value if the data is empty */
    static char *_2charpSec(const E * const data, int length, char *def) {
        if (data==0||length==0) {
            return TplConvert<E>::copy(def);
        }
        return TplConvert<E>::_2charp(data, length);
    }


};


#endif

/****************************************************************************/


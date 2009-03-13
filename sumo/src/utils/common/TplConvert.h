/****************************************************************************/
/// @file    TplConvert.h
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
#ifndef TplConvert_h
#define TplConvert_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <cmath>
#include <climits>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>


// ===========================================================================
// class definitions
// ===========================================================================
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
    static std::string _2str(const E * const data) {
        return _2str(data, getLength(data));
    }


    /** converts a 0-terminated char-type array into the integer value
            described by it
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does
            not contain an integer */
    static int _2int(const E * const data) {
        return _2int(data, INT_MAX);
    }


    /** converts a 0-terminated char-type array into the long value
            described by it
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does
            not contain a long */
    static long _2long(const E * const data) {
        return _2long(data, INT_MAX);
    }


    /** converts a 0-terminated char-type array into the SUMOReal value
            described by it
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string
            does not contain a SUMOReal */
    static SUMOReal _2SUMOReal(const E * const data) {
        return _2SUMOReal(data, INT_MAX);
    }


    /** converts a 0-terminated char-type array into the SUMOReal value
            described by it
        returns true when the first char is one of the following: '1',
            'x', 't', 'T'
        throws an EmptyData - exception if the given string is empty */
    static bool _2bool(const E * const data) {
        return _2bool(data, 1);
    }


    /** converts a 0-terminated char-type array into a 0-terminated
            0-terminated c-char-string
        throws an EmptyData - exception if the given string is empty */
    static char *_2charp(const E * const data) {
        return _2charp(data, getLength(data));
    }


    // conversion methods with a length
    /** converts a char-type array into std::string considering the given
            length
        throws an EmptyData - exception if the given string is empty */
    static std::string _2str(const E * const data, unsigned length) {
        if (data==0) {
            throw EmptyData();
        }
        if (length==0) {
            return "";
        }
        char *buf = new char[length+1];
        unsigned i = 0;
        for (i=0; i<length; i++) {
            buf[i] = (char) data[i];
        }
        buf[i] = 0;
        std::string ret = buf;
        delete[] buf;
        return ret;
    }


    /** converts a char-type array into the integer value described by it
            considering the given length
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does
            not contain an integer */
    static int _2int(const E * const data, unsigned length) {
        if (data==0||length==0||data[0]==0) {
            throw EmptyData();
        }
        int sgn = 1;
        unsigned i=0;
        if (data[0]=='+') {
            i++;
        }
        if (data[0]=='-') {
            i++;
            sgn = -1;
        }
        int val = 0;
        for (; i<length&&data[i]!=0; i++) {
            val = val * 10;
            char akt = (char) data[i];
            if (akt<'0'||akt>'9') {
                throw NumberFormatException();
            }
            val = val + akt - 48;
        }
        if (i==0) {
            throw EmptyData();
        }
        return val * sgn;
    }


    /** converts a char-type array into the long value described by it
            considering the given length
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does
            not contain a long */
    static long _2long(const E * const data, unsigned length) {
        if (data==0||length==0||data[0]==0) {
            throw EmptyData();
        }
        long sgn = 1;
        unsigned i=0;
        if (data[0]=='+') {
            i++;
        }
        if (data[0]=='-') {
            i++;
            sgn = -1;
        }
        long ret = 0;
        for (; i<length&&data[i]!=0; i++) {
            ret = ret * 10;
            char akt = (char) data[i];
            if (akt<'0'||akt>'9') {
                throw NumberFormatException();
            }
            ret = ret + akt - 48;
        }
        if (i==0) {
            throw EmptyData();
        }
        return ret * sgn;
    }


    /** converts a char-type array into the SUMOReal value described by it
            considering the given length
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does
            not contain a SUMOReal */
    static SUMOReal _2SUMOReal(const E * const data, unsigned length) {
        if (data==0||length==0||data[0]==0) {
            throw EmptyData();
        }
        SUMOReal ret = 0;
        unsigned i = 0;
        SUMOReal sgn = 1;
        if (data[0]=='+') {
            i++;
        }
        if (data[0]=='-') {
            i++;
            sgn = -1;
        }
        for (; i<length&&data[i]!=0&&data[i]!='.'&&data[i]!=','&&data[i]!='e'&&data[i]!='E'; i++) {
            ret = ret * 10;
            char akt = (char) data[i];
            if (akt<'0'||akt>'9') {
                throw NumberFormatException();
            }
            ret = ret + akt - 48;
        }
        // check what has happened - end of string, e or decimal point
        if ((char) data[i]!='.'&&(char) data[i]!=','&&data[i]!='e'&&data[i]!='E') {
            if (i==0) {
                throw EmptyData();
            }
            return ret * sgn;
        }
        if (data[i]=='e'||data[i]=='E') {
            // no decimal point, just an exponent
            try {
                int exp = _2int(data+i+1, length-i-1);
                SUMOReal exp2 = (SUMOReal) pow(10.0, exp);
                return ret*sgn*exp2;
            } catch (EmptyData&) {
                // the exponent was empty
                throw NumberFormatException();
            }
        }
        SUMOReal div = 10;
        // skip the dot
        i++;
        // parse values behin decimal point
        for (; i<length&&data[i]!=0&&data[i]!='e'&&data[i]!='E'; i++) {
            char akt = (char) data[i];
            if (akt<'0'||akt>'9') {
                throw NumberFormatException();
            }
            ret = ret + ((SUMOReal)(akt - 48)) / div;
            div = div * 10;
        }
        if (data[i]!='e'&&data[i]!='E') {
            // no exponent
            return ret * sgn;
        }
        // eponent and decimal dot
        try {
            int exp = _2int(data+i+1, length-i-1);
            SUMOReal exp2 = (SUMOReal) pow(10.0, exp);
            return ret*sgn*exp2;
        } catch (EmptyData&) {
            // the exponent was empty
            throw NumberFormatException();
        }
    }


    /** converts a char-type array into the SUMOReal value described by it
            considering the given length
        returns true when the first char is one of the following: '1',
            'x', 't', 'T'
        throws an EmptyData - exception if the given string is empty */
    static bool _2bool(const E * const data, unsigned length) {
        if (data==0||length==0||data[0]==0) {
            throw EmptyData();
        }
        char akt = (char) data[0];
        if (akt=='1' || akt=='x' || akt=='t' || akt=='T') {
            return true;
        }
        if (akt=='0' || akt=='-' || akt=='f' || akt=='F') {
            return false;
        }
        throw BoolFormatException();
    }


    /** converts a char-type array into a 0-terminated 0-terminated
            c-char-string considering the given length
        throws an EmptyData - exception if the given string is empty */
    static char *_2charp(const E * const data, int length) {
        if (length==0||data==0) {
            throw EmptyData();
        }
        char *ret = new char[length+1];
        unsigned i = 0;
        for (; i<length; i++) {
            ret[i] = (char) data[i];
        }
        ret[i] = 0;
        return ret;
    }


    /** duplicates the given string */
    static E *duplicate(const E * const s) {
        unsigned i=0;
        for (; s[i]!=0; i++);
        char *ret = new E[i+1];
        for (i=0; s[i]!=0; i++) {
            ret[i] = s[i];
        }
        ret[i] = 0;
        return ret;
    }


    /** returns the length of the string (the position of the 0-character) */
    static unsigned getLength(const E * const data) {
        if (data==0) {
            return 0;
        }
        unsigned i = 0;
        for (; data[i]!=0; i++);
        return i;
    }

};


#endif

/****************************************************************************/


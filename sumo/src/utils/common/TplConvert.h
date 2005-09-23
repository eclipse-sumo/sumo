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
// Revision 1.1  2005/09/23 06:05:45  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.1  2005/09/20 06:13:00  dksumo
// floats and doubles replaced by SUMOReal; warnings removed
//
// Revision 1.4  2005/09/12 07:41:23  dksumo
// dos2unix applied; debugging new loading
//
// Revision 1.3  2005/09/09 12:54:02  dksumo
// complete code rework: debug_new and config added
//
// Revision 1.2  2005/04/26 08:11:57  dksumo
// level3 warnings patched; debugging
//
// Revision 1.1.2.1  2005/04/15 10:08:41  dksumo
// level3 warnings removed
//
// Revision 1.1  2004/10/22 12:50:37  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.2  2003/02/07 10:48:00  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:03:56  dkrajzew
// initial commit for classes that perform string conversions
//
// Revision 1.3  2002/06/11 15:58:25  dkrajzew
// windows eol removed
//
// Revision 1.2  2002/06/10 07:40:22  dkrajzew
// documentation added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <cmath>
#include <utils/common/UtilExceptions.h>


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
    static std::string _2str(const E * const data)
    {
        return _2str(data, getLength(data));
    }


    /** converts a 0-terminated char-type array into the integer value
            described by it
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does
            not contain an integer */
    static int _2int(const E * const data)
    {
        return _2int(data, INT_MAX);
    }


    /** converts a 0-terminated char-type array into the long value
            described by it
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does
            not contain a long */
    static long _2long(const E * const data)
    {
        return _2long(data, INT_MAX);
    }


    /** converts a 0-terminated char-type array into the SUMOReal value
            described by it
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string
            does not contain a SUMOReal */
    static SUMOReal _2SUMOReal(const E * const data)
    {
        return _2SUMOReal(data, INT_MAX);
    }


    /** converts a 0-terminated char-type array into the SUMOReal value
            described by it
        returns true when the first char is one of the following: '1',
            'x', 't', 'T'
        throws an EmptyData - exception if the given string is empty */
    static bool _2bool(const E * const data)
    {
        return _2bool(data, 1);
    }


    /** converts a 0-terminated char-type array into a 0-terminated
            0-terminated c-char-string
        throws an EmptyData - exception if the given string is empty */
    static char *_2charp(const E * const data)
    {
        return _2charp(data, getLength(data));
    }


    // conversion methods with a length
    /** converts a char-type array into std::string considering the given
            length
        throws an EmptyData - exception if the given string is empty */
    static std::string _2str(const E * const data, int length)
    {
        if(data==0||length==0) {
            throw EmptyData();
        }
        char *buf = new char[length+1];
        int i = 0;
        for(i=0; i<length; i++) {
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
    static int _2int(const E * const data, int length)
    {
        if(data==0||length==0||data[0]==0) {
            throw EmptyData();
        }
        int sgn = 1;
        int i=0;
        if(data[0]=='+') {
            i++;
        }
        if(data[0]=='-') {
            i++;
            sgn = -1;
        }
        int val = 0;
        for(; i<length&&data[i]!=0; i++) {
            val = val * 10;
            char akt = (char) data[i];
            if(akt<'0'||akt>'9') {
                throw NumberFormatException();
            }
            val = val + akt - 48;
        }
        if(i==0) {
            throw EmptyData();
        }
        return val * sgn;
    }


    /** converts a char-type array into the long value described by it
            considering the given length
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does
            not contain a long */
    static long _2long(const E * const data, int length)
    {
        if(data==0||length==0||data[0]==0) {
            throw EmptyData();
        }
        long sgn = 1;
        int i=0;
        if(data[0]=='+') {
            i++;
        }
        if(data[0]=='-') {
            i++;
            sgn = -1;
        }
        long ret = 0;
        for(; i<length&&data[i]!=0; i++) {
            ret = ret * 10;
            char akt = (char) data[i];
            if(akt<'0'||akt>'9') {
                throw NumberFormatException();
            }
            ret = ret + akt - 48;
        }
        if(i==0) {
            throw EmptyData();
        }
        return ret * sgn;
    }


    /** converts a char-type array into the SUMOReal value described by it
            considering the given length
        throws an EmptyData - exception if the given string is empty
        throws a NumberFormatException - exception when the string does
            not contain a SUMOReal */
    static SUMOReal _2SUMOReal(const E * const data, int length)
    {
        if(data==0||length==0||data[0]==0) {
            throw EmptyData();
        }
        SUMOReal ret = 0;
        int i = 0;
        SUMOReal sgn = 1;
        if(data[0]=='+') {
            i++;
        }
        if(data[0]=='-') {
            i++;
            sgn = -1;
        }
        for(; i<length&&data[i]!=0&&data[i]!='.'&&data[i]!=','&&data[i]!='e'&&data[i]!='E'; i++) {
            ret = ret * 10;
            char akt = (char) data[i];
            if(akt<'0'||akt>'9') {
                throw NumberFormatException();
            }
            ret = ret + akt - 48;
        }
        // check what has happened - end of string, e or decimal point
        if((char) data[i]!='.'&&(char) data[i]!=','&&data[i]!='e'&&data[i]!='E') {
            if(i==0) {
                throw EmptyData();
            }
            return ret * sgn;
        }
        if(data[i]=='e'||data[i]=='E') {
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
        for(; i<length&&data[i]!=0&&data[i]!='e'&&data[i]!='E'; i++) {
            char akt = (char) data[i];
            if(akt<'0'||akt>'9') {
                throw NumberFormatException();
            }
            ret = ret + ((SUMOReal) (akt - 48)) / div;
            div = div * 10;
        }
        if(data[i]!='e'&&data[i]!='E') {
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
    static bool _2bool(const E * const data, int length)
    {
        if(data==0||length==0||data[0]==0) {
            throw EmptyData();
        }
        char akt = (char) data[0];
        return akt=='1' || akt=='x' || akt=='t' || akt=='T';
    }


    /** converts a char-type array into a 0-terminated 0-terminated
            c-char-string considering the given length
        throws an EmptyData - exception if the given string is empty */
    static char *_2charp(const E * const data, int length)
    {
        if(length==0||data==0) {
            throw EmptyData();
        }
        char *ret = new char[length+1];
        int i = 0;
        for(; i<length; i++) {
            ret[i] = (char) data[i];
        }
        ret[i] = 0;
        return ret;
    }


    /** duplicates the given string */
    static E *duplicate(const E * const s)
    {
        int i=0;
        for(;s[i]!=0; i++);
        char *ret = new E[i+1];
        for(i=0; s[i]!=0; i++) {
            ret[i] = s[i];
        }
        ret[i] = 0;
        return ret;
    }


    /** returns the length of the string (the position of the 0-character) */
    static size_t getLength(const E * const data)
    {
        if(data==0) {
            return 0;
        }
        size_t i = 0;
        for(; data[i]!=0; i++);
        return i;
    }

};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

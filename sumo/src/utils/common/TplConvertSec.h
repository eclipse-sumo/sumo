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
// Revision 1.2  2005/10/07 11:43:30  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
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
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <string>
#include "TplConvert.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * TplConvertSec
 * The same as TplConvert, but more secure, meaning throwing less exceptions
 * as methods do retrieve a value that is used when nothing to convert is
 * supplied. Still, some methods do throw exceptions when f.e. something
 * that should be a number is not.
 */
template<class E>
class TplConvertSec {
public:
   // conversion methods not throwing an exeption without a length
    /** converts a 0-terminated char-type array into std::string
        returns the default value if the data is empty */
    static std::string _2strSec(const E * const data,
        const std::string &def)
    {
        return _2strSec(data, TplConvert<E>::getLength(data), def);
    }


    /** converts a 0-terminated char-type array into the integer value
            described by it
        returns the default value if the data is empty */
    static int _2intSec(const E * const data, int def)
    {
        return _2intSec(data, INT_MAX, def);
    }


    /** converts a 0-terminated char-type array into the long value
            described by it
        returns the default value if the data is empty */
    static long _2longSec(const E * const data, long def)
    {
        return _2longSec(data, INT_MAX, def);
    }


    /** converts a 0-terminated char-type array into the SUMOReal value
            described by it
        returns the default value if the data is empty */
    static SUMOReal _2SUMORealSec(const E * const data, SUMOReal def)
    {
        return _2SUMORealSec(data, INT_MAX, def);
    }


    /** converts a 0-terminated char-type array into the SUMOReal value
            described by it
        returns true when the first char is one of the following: '1', 'x', 't', 'T'
        returns the default value if the data is empty */
    static bool _2boolSec(const E * const data, bool def)
    {
       return _2boolSec(data, 1, def);
    }


    /** converts a 0-terminated char-type array into a 0-terminated
            0-terminated c-char-string
        returns the default value if the data is empty */
    static char *_2charpSec(const E * const data, char *def)
    {
        return _2charpSec(data, TplConvert<E>::getLength(data), def);
    }


    // conversion not throwing an exception methods with a length
    /** converts a char-type array into std::string considering
            the given length
        returns the default value if the data is empty */
    static std::string _2strSec(const E * const data, int length,
        const std::string &def)
    {
        if(data==0||length==0) {
            return def;
        }
        return TplConvert<E>::_2str(data, length);
    }


    /** converts a char-type array into the integer value described
            by it considering the given length
        returns the default value if the data is empty */
    static int _2intSec(const E * const data, int length, int def)
    {
        if(data==0||length==0||data[0]==0) {
            return def;
        }
        return TplConvert<E>::_2int(data, length);
    }


    /** converts a char-type array into the long value described
            by it considering the given length
        returns the default value if the data is empty */
    static long _2longSec(const E * const data, int length, long def)
    {
        if(data==0||length==0||data[0]==0) {
            return def;
        }
        return TplConvert<E>::_2long(data, length);
    }


    /** converts a char-type array into the SUMOReal value described
            by it considering the given length
        returns the default value if the data is empty */
    static SUMOReal _2SUMORealSec(const E * const data, int length, SUMOReal def)
    {
        if(data==0||length==0||data[0]==0) {
            return def;
        }
        return TplConvert<E>::_2SUMOReal(data, length);
    }


    /** converts a char-type array into the SUMOReal value described
            by it considering the given length
        returns the default value if the data is empty */
    static bool _2boolSec(const E * const data, int length, bool def)
    {
        if(data==0||length==0||data[0]==0) {
            return def;
        }
        return TplConvert<E>::_2bool(data, length);
    }


    /** converts a char-type array into a 0-terminated 0-terminated
            c-char-string considering the given length
        returns the default value if the data is empty */
    static char *_2charpSec(const E * const data, int length, char *def)
    {
        if(data==0||length==0) {
            return copy(def);
        }
        return TplConvert<E>::_2charp(data, length);
    }


};


/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
// End:

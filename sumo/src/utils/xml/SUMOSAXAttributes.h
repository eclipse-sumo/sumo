/****************************************************************************/
/// @file    SUMOSAXAttributes.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 30 Mar 2007
/// @version $Id: SUMOSAXAttributes.h 5002 2008-02-01 13:46:21Z dkrajzew $
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMOSAXAttributes_h
#define SUMOSAXAttributes_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/SUMOTime.h>
#include "SUMOXMLDefinitions.h"
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOSAXAttributes
 * @brief 
 *
 */
class SUMOSAXAttributes 
{
public:
    SUMOSAXAttributes() { }


    virtual ~SUMOSAXAttributes() { }


    //{ methods for retrieving attribute values
    /**
     * @brief Returns the information whether the named (by its enum-value) attribute is within the current list
     */
    virtual bool hasAttribute(SumoXMLAttr id) const throw() = 0;


    /**
     * @brief Returns the bool-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2bool.
     *  If the attribute is empty or ==0, TplConvert<XMLCh>::_2bool throws an
     *  EmptyData-exception which is passed.
     * If the value can not be parsed to a bool, TplConvert<XMLCh>::_2bool throws a
     *  BoolFormatException-exception which is passed.
     *
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     * @exception BoolFormatException If the attribute value can not be parsed to a bool
     */
    virtual bool getBool(SumoXMLAttr id) const throw(EmptyData, BoolFormatException) = 0;

    /**
     * @brief Returns the bool-value of the named (by its enum-value) attribute or the given value if the attribute is not known
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2boolSec.
     *  If the attribute is empty, TplConvert<XMLCh>::_2boolSec throws an
     *  EmptyData-exception which is passed. If the attribute==0, TplConvert<XMLCh>::_2boolSec
     *  returns the default value.
     *
     * @exception EmptyData If the attribute value is an empty string
     */
    virtual bool getBoolSecure(SumoXMLAttr id, bool val) const throw(EmptyData) = 0;


    /**
     * @brief Returns the int-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2int.
     *  If the attribute is empty or ==0, TplConvert<XMLCh>::_2int throws an
     *  EmptyData-exception which is passed.
     * If the value can not be parsed to an int, TplConvert<XMLCh>::_2int throws a
     *  NumberFormatException-exception which is passed.
     *
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     * @exception NumberFormatException If the attribute value can not be parsed to an int
     */
    virtual int getInt(SumoXMLAttr id) const throw(EmptyData, NumberFormatException) = 0;

    /**
     * @brief Returns the int-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2intSec.
     *  If the attribute is empty, TplConvert<XMLCh>::_2intSec throws an
     *  EmptyData-exception which is passed. If the attribute==0, TplConvert<XMLCh>::_2intSec
     *  returns the default value.
     * If the value can not be parsed to an int, TplConvert<XMLCh>::_2intSec throws a
     *  NumberFormatException-exception which is passed.
     *
     * @exception EmptyData If the attribute value is an empty string
     * @exception NumberFormatException If the attribute value can not be parsed to an int
     */
    virtual int getIntSecure(SumoXMLAttr id, int def) const throw(EmptyData, NumberFormatException) = 0;


    /**
     * @brief Returns the string-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2str.
     *  If the attribute is ==0, TplConvert<XMLCh>::_2str throws an
     *  EmptyData-exception which is passed.
     *
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     */
    virtual std::string getString(SumoXMLAttr id) const throw(EmptyData) = 0;

    /**
     * @brief Returns the string-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2strSec.
     *  If the attribute is ==0, TplConvert<XMLCh>::_2strSec returns the default value.
     *
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     */
    virtual std::string getStringSecure(SumoXMLAttr id,
                                const std::string &str) const throw(EmptyData) = 0;


    /**
     * @brief Returns the SUMOReal-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2SUMOReal.
     *  If the attribute is empty or ==0, TplConvert<XMLCh>::_2SUMOReal throws an
     *  EmptyData-exception which is passed.
     * If the value can not be parsed to a SUMOReal, TplConvert<XMLCh>::_2SUMOReal throws a
     *  NumberFormatException-exception which is passed.
     *
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     * @exception NumberFormatException If the attribute value can not be parsed to an SUMOReal
     */
    virtual SUMOReal getFloat(SumoXMLAttr id) const throw(EmptyData, NumberFormatException) = 0;

    /**
     * @brief Returns the SUMOReal-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2SUMORealSec.
     *  If the attribute is empty, TplConvert<XMLCh>::_2SUMORealSec throws an
     *  EmptyData-exception which is passed. If the attribute==0, TplConvert<XMLCh>::_2SUMORealSec
     *  returns the default value.
     * If the value can not be parsed to a SUMOReal, TplConvert<XMLCh>::_2SUMORealSec throws a
     *  NumberFormatException-exception which is passed.
     *
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     * @exception NumberFormatException If the attribute value can not be parsed to an SUMOReal
     */
    virtual SUMOReal getFloatSecure(SumoXMLAttr id, SUMOReal def) const throw(EmptyData, NumberFormatException) = 0;

    /**
     * @brief Returns the information whether the named attribute is within the current list
     */
    virtual bool hasAttribute(const std::string &id) const throw() = 0;


    /**
     * @brief Returns the SUMOReal-value of the named attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2SUMOReal.
     *  If the attribute is empty or ==0, TplConvert<XMLCh>::_2SUMOReal throws an
     *  EmptyData-exception which is passed.
     * If the value can not be parsed to a SUMOReal, TplConvert<XMLCh>::_2SUMOReal throws a
     *  NumberFormatException-exception which is passed.
     *
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     * @exception NumberFormatException If the attribute value can not be parsed to an SUMOReal
     */
    virtual SUMOReal getFloat(const std::string &id) const throw(EmptyData, NumberFormatException) = 0;

    /**
     * @brief Returns the string-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. 
     *  If the attribute is ==0, TplConvert<XMLCh>::_2strSec returns the default value.
     */
    virtual std::string getStringSecure(const std::string &id,
                                const std::string &str) const throw() = 0;
    //}


private:
    /// we made the copy constructor invalid
    SUMOSAXAttributes(const SUMOSAXAttributes &src);

    /// we made the assignment operator invalid
    SUMOSAXAttributes &operator=(const SUMOSAXAttributes &src);

};


#endif

/****************************************************************************/


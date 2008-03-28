/****************************************************************************/
/// @file    SUMOSAXAttributesImpl_Xerces.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 30 Mar 2007
/// @version $Id: SUMOSAXAttributesImpl_Xerces.h 5002 2008-02-01 13:46:21Z dkrajzew $
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
#ifndef SUMOSAXAttributesImpl_Xerces_h
#define SUMOSAXAttributesImpl_Xerces_h


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
#include "SUMOSAXAttributes.h"
#include <xercesc/sax2/Attributes.hpp>
#include <map>


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOSAXAttributesImpl_Xerces
 * @brief 
 *
 */
class SUMOSAXAttributesImpl_Xerces : public SUMOSAXAttributes
{
public:
    SUMOSAXAttributesImpl_Xerces(const Attributes &attrs,
        const std::map<SumoXMLAttr, XMLCh*> &predefinedTags) throw();


    virtual ~SUMOSAXAttributesImpl_Xerces() throw();


    //{ methods for retrieving attribute values
    /**
     * @brief Returns the information whether the named (by its enum-value) attribute is within the current list
     */
    bool hasAttribute(SumoXMLAttr id) const throw();


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
    bool getBool(SumoXMLAttr id) const throw(EmptyData, BoolFormatException);

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
    bool getBoolSecure(SumoXMLAttr id, bool val) const throw(EmptyData);


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
    int getInt(SumoXMLAttr id) const throw(EmptyData, NumberFormatException);

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
    int getIntSecure(SumoXMLAttr id, int def) const throw(EmptyData, NumberFormatException);


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
    std::string getString(SumoXMLAttr id) const throw(EmptyData);

    /**
     * @brief Returns the string-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2strSec.
     *  If the attribute is ==0, TplConvert<XMLCh>::_2strSec returns the default value.
     *
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     */
    std::string getStringSecure(SumoXMLAttr id,
                                const std::string &str) const throw(EmptyData);


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
    SUMOReal getFloat(SumoXMLAttr id) const throw(EmptyData, NumberFormatException);

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
    SUMOReal getFloatSecure(SumoXMLAttr id, SUMOReal def) const throw(EmptyData, NumberFormatException);


    /**
     * @brief Returns the information whether the named attribute is within the current list
     */
    bool hasAttribute(const std::string &id) const throw();


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
    SUMOReal getFloat(const std::string &id) const throw(EmptyData, NumberFormatException);

    /**
     * @brief Returns the string-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. 
     *  If the attribute is ==0, TplConvert<XMLCh>::_2strSec returns the default value.
     */
    std::string getStringSecure(const std::string &id,
                                const std::string &str) const throw();
    //}

private:
    const XMLCh *getAttributeValueSecure(SumoXMLAttr id) const throw();

private:
    const Attributes &myAttrs;
    // the map of tag names to their internal numerical representation
    typedef std::map<SumoXMLAttr, XMLCh*> AttrMap;
    const AttrMap &myPredefinedTags;


private:
    /// we made the copy constructor invalid
    SUMOSAXAttributesImpl_Xerces(const SUMOSAXAttributesImpl_Xerces &src);

    /// we made the assignment operator invalid
    SUMOSAXAttributesImpl_Xerces &operator=(const SUMOSAXAttributesImpl_Xerces &src);

};


#endif

/****************************************************************************/


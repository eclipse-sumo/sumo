/****************************************************************************/
/// @file    SUMOSAXAttributes.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 30 Mar 2007
/// @version $Id$
///
// Encapsulated SAX-Attributes
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
#include <vector>
#include <utils/common/SUMOTime.h>
#include "SUMOXMLDefinitions.h"
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOSAXAttributes
 * @brief Encapsulated SAX-Attributes
 *
 * This class is an interface for using encapsulated SAX-attributes.
 * Encapsulation is done to allow a common acces without the need to
 *  import all the Xerces-definitions.
 */
class SUMOSAXAttributes {
public:
    /// @brief Constructor
    SUMOSAXAttributes() throw() { }


    /// @brief Destructor
    virtual ~SUMOSAXAttributes() throw() { }


    /** @brief Tries to read the id from the attributes, stores it into "id" if given
     *
     * If there is no attribute named "id" or it is empty, false is returned.
     *  An error is reported to error message handler if "report" is true.
     * Otherwise, the id is stored in the variable "id" and true is returned.
     *
     * @param[in] objecttype The name of the parsed object type; used for error message generation
     * @param[out] id The read id is stored herein (or "" if no id is given)
     * @param[in] report Whether missing id shall be reported to MsgHandler::getErrorInstance
     * @return Whether a valid id could be retrieved
     */
    bool setIDFromAttributes(const char *objecttype, std::string &id,
                             bool report=true) const throw();


    /** @brief Tries to read given attribute assuming it is an int
     *
     * If an error occures (the attribute is not there, it is not numeric), "ok" is
     *  set to false and an error message is written to MsgHandler::getErrorInstance.
     *
     * Otherwise, "ok" is set to true and the read value is returned.
     *
     * @param[in] attr The id of the attribute to read
     * @param[in] objecttype The name of the parsed object type; used for error message generation
     * @param[in] objectid The name of the parsed object; used for error message generation
     * @param[out] ok Whether the value could be read
     * @param[in] report Whether errors shall be written to msg handler's error instance
     * @return The read value; -1 if an error occured
     */
    int getIntReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid, bool &ok,
                        bool report=true) const throw();


    /** @brief Tries to read given attribute assuming it is an SUMOReal
     *
     * If an error occures (the attribute is not there, it is not numeric), "ok" is
     *  set to false and an error message is written to MsgHandler::getErrorInstance.
     *
     * Otherwise, "ok" is set to true and the read value is returned.
     *
     * @param[in] attr The id of the attribute to read
     * @param[in] objecttype The name of the parsed object type; used for error message generation
     * @param[in] objectid The name of the parsed object; used for error message generation
     * @param[out] ok Whether the value could be read
     * @param[in] report Whether errors shall be written to msg handler's error instance
     * @return The read value; -1 if an error occured
     */
    SUMOReal getSUMORealReporting(SumoXMLAttr attr, const char *objecttype, const char *objectid, bool &ok,
                                  bool report=true) const throw();




    /// @name virtual methods for retrieving attribute values
    /// @{

    /** @brief Returns the information whether the named (by its enum-value) attribute is within the current list
     *
     * @param[in] id The id of the attribute to search for
     * @return Whether the attribute is within the attributes
     */
    virtual bool hasAttribute(SumoXMLAttr id) const throw() = 0;


    /** @brief Returns the information whether the named attribute is within the current list
     *
     * @param[in] id The name of the attribute to search for
     * @return Whether the named attribute is within the attributes
     */
    virtual bool hasAttribute(const std::string &id) const throw() = 0;


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
     * @param[in] id The id of the attribute to return the value of
     * @return The attribute's value as a bool, if it could be read and parsed
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
     * @param[in] id The id of the attribute to return the value of
     * @param[in] val The default value to return if the attribute is not in attributes
     * @return The attribute's value as a bool, if it could be read and parsed
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
     * @param[in] id The id of the attribute to return the value of
     * @return The attribute's value as an int, if it could be read and parsed
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
     * @param[in] id The id of the attribute to return the value of
     * @param[in] def The default value to return if the attribute is not in attributes
     * @return The attribute's value as an int, if it could be read and parsed
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
     * @param[in] id The id of the attribute to return the value of
     * @return The attribute's value as a string, if it could be read and parsed
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
     * @param[in] id The id of the attribute to return the value of
     * @param[in] def The default value to return if the attribute is not in attributes
     * @return The attribute's value as a string, if it could be read and parsed
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     */
    virtual std::string getStringSecure(SumoXMLAttr id,
                                        const std::string &def) const throw(EmptyData) = 0;


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
     * @param[in] id The id of the attribute to return the value of
     * @return The attribute's value as a float, if it could be read and parsed
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
     * @param[in] id The id of the attribute to return the value of
     * @param[in] def The default value to return if the attribute is not in attributes
     * @return The attribute's value as a float, if it could be read and parsed
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     * @exception NumberFormatException If the attribute value can not be parsed to an SUMOReal
     */
    virtual SUMOReal getFloatSecure(SumoXMLAttr id, SUMOReal def) const throw(EmptyData, NumberFormatException) = 0;


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
     * @param[in] id The name of the attribute to return the value of
     * @return The attribute's value as a float, if it could be read and parsed
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     * @exception NumberFormatException If the attribute value can not be parsed to an SUMOReal
     */
    virtual SUMOReal getFloat(const std::string &id) const throw(EmptyData, NumberFormatException) = 0;


    /**
     * @brief Returns the string-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list.
     *  If the attribute is ==0, TplConvert<XMLCh>::_2strSec returns the default value.
     * @param[in] id The name of the attribute to return the value of
     * @param[in] def The default value to return if the attribute is not in attributes
     * @return The attribute's value as a string, if it could be read and parsed
     */
    virtual std::string getStringSecure(const std::string &id,
                                        const std::string &def) const throw() = 0;
    //}


    /** @brief Converts the given attribute id into a man readable string
     *
     * @param[in] attr The id of the attribute to return the name of
     * @return The name of the described attribute
     */
    virtual std::string getName(SumoXMLAttr attr) const throw() = 0;


    /** @brief Splits the given string
     *
     * Spaces, ",", and ";" are assumed to be separator characters.
     * Though, in the case a "," or a ";" occures, a warning is generated (once).
     *
     * @param[in] def The string to split
     * @param[out] into The vector to fill
     */
    static void parseStringVector(const std::string &def, std::vector<std::string> &into) throw();


private:
    /// @brief Information whether the usage of a deprecated divider was reported
    static bool myHaveInformedAboutDeprecatedDivider;

private:
    /// @brief Invalidated copy constructor.
    SUMOSAXAttributes(const SUMOSAXAttributes &src);

    /// @brief Invalidated assignment operator.
    SUMOSAXAttributes &operator=(const SUMOSAXAttributes &src);


};


#endif

/****************************************************************************/


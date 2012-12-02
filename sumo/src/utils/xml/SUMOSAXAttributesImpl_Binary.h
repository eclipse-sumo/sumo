/****************************************************************************/
/// @file    SUMOSAXAttributesImpl_Binary.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Fri, 30 Mar 2007
/// @version $Id$
///
// Encapsulated Xerces-SAX-attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMOSAXAttributesImpl_Binary_h
#define SUMOSAXAttributesImpl_Binary_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <set>
#include "SUMOSAXAttributes.h"


// ===========================================================================
// class declarations
// ===========================================================================
class BinaryInputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMOSAXAttributesImpl_Binary
 * @brief Encapsulated Xerces-SAX-attributes
 *
 * @see SUMOSAXAttributes
 */
class SUMOSAXAttributesImpl_Binary : public SUMOSAXAttributes {
public:
    /** @brief Constructor
     *
     * @param[in] attrs The encapsulated xerces-attributes
     * @param[in] predefinedTags Map of attribute ids to their xerces-representation
     * @param[in] predefinedTagsMML Map of attribute ids to their (readable) string-representation
     */
    SUMOSAXAttributesImpl_Binary(const std::map<int, std::string>& predefinedTagsMML,
                                 const std::string& objectType,
                                 BinaryInputDevice* in);


    /// @brief Destructor
    virtual ~SUMOSAXAttributesImpl_Binary();




    /// @name methods for retrieving attribute values
    /// @{

    /** @brief Returns the information whether the named (by its enum-value) attribute is within the current list
     *
     * @param[in] id The id of the searched attribute
     * @return Whether the attribute is within the attributes
     */
    bool hasAttribute(int id) const;


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
    bool getBool(int id) const throw(EmptyData, BoolFormatException);

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
     * @param[in] def The default value to return if the attribute is not in attributes
     * @return The attribute's value as a bool, if it could be read and parsed
     * @exception EmptyData If the attribute value is an empty string
     */
    bool getBoolSecure(int id, bool def) const throw(EmptyData);


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
    int getInt(int id) const;

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
    int getIntSecure(int id, int def) const;


    /**
     * @brief Returns the long-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2long.
     *  If the attribute is empty or ==0, TplConvert<XMLCh>::_2long throws an
     *  EmptyData-exception which is passed.
     * If the value can not be parsed to a long, TplConvert<XMLCh>::_2long throws a
     *  NumberFormatException-exception which is passed.
     *
     * @param[in] id The id of the attribute to return the value of
     * @return The attribute's value as an int, if it could be read and parsed
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     * @exception NumberFormatException If the attribute value can not be parsed to an int
     */
    SUMOLong getLong(int id) const;


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
    std::string getString(int id) const throw(EmptyData);

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
    std::string getStringSecure(int id,
                                const std::string& def) const throw(EmptyData);


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
    SUMOReal getFloat(int id) const;

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
    SUMOReal getFloatSecure(int id, SUMOReal def) const;


    /**
     * @brief Returns the information whether the named attribute is within the current list
     */
    bool hasAttribute(const std::string& id) const;


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
    SUMOReal getFloat(const std::string& id) const;


    /**
     * @brief Returns the string-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list.
     *  If the attribute is ==0, TplConvert<XMLCh>::_2strSec returns the default value.
     * @param[in] id The name of the attribute to return the value of
     * @param[in] def The default value to return if the attribute is not in attributes
     * @return The attribute's value as a string, if it could be read and parsed
     */
    std::string getStringSecure(const std::string& id,
                                const std::string& def) const;
    //}


    /**
     * @brief Returns the value of the named attribute
     *
     * Tries to retrieve the attribute from the the attribute list.
     * @return The attribute's value as a string, if it could be read and parsed
     */
    SumoXMLEdgeFunc getEdgeFunc(bool& ok) const;


    /**
     * @brief Returns the value of the named attribute
     *
     * Tries to retrieve the attribute from the the attribute list.
     * @return The attribute's value as a string, if it could be read and parsed
     */
    SumoXMLNodeType getNodeType(bool& ok) const;


    /**
     * @brief Returns the value of the named attribute
     *
     * Tries to retrieve the attribute from the the attribute list.
     * @return The attribute's value as a RGBColor, if it could be read and parsed
     */
    RGBColor getColorReporting(const char* objectid, bool& ok) const;


    /** @brief Tries to read given attribute assuming it is a PositionVector
     *
     * If an error occurs (the attribute is not there, it's empty), "ok" is
     *  set to false and an error message is written to MsgHandler::getErrorInstance.
     *
     * Otherwise, "ok" is not changed.
     *
     * @param[in] attr The id of the attribute to read
     * @param[in] objectid The name of the parsed object; used for error message generation
     * @param[out] ok Whether the value could be read
     * @param[in] report Whether errors shall be written to msg handler's error instance
     * @return The read value if given and not empty; "" if an error occured
     */
    PositionVector getShapeReporting(int attr, const char* objectid, bool& ok,
                                     bool allowEmpty) const;

    /** @brief Tries to read given attribute assuming it is a Boundary
     *
     * If an error occurs (the attribute is not there, it's empty), "ok" is
     *  set to false and an error message is written to MsgHandler::getErrorInstance.
     *
     * Otherwise, "ok" is not changed.
     *
     * @param[in] attr The id of the attribute to read
     * @param[in] objectid The name of the parsed object; used for error message generation
     * @param[out] ok Whether the value could be read
     * @param[in] report Whether errors shall be written to msg handler's error instance
     * @return The read value if given and not empty; "" if an error occured
     */
    Boundary getBoundaryReporting(int attr, const char* objectid, bool& ok) const;


    /** @brief Converts the given attribute id into a man readable string
     *
     * Returns a "?" if the attribute is not known.
     *
     * @param[in] attr The id of the attribute to return the name of
     * @return The name of the described attribute
     */
    std::string getName(int attr) const;


    /** @brief Prints all attribute names and values into the given stream
     *
     * @param[in] os The stream to use
     */
    void serialize(std::ostream& os) const;


private:
    /// @brief Map of attribute ids to names
    const std::map<int, std::string>& myAttrIds;

    /// @brief the attributes which are set
    std::set<int> myAttrs;

    /// @brief Map of attribute ids to char
    std::map<int, char> myCharValues;

    /// @brief Map of attribute ids to integers
    std::map<int, int> myIntValues;

    /// @brief Map of attribute ids to floats
    std::map<int, SUMOReal> myFloatValues;

    /// @brief Map of attribute ids to string
    std::map<int, std::string> myStringValues;

    /// @brief Map of attribute ids to string
    std::map<int, PositionVector> myPositionVectors;


private:
    /// @brief Invalidated copy constructor.
    SUMOSAXAttributesImpl_Binary(const SUMOSAXAttributesImpl_Binary& src);

    /// @brief Invalidated assignment operator.
    SUMOSAXAttributesImpl_Binary& operator=(const SUMOSAXAttributesImpl_Binary& src);


};


#endif

/****************************************************************************/


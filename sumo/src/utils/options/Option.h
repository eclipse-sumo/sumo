/****************************************************************************/
/// @file    Option.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id: $
///
// A class representing a single program option
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
#ifndef Option_h
#define Option_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <exception>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * Definition of a vector of unsigned ints for later parsing of values
 * such as frequency lists
 */
typedef std::vector<int> IntVector;


/* -------------------------------------------------------------------------
 * Option
 * ----------------------------------------------------------------------- */
/**
 * @class Option
 * The base class for a single program option. All options which hold values
 * are derived from this class as the type may differ.
 *
 * When set, the value is valid. May be set with an initial, default value.
 *
 * Usage:
 * As the options should only be used during the program initialisation, some
 * methods do output error messages to cout. Error messages may be printed,
 * when a default value is overwritten.
 *
 * Most of the getter- and setter-methods throw exceptions as this base class
 * is not meant to hold any values by itself. Theses are stored in the derived
 * classes. This class does only supply common access for a common base class
 * while the real access methods are implemented in the derived classes.
 * The access of the proper valuetype for each of the derived class must be
 * validated by the programmer.
 *
 * Exceptions:
 * Only the exception "InvalidArgument" from "UtilExceptions" is thrown
 */
class Option
{
public:
    /** destructor */
    virtual ~Option();

    /** returns the information whether the value is valid */
    bool isSet() const;

    /** returns the SUMOReal value */
    virtual SUMOReal getFloat() const;

    /** returns the long value */
    virtual long getLong() const;

    /** returns the int value */
    virtual int getInt() const;

    /** returns the string value */
    virtual std::string getString() const;

    /** returns the bool value */
    virtual bool getBool() const;

    /** returns the bool value */
    virtual const IntVector &getIntVector() const;

    /** sets the value (used for non-bool options) */
    virtual bool set(std::string v, bool isDefault=false);

    /** sets the value (used for bool options) */
    virtual bool set(bool v, bool isDefault=false);

    /** returns the string-representation of the value */
    virtual std::string getValue() const;

    /** returns the information whether the option is a bool option */
    virtual bool isBool() const;

    /** returns the information whether the option holds the default value */
    virtual bool isDefault() const;

    /// returns the information whether this option is a file name
    virtual bool isFileName() const;

    /** Returns the information whether the option may be set a further time */
    bool isWriteable() const;

    /** OptionsCont is a friend class as it may reinitialise the option
        to be a default value */
    friend class OptionsCont;

    /// Returns the description of what this option does
    const std::string &getDescription() const;

    /// Returns a the type name of this option
    virtual const std::string &getTypeName() const;

    /// Returns whether the option has to be set
    bool isMandatory() const;


protected:
    /** sets the "myAmSet" - information. returns whether the option was a
        default option */
    bool markSet(bool isDefault);

protected:
    /** constructor (paramter: value is valid) */
    Option(bool set=false);

    /** copy constructor */
    Option(const Option &s);

    /** assignment operator */
    virtual Option &operator=(const Option &s);

protected:
    /// A type name for this option (has presets, but may be overwritten)
    std::string myTypeName;

private:
    /** information whether the value is set */
    bool myAmSet;

    /** information whether the value is the default value (is then set) */
    bool myHaveTheDefaultValue;

    /** information whether the value may be changed */
    bool myAmWritable;

    /// The description what this option does */
    std::string myDescription;

    /// Information whether the option must be set
    bool myAmMandatory;

};


/* -------------------------------------------------------------------------
 * Option_Integer
 * ----------------------------------------------------------------------- */
/**
 * @class Option_Integer
 * @brief An integer-option
 */
class Option_Integer : public Option
{
public:
    /** constructor; the value will be invalid (unset) */
    Option_Integer();

    /** constructor; the default value is given */
    Option_Integer(int value);

    /** copy constructor */
    Option_Integer(const Option_Integer &s);

    /** destructor */
    ~Option_Integer();

    /** assignment operator */
    Option_Integer &operator=(const Option_Integer &s);

    /** returns the integer value */
    int getInt() const;

    /** sets the given value (converts it to int) */
    bool set(std::string v, bool isDefault=false);

    /** returns the values string-representation */
    std::string getValue() const;


private:
    /** the value, valid only when the base-classes "myAmSet"-member is true */
    int      myValue;

};


/* -------------------------------------------------------------------------
 * Option_Long
 * ----------------------------------------------------------------------- */
class Option_Long : public Option
{
public:
    /** constructor; the value will be invalid (unset) */
    Option_Long();

    /** constructor; the default value is given */
    Option_Long(long value);

    /** copy constructor */
    Option_Long(const Option_Long &s);

    /** destructor */
    ~Option_Long();

    /** assignment operator */
    Option_Long &operator=(const Option_Long &s);

    /** returns the long value */
    long getLong() const;

    /** sets the given value (converts it to long) */
    bool set(std::string v, bool isDefault=false);

    /** returns the values string-representation */
    std::string getValue() const;


private:
    /** the value, valid only when the base-classes "myAmSet"-member is true */
    long      myValue;

};


/* -------------------------------------------------------------------------
 * Option_String
 * ----------------------------------------------------------------------- */
class Option_String : public Option
{
public:
    /** constructor; the value will be invalid (unset) */
    Option_String();

    /** constructor; the default value is given */
    Option_String(std::string value);

    /** copy constructor */
    Option_String(const Option_String &s);

    /** destructor */
    virtual ~Option_String();

    /** assignment operator */
    Option_String &operator=(const Option_String &s);

    /** returns the string value */
    std::string getString() const;

    /** sets the given value */
    bool set(std::string v, bool isDefault=false);

    /** returns the values string-representation */
    std::string getValue() const;


protected:
    /** the value, valid only when the base-classes "myAmSet"-member is true */
    std::string      myValue;

};


/* -------------------------------------------------------------------------
 * Option_Float
 * ----------------------------------------------------------------------- */
class Option_Float : public Option
{
public:
    /** constructor; the value will be invalid (unset) */
    Option_Float();

    /** constructor; the default value is given */
    Option_Float(SUMOReal value);

    /** copy constructor */
    Option_Float(const Option_Float &s);

    /** destructor */
    ~Option_Float();

    /** assignment operator */
    Option_Float &operator=(const Option_Float &s);

    /** returns the SUMOReal value */
    SUMOReal getFloat() const;

    /** sets the given value (converts it to SUMOReal) */
    bool set(std::string v, bool isDefault=false);

    /** returns the values string-representation */
    std::string getValue() const;


private:
    /** the value, valid only when the base-classes "myAmSet"-member is true */
    SUMOReal       myValue;

};


/* -------------------------------------------------------------------------
 * Option_Bool
 * ----------------------------------------------------------------------- */
class Option_Bool : public Option
{
public:
    /** constructor; the value will be invalid (unset) */
    Option_Bool();

    /** constructor; the default value is given */
    Option_Bool(bool value);

    /** copy constructor */
    Option_Bool(const Option_Bool &s);

    /** destructor */
    ~Option_Bool();

    /** assignment operator */
    Option_Bool &operator=(const Option_Bool &s);

    /** returns the boolean value */
    bool getBool() const;

    /** sets the given value (converts it to bool) */
    bool set(bool v, bool isDefault=false);

    /** returns the values string-representation */
    std::string getValue() const;

    /** returns always true */
    bool isBool() const;


private:
    /** the value, valid only when the base-classes "myAmSet"-member is true */
    bool        myValue;

};


/* -------------------------------------------------------------------------
 * Option_FileName
 * ----------------------------------------------------------------------- */
class Option_FileName : public Option_String
{
public:
    /** constructor; the value will be invalid (unset) */
    Option_FileName();

    /** constructor; the default value is given */
    Option_FileName(std::string value);

    /** copy constructor */
    Option_FileName(const Option_String &s);

    /** destructor */
    virtual ~Option_FileName();

    /** assignment operator */
    Option_FileName &operator=(const Option_FileName &s);

    /// returns the information whether this option is a file name
    bool isFileName() const;

};


/* -------------------------------------------------------------------------
 * Option_IntVector
 * ----------------------------------------------------------------------- */
class Option_IntVector : public Option
{
public:
    /** constructor; the value will be invalid (unset) */
    Option_IntVector();

    /** constructor; the default value is given */
    Option_IntVector(const IntVector &value);

    /** constructor;
        the default value will be parsed from the string
        use ';' as delimiters */
    Option_IntVector(const std::string &value);

    /** copy constructor */
    Option_IntVector(const Option_IntVector &s);

    /** destructor */
    virtual ~Option_IntVector();

    /** assignment operator */
    Option_IntVector &operator=(const Option_IntVector &s);

    /// Returns the parsed vector of ints
    const IntVector &getIntVector() const;

    /** sets the given value (converts it to int) */
    bool set(std::string v, bool isDefault=false);

    /** returns the values string-representation */
    std::string getValue() const;

    std::string getString() const;


protected:
    IntVector myValue;
};


#endif

/****************************************************************************/


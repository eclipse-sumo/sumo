/****************************************************************************/
/// @file    Option.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Classes representing a single program option (with different types)
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
#ifndef Option_h
#define Option_h


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
#include <exception>
#include <utils/common/UtilExceptions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @typedef IntVector
 * @brief Definition of a vector of unsigned ints
 */
typedef std::vector<int> IntVector;


/* -------------------------------------------------------------------------
 * Option
 * ----------------------------------------------------------------------- */
/**
 * @class Option
 * @brief A class representing a single program option
 *
 * The base class for a single program option. All options which hold values
 *  are derived from this class as the type of stored values may differ.
 *
 * Most of the getter-methods throw exceptions because this base class is not meant
 *  to hold any values by itself. Instead, the derived classes implement the
 *  appropriate method (Option_Integer implements getInt, f.e.). So, when one
 *  tries to retrieve a value which is not of the type of the option, an
 *  exception will be thrown. This behaviour is meant to be valid, because
 *  errors like this one only occur when building and testing the application.
 *
 * Due to described behaviour, this class has no public constructors. Only
 *  construction of derived, value and type holding, classes is allowed.
 *
 *  At the begin (after being constructed) an Option either has a default value or not.
 *   In dependance to this, myHaveTheDefaultValue is set. Also, myAmSet is set to
 *   true if a default value was supported. myAmWritable is set to true,
 *   indicating that a new value may be set.
 *
 * Each option may have a description about its purpose stored. Furthermore, it
 *  stores a man-readable type name for this option.
 */
class Option
{
public:
    /** destructor */
    virtual ~Option() throw();


    /** @brief returns the information whether this options holds a valid value
     * @return Whether a value has been set
     */
    bool isSet() const throw();


    /** @brief Returns the stored SUMOReal value
     *
     * Option_Float returns the stored real number in this method's reimplementation.
     *  All other option classes do not override this method which throws an InvalidArgument-exception.
     *
     * @return Returns the stored real number if being an instance of Option_Float
     * @exception InvalidArgument If the class is not an instance of Option_Float
     */
    virtual SUMOReal getFloat() const throw(InvalidArgument);


    /** @brief Returns the stored integer value
     *
     * Option_Integer returns the stored integer number in this method's reimplementation.
     *  All other option classesdo not override this method which throws an InvalidArgument-exception.
     *
     * @return Returns the stored integer number if being an instance of Option_Integer
     * @exception InvalidArgument If the class is not an instance of Option_Integer
     */
    virtual int getInt() const throw(InvalidArgument);


    /** @brief Returns the stored string value
     *
     * Option_String and Option_FileName return the stored string in this method's reimplementation.
     *  All other option classes do not override this method which throws an InvalidArgument-exception.
     *
     * @return Returns the stored string if being an instance of Option_String
     * @exception InvalidArgument If the class is not an instance of Option_String
     */
    virtual std::string getString() const throw(InvalidArgument);


    /** @brief Returns the stored boolean value
     *
     * Option_Bool returns the stored boolean in this method's reimplementation.
     *  All other option classes do not override this method which throws an InvalidArgument-exception.
     *
     * @return Returns the stored boolean if being an instance of Option_Bool
     * @exception InvalidArgument If the class is not an instance of Option_Bool
     */
    virtual bool getBool() const throw(InvalidArgument);


    /** @brief Returns the stored integer vector
     *
     * Option_IntVector returns the stored integer vector in this method's reimplementation.
     *  All other option classes do not override this method which throws an InvalidArgument-exception.
     *
     * @return Returns the stored integer vector if being an instance of Option_IntVector
     * @exception InvalidArgument If the class is not an instance of Option_IntVector
     */
    virtual const IntVector &getIntVector() const throw(InvalidArgument);


    /** @brief Stores the given value (used for non-bool options)
     *
     * This method is overriden by all option classes but Option_Bool.
     *  The value is converted into the proper type and stored in "myValue".
     *  Then, "markSet" is called in order to know that a value has been set.
     *
     * The method defined in Option throws an exception, because all derived
     *  classes should override it - for Option_Bool, it is tested on a different
     *  position whether to use this method or bool set(bool).
     *
     * The method returns whether the value could be set (the return value from
     *  "markSet").
     *
     * If the string could not be converted into the type, an InvalidArgument
     *  is thrown.
     *
     * @return Whether the new value could be set
     * @exception InvalidArgument If being an instance of Option or Option_Bool or if the value could not be converted
     */
    virtual bool set(const std::string &v) throw(InvalidArgument);


    /** @brief Sets the value (used for bool options)
     *
     * @return Whether the new value could be set
     */
    virtual bool set(bool v) throw(InvalidArgument);


    /** @brief Returns the string-representation of the value
     *
     * The stored value is encoded into a string and returned.
     *
     * @return The stored value encoded into a string-
     */
    virtual std::string getValueString() const throw(InvalidArgument);


    /** @brief Returns the information whether the option is a bool option
     *
     * Returns false. Only Option_Bool overrides this method returning true.
     *
     * @return true if the Option is an Option_Bool, false otherwise
     */
    virtual bool isBool() const throw();


    /** @brief Returns the information whether the option holds the default value
     *
     * @return true if the option was not set from command line / configuration, false otherwise
     */
    virtual bool isDefault() const throw();


    /** @brief Returns the information whether this option is a file name
     *
     * Returns false. Only Option_FileName overrides this method returning true.
     *
     * @return true if the Option is an Option_FileName, false otherwise
     */
    virtual bool isFileName() const throw();


    /** @brief Returns the information whether the option may be set a further time
     *
     * This method returns whether the option was not already set using command line
     *  options / configuration. This is done by returning the value of myAmWritable.
     *
     * @return Whether the option may be set from the command line / configuration
     */
    bool isWriteable() const throw();


    /** OptionsCont is a friend class */
    friend class OptionsCont;


    /** @brief Returns the description of what this option does
     *
     * The description stored in myDescription is returned.
     *
     * @return The description of this option's purpose
     */
    const std::string &getDescription() const throw();


    /** @brief Returns the mml-type name of this option
     *
     * The type name stored in myTypeName is returned.
     *
     * @return The man-readable type name
     */
    virtual const std::string &getTypeName() const throw();


protected:
    /** @brief Marks the information as set
     *
     * Sets the "myAmSet" - information. Returns whether the option was writeable before.
     *
     * @return Whether the option was not set before.
     */
    bool markSet() throw();


protected:
    /** @brief Constructor
     *
     * This constructor should be used by derived classes.
     * The boolean value indicates whether a default value was supplied or not.
     *
     * @param[in] set A default value was supplied
     */
    Option(bool set=false) throw();


    /** @brief Copy constructor */
    Option(const Option &s) throw();


    /** @brief Assignment operator */
    virtual Option &operator=(const Option &s) throw();


protected:
    /// @brief A type name for this option (has presets, but may be overwritten)
    std::string myTypeName;


private:
    /** @brief information whether the value is set */
    bool myAmSet;

    /** @brief information whether the value is the default value (is then set) */
    bool myHaveTheDefaultValue;

    /** @brief information whether the value may be changed */
    bool myAmWritable;

    /// @brief The description what this option does
    std::string myDescription;

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
    /** @brief Constructor for an option with no default value
     *
     * Calls Option(false)
     */
    Option_Integer() throw();


    /** @brief Constructor for an option with a default value
     *
     * Calls Option(true)
     *
     * @param[in] value This option's default value
     */
    Option_Integer(int value) throw();


    /** @brief Copy constructor */
    Option_Integer(const Option_Integer &s) throw();


    /** @brief Destructor */
    ~Option_Integer() throw();


    /** @brief Assignment operator */
    Option_Integer &operator=(const Option_Integer &s) throw();


    /** @brief Returns the stored integer value
     * @see Option::getInt()
     * @return Returns the stored integer number
     */
    int getInt() const throw(InvalidArgument);


    /** @brief Stores the given value after parsing it into an integer
     *
     *  The value is converted into an integer and stored in "myValue".
     *  Then, "markSet" is called in order to know that a value has been set.
     *
     * The method returns whether the value could be set (the return value from
     *  "markSet").
     *
     * If the string could not be converted into an integer, an InvalidArgument
     *  is thrown.
     *
     * @see bool Option::set(std::string v)
     * @return Whether the new value could be set
     * @exception InvalidArgument If the value could not be converted into an integer
     */
    bool set(const std::string &v) throw(InvalidArgument);


    /** @brief Returns the string-representation of the value
     *
     * The stored value is encoded into a string and returned.
     *
     * @see std::string Option::getValueString()
     * @return The stored value encoded into a string
     */
    std::string getValueString() const throw(InvalidArgument);


private:
    /** the value, valid only when the base-classes "myAmSet"-member is true */
    int      myValue;

};


/* -------------------------------------------------------------------------
 * Option_String
 * ----------------------------------------------------------------------- */
class Option_String : public Option
{
public:
    /** @brief Constructor for an option with no default value
     *
     * Calls Option(false)
     */
    Option_String() throw();


    /** @brief Constructor for an option with a default value
     *
     * Calls Option(true)
     *
     * @param[in] value This option's default value
     */
    Option_String(const std::string &value) throw();


    /** @brief Copy constructor */
    Option_String(const Option_String &s) throw();


    /** @brief Destructor */
    virtual ~Option_String() throw();


    /** @brief Assignment operator */
    Option_String &operator=(const Option_String &s) throw();


    /** @brief Returns the stored string value
     * @see std::string Option::getString()
     * @return Returns the stored string
     */
    std::string getString() const throw(InvalidArgument);


    /** @brief Stores the given value
     *
     *  The value is stored in "myValue".
     *  Then, "markSet" is called in order to know that a value has been set.
     *
     * The method returns whether the value could be set (the return value from
     *  "markSet").
     *
     * @see bool Option::set(std::string v)
     * @return Whether the new value could be set
     */
    bool set(const std::string &v) throw(InvalidArgument);


    /** @brief Returns the string-representation of the value
     *
     * The stored value is encoded into a string and returned.
     *
     * @see std::string Option::getValueString()
     * @return The stored value encoded into a string
     */
    std::string getValueString() const throw(InvalidArgument);


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
    /** @brief Constructor for an option with no default value
     *
     * Calls Option(false)
     */
    Option_Float() throw();


    /** @brief Constructor for an option with a default value
     *
     * Calls Option(true)
     *
     * @param[in] value This option's default value
     */
    Option_Float(SUMOReal value) throw();


    /** @brief Copy constructor */
    Option_Float(const Option_Float &s) throw();


    /** @brief Destructor */
    ~Option_Float() throw();


    /** @brief Assignment operator */
    Option_Float &operator=(const Option_Float &s) throw();


    /** @brief Returns the stored SUMOReal value
     * @see SUMOReal Option::getFloat()
     * @return Returns the stored real number
     */
    SUMOReal getFloat() const throw(InvalidArgument);


    /** @brief Stores the given value after parsing it into a SUMOReal
     *
     *  The value is converted into a SUMOReal and stored in "myValue".
     *  Then, "markSet" is called in order to know that a value has been set.
     *
     * The method returns whether the value could be set (the return value from
     *  "markSet").
     *
     * If the string could not be converted into a SUMOReal, an InvalidArgument
     *  is thrown.
     *
     * @see bool Option::set(std::string v)
     * @return Whether the new value could be set
     * @exception InvalidArgument If the value could not be converted into a SUMOReal
     */
    bool set(const std::string &v) throw(InvalidArgument);


    /** @brief Returns the string-representation of the value
     *
     * The stored value is encoded into a string and returned.
     *
     * @see std::string Option::getValueString()
     * @return The stored value encoded into a string
     */
    std::string getValueString() const throw(InvalidArgument);


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
    /** @brief Constructor for an option with no default value
     *
     * Calls Option(false)
     */
    Option_Bool() throw();


    /** @brief Constructor for an option with a default value
     *
     * Calls Option(true)
     *
     * @param[in] value This option's default value
     */
    Option_Bool(bool value) throw();


    /** @brief Copy constructor */
    Option_Bool(const Option_Bool &s) throw();


    /** @brief Destructor */
    ~Option_Bool() throw();


    /** @brief Assignment operator */
    Option_Bool &operator=(const Option_Bool &s) throw();


    /** @brief Returns the stored boolean value
     * @see bool Option::getBool()
     * @return Returns the stored boolean
     */
    bool getBool() const throw(InvalidArgument);

    /** sets the given value (converts it to bool) */
    bool set(bool v) throw(InvalidArgument);


    /** @brief Returns the string-representation of the value
     *
     * If myValue is true, "true" is returned, "false" otherwise.
     *
     * @see std::string Option::getValueString()
     * @return The stored value encoded into a string
     */
    std::string getValueString() const throw(InvalidArgument);


    /** @brief Returns true, the information whether the option is a bool option
     *
     * Returns true.
     *
     * @see bool Option::isBool()
     * @return true
     */
    bool isBool() const throw();


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
    /** @brief Constructor for an option with no default value
     */
    Option_FileName() throw();


    /** @brief Constructor for an option with a default value
     *
     * @param[in] value This option's default value
     */
    Option_FileName(const std::string &value) throw();


    /** @brief Copy constructor */
    Option_FileName(const Option_String &s) throw();


    /** @brief Destructor */
    virtual ~Option_FileName() throw();

    /** @brief Assignment operator */
    Option_FileName &operator=(const Option_FileName &s) throw();


    /** @brief Returns true, the information whether this option is a file name
     *
     * Returns true.
     *
     * @return true
     */
    bool isFileName() const throw();

};


/* -------------------------------------------------------------------------
 * Option_IntVector
 * ----------------------------------------------------------------------- */
class Option_IntVector : public Option
{
public:
    /** @brief Constructor for an option with no default value
     */
    Option_IntVector() throw();


    /** @brief Constructor for an option with a default value
     *
     * @param[in] value This option's default value
     */
    Option_IntVector(const IntVector &value) throw();


    /** @brief Copy constructor */
    Option_IntVector(const Option_IntVector &s) throw();


    /** @brief Destructor */
    virtual ~Option_IntVector() throw();


    /** @brief Assignment operator */
    Option_IntVector &operator=(const Option_IntVector &s) throw();


    /** @brief Returns the stored integer vector
     * @see const IntVector &Option::getIntVector()
     * @return Returns the stored integer vector
     */
    const IntVector &getIntVector() const throw(InvalidArgument);


    /** @brief Stores the given value after parsing it into a vector of integers
     *
     *  The value is converted into a vector of integers and stored in "myValue".
     *  Then, "markSet" is called in order to know that a value has been set.
     *
     * The method returns whether the value could be set (the return value from
     *  "markSet").
     *
     * If the string could not be converted into a vector of integers, an InvalidArgument
     *  is thrown.
     *
     * @see bool Option::set(std::string v)
     * @return Whether the new value could be set
     * @exception InvalidArgument If the value could not be converted into a vector of integers
     */
    bool set(const std::string &v) throw(InvalidArgument);


    /** @brief Returns the string-representation of the value
     *
     * The stored value is encoded into a string and returned.
     *
     * @see std::string Option::getValueString()
     * @return The stored value encoded into a string
     */
    std::string getValueString() const throw(InvalidArgument);


private:
    /** the value, valid only when the base-classes "myAmSet"-member is true */
    IntVector myValue;
};


#endif

/****************************************************************************/


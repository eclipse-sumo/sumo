#ifndef Option_h
#define Option_h
/***************************************************************************
                          Option.h
			  A class representing a single program option
			  together with her derivates to represent different
			  value types
                             -------------------
    project              : SUMO
    begin                : Mon, 17 Dec 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.2  2002/06/11 13:43:36  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.1  2002/02/18 11:05:48  traffic
// "getValues()" now returns a string
//
// Revision 2.0  2002/02/14 14:43:27  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:18  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <exception>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * Option
 * The base class for a single program option. All options which hold values
 * are derived from this class as the type may differ.
 * When set, the value is valid. May be set with an initial, default value.
 * Usage:
 * As the options should only be used during the program initialisation, some
 * methods do output error messages to cout. Error messages may be printed,
 * when a default value is overwritten.
 * Most of the getter- and setter-methods throw exceptions as this base class
 * is not meant to hold any values by itself. Theses are stored in the derived
 * classes. This class does only supply common access for a common base class
 * while the real access methods are implemented in the derived classes.
 * The access of the proper valuetype for each of the derived class must be
 * validated by the programmer.
 * Exceptions:
 * Only the exception "InvalidArgument" from "UtilExceptions" is thrown
 */
class Option {
 private:
    /** information whether the value is set */
    bool          _set;
    /** information whether the value is the default value (is then set) */
    bool          _default;
 public:
    /** destructor */
    virtual ~Option();
    /** returns the information whether the value is valid */
    bool isSet() const;
    /** returns the float value */
    virtual float getFloat() const;
    /** returns the long value */
    virtual long getLong() const;
    /** returns the int value */
    virtual int getInt() const;
    /** returns the string value */
    virtual std::string getString() const;
    /** returns the bool value */
    virtual bool getBool() const;
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

    /** OptionsCont is a friend class as it may reinitialise the option to be a default value */
    friend class OptionsCont;
 protected:
    /** sets the "_set" - information. returns whether the option was a default option */
    bool markSet(bool isDefault);
 protected:
    /** constructor (paramter: value is valid) */
    Option(bool set=false);
    /** copy constructor */
    Option(const Option &s);
    /** assignment operator */
    virtual Option &operator=(const Option &s);
};

/**
 * Option_Integer
 * An integer-option
 */
class Option_Integer : public Option {
 private:
    /** the value, valid only when the base-classes "_set"-member is true */
    int      _value;
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
};

class Option_Long : public Option {
 private:
    /** the value, valid only when the base-classes "_set"-member is true */
    long      _value;
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
};


class Option_String : public Option {
 private:
    /** the value, valid only when the base-classes "_set"-member is true */
    std::string      _value;
 public:
    /** constructor; the value will be invalid (unset) */
    Option_String();
    /** constructor; the default value is given */
    Option_String(std::string value);
    /** copy constructor */
    Option_String(const Option_String &s);
    /** destructor */
    ~Option_String();
    /** assignment operator */
    Option_String &operator=(const Option_String &s);
    /** returns the string value */
    std::string getString() const;
    /** sets the given value */
    bool set(std::string v, bool isDefault=false);
    /** returns the values string-representation */
    std::string getValue() const;
};


class Option_Float : public Option {
 private:
    /** the value, valid only when the base-classes "_set"-member is true */
    float       _value;
 public:
    /** constructor; the value will be invalid (unset) */
    Option_Float();
    /** constructor; the default value is given */
    Option_Float(float value);
    /** copy constructor */
    Option_Float(const Option_Float &s);
    /** destructor */
    ~Option_Float();
    /** assignment operator */
    Option_Float &operator=(const Option_Float &s);
    /** returns the float value */
    float getFloat() const;
    /** sets the given value (converts it to float) */
    bool set(std::string v, bool isDefault=false);
    /** returns the values string-representation */
    std::string getValue() const;
};


class Option_Bool : public Option {
 private:
    /** the value, valid only when the base-classes "_set"-member is true */
    bool        _value;
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
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "Option.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:






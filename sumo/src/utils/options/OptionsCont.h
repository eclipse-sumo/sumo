/****************************************************************************/
/// @file    OptionsCont.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// A storage for options (typed value containers)
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
#ifndef OptionsCont_h
#define OptionsCont_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "Option.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OptionsCont
 * @brief A storage for options typed value containers)
 *
 * This container stores options (typed value containers) by their names.
 * In the case of command line tools, this container is the main interface
 *  between a user's definitions about what to do (command line options, 
 *  configuration files) and the application.
 *
 * At the begin, the application should initialise the container. Because 
 *  the OptionsCont is also responsible for printing the help screen, one
 *  should name and describe the application, first. This means that the 
 *  first usage of this container should look similar to this:
 * @code
 * OptionsCont &oc = OptionsCont::getOptions();
 * // give some application descriptions
 * oc.setApplicationDescription(<ONE_LINE_DESCRIPTION>);
 * oc.setApplicationName(<APPLICATION_NAME>, "SUMO netconvert Version " + (string)VERSION_STRING);
 * @endcode
 * @see setApplicationDescription
 * @see setApplicationName
 *
 * Then, you may also add some example calls using addCallExample.
 * @see addCallExample
 *
 * In the next step, option subtopics are registered. This is needed
 *  for the help screen and for writing the templates/saving configurations.
 *  A subtopic is added using addOptionSubTopic(<SUBTOPIC_NAME>).
 * @see addOptionSubTopic
 *
 * After this, you may add options to the container. This is done using 
 *  doRegister. This method requires a long name for the option and
 *  the value container. The value container must be an instance of
 *  one of the classes derived from "Option". Do not use Option itself!
 *  This is a base class which is meant to supply a default behaviour, 
 *  but this default behaviour throws exceptions only! 
 * @see Option
 * @see doRegister
 * @see addSynonyme
 *
 * Once stored inside this container, options will not be visible to the
 *  world and are deleted by the container. Only values and stati of the
 *  options may be returned. While accessing the options, the programmer
 *  must assure that he asks for the right value (only Option_Bool is able
 *  to return a boolean value, other option types will throw exceptions).
 *  Further, options added to the container must not be deleted outside
 *  this container (the container becomes the owner).
 *
 * For being printed in the help screen, a description together with the
 *  subtopic the option belongs to must be given to OptionsCont. This is
 *  done using addDescription(<OPTION_NAME>, <SUBTOPIC>, <DESCRIPTION>).
 * @see addDescription
 *
 * @see http://sumo.sourceforge.net/wiki/index.php/OptionsSubSystem
 */
class OptionsCont
{
public:
    /** @brief Retrieves the options */
    static OptionsCont &getOptions() throw();


    /** @brief Constructor */
    OptionsCont() throw();


    /** @brief Destructor */
    ~OptionsCont() throw();


    /** @brief Adds an option under the given name 
     * @param[in] name The (long) name of the option
     * @param[in] v The option (typed value storage)
     * @exception InvalidArgument If the name is already used
     */
    void doRegister(const std::string &name, Option *v) throw(InvalidArgument);


    /** @brief Adds an option under the given name and the given abbreviation 
     *
     * Adds the option under both names using void doRegister(const std::string &name, Option *v);
     * 
     * @param[in] name The (long) name of the option
     * @param[in] abbr The (short) name of the option
     * @param[in] v The option (typed value storage)
     * @exception InvalidArgument If one of the names is already used
     */
    void doRegister(const std::string &name, char abbr, Option *v) throw(InvalidArgument);


    /** @brief Adds a synonyme for an options name (any order) 
     *
     * Tries to find one of the synonymes. If both are known and the option differs
     *  for both, an InvalidArgument exception is thrown. If none is known, also.
     *
     * If one of the synonymes is known and the other not, the option from the known
     *  one is made accessable by the other.
     *
     * In the case both synonymes are known and have the same option assigned, nothing
     *  is done.
     *
     * @param[in] name1 The first synonyme
     * @param[in] name2 The second synonyme
     * @exception InvalidArgument If none of the synonymes or both synonymes with different options were registered before 
     */
    void addSynonyme(const std::string &name1, const std::string &name2) throw(InvalidArgument);


    /** @brief Adds a description for an option 
     *
     * Tries to retrieve the named option and to set the given description. Adds
     *  the name to the list of option names to be located in the named subtopic.
     *
     * Throws an InvalidArgument if the option is not known or already has 
     *  a description set.
     * 
     * @param[in] name The option's name
     * @param[in] subtopic The subtopic to locate the description within
     * @param[in] description The description
     * @exception InvalidArgument If none of the synonymes or both synonymes with different options were registered before 
     */
    void addDescription(const std::string &name, const std::string &subtopic,
                        const std::string &description) throw(InvalidArgument);


    /** @brief Returns the information whether the named option is known 
     * @return true if an option has been added before under the given name, false otherwise
     */
    bool exists(const std::string &name) const throw();


    /** @brief Returns the information whether the named option is set
     *
     * The named option is tried to be retrieved from the container. If
     *  it does not exist, an InvalidArgument is thrown. If it could be
     *  retrieved, the information whether the option has a value stored
     *  is returned.
     *
     * An option "is set" if a default value was supplied or a value has been
     *  set from the command line / the configuration file.
     *
     * @return true if the option has a valid value, false otherwise
     * @exception InvalidArgument If the named option is not known
     */
    bool isSet(const std::string &name) const throw(InvalidArgument);


    /** @brief Returns the information whether the named option has still the default value 
     *
     * The named option is tried to be retrieved from the container. If
     *  it does not exist, an InvalidArgument is thrown. If it could be
     *  retrieved, the information whether the option still has the default
     *  value is returned.
     *
     * An option "is default" if no value has been set from the command line 
     *  / the configuration file.
     *
     * @return true if the option still has the default value
     * @exception InvalidArgument If the named option is not known
     */
    bool isDefault(const std::string &name) const throw(InvalidArgument);


    /** @brief Returns the string-value of the named option (only for Option_String) 
     *
     */
    std::string getString(const std::string &name) const throw(InvalidArgument);

    /** returns the SUMOReal-value of the named option (only for Option_Float) */
    SUMOReal getFloat(const std::string &name) const;

    /** returns the int-value of the named option (only for Option_Intger) */
    int getInt(const std::string &name) const;

    /** returns the long-value of the named option (only for Option_Long) */
    long getLong(const std::string &name) const;

    /** returns the boolean-value of the named option
        (only for Option_Bool) */
    bool getBool(const std::string &name) const;

    /** returns the list of integer-value of the named option
        (only for Option_IntVector) */
    const IntVector &getIntVector(const std::string &name) const;

    /** returns the list of string-value of the named option
        (only for Option_String) */
    std::vector<std::string> getStringVector(const std::string &name) const;

    /** returns the named option is a list of string values containing a specified item */
    bool isInStringVector(const std::string &optionName,
                          const std::string &itemName);

    /** returns the information whether the option is a boolean option */
    bool isBool(const std::string &name) const;

    /** checks whether the named option is usable as a file list
        (with at least a single file)
        returns true if yes, false if the list is not set and throws
        a ProcessError when the list is not empty but contains only
        delimiters */
    bool isUsableFileList(const std::string &name) const;

    /** sets the given value for the named option */
    bool set(const std::string &name, const std::string &value);

    /** sets the given boolean value for the named option
        (Option_Bool only) */
    bool set(const std::string &name, bool value);

    /** returns the synonymes of an option name */
    std::vector<std::string> getSynonymes(const std::string &name) const;

    /** resets all options to be the default value */
    void resetWritable();

    /** Returns the infomration whether the named option may be set */
    bool isWriteable(const std::string &name);

    /// returns the information whether the named item is a file name
    bool isFileName(const std::string &name) const;

    /** output operator */
    friend std::ostream& operator<<(std::ostream& os, const OptionsCont& oc);

    /** removes all previous information from the container */
    void clear();

    /// Sets the application name
    void setApplicationName(const std::string &appName, const std::string &fullName);

    /// Sets the application description
    void setApplicationDescription(const std::string &appDesc);

    /// Add a call example
    void addCallExample(const std::string &example);

    /// Sets an additional message to be printed at the begin of the help screen
    void setAdditionalHelpMessage(const std::string &add);

    /// Adds an option subtopic
    void addOptionSubTopic(const std::string &topic);

    /// Prints the help
    void printHelp(std::ostream &os);

    /// Writes the configuration
    void writeConfiguration(std::ostream &os, bool filled,
                            bool complete, bool addComments);

    /// Writes a standard XML header, optionally including the configuration
    void writeXMLHeader(std::ostream &os, const bool writeConfig=true);

    /// Checks for help and configuration output, returns whether we should exit
    bool processMetaOptions(bool missingOptions);

private:
    /** returns the named option */
    Option *getSecure(const std::string &name) const;

    /** writes the warning about SUMOReal setting to cout */
    void reportDoubleSetting(const std::string &arg) const;

    /** converts an abbreviation into a name */
    std::string convertChar(char abbr) const;

    /** @brief writes the given string 'formatted' meaning that it will
     * be wrapped at ';' or ' ' whenever it is longer than a line */
    void splitLines(std::ostream &os, std::string what,
                    size_t offset, size_t nextOffset);


private:
    /// The static options container used
    static OptionsCont myOptions;

    /** definition of the type that stores the addresses of used options */
    typedef std::vector<Option*> ItemAddressContType;

    /** definition of the type that realises the access to options */
    typedef std::map<std::string, Option*> KnownContType;

    /** storage for option-addresses */
    ItemAddressContType myAddresses;

    /** access map of options */
    KnownContType myValues;

    /// some information on the application
    std::string myAppName, myFullName, myAppDescription, myAdditionalMessage;

    /// lists of call examples and option subtopics
    std::vector<std::string> myCallExamples, mySubTopics;

    /// A map from subtopic to option
    std::map<std::string, std::vector<std::string> > mySubTopicEntries;

    /// Information whether a warning a deprecated divider
    mutable bool myHaveInformedAboutDeprecatedDivider;

private:
    /**
     * @class abbreviation_finder
     * @brief A class to find abbreviated option names (length=1)
     */
    class abbreviation_finder
    {
    public:
        /** constructor */
        explicit abbreviation_finder()
        { }

        /** the comparing function */
        bool operator()(const std::string &s)
        {
            return s.length()==1;
        }
    };


private:
    /** invalid copy constructor */
    OptionsCont(const OptionsCont &s);

    /** invalid assignment operator */
    OptionsCont &operator=(const OptionsCont &s);

};


#endif

/****************************************************************************/


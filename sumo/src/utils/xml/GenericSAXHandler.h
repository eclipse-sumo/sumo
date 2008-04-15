/****************************************************************************/
/// @file    GenericSAXHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A handler which converts occuring elements and attributes into enums
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
#ifndef GenericSAXHandler_h
#define GenericSAXHandler_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <stack>
#include <sstream>
#include <vector>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <utils/common/UtilExceptions.h>
#include "SUMOXMLDefinitions.h"
#include "SUMOSAXAttributes.h"

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
 * @class GenericSAXHandler
 * @brief A handler which converts occuring elements and attributes into enums
 *
 * Normally, when using a standard SAX-handler, we would have to compare
 *  the incoming XMLCh*-element names with the ones we can parse. The same
 *  applies to parsing the attributes. This was assumed to be very time consuming,
 *  that's why we derive our handlers from this class.
 *
 * The idea behind this second handler layer was avoid repeated conversion
 *  from strings/whatever to XMLCh* and back again. The usage is quite straight
 *  forward, the only overhead is the need to define the enums - both emelents
 *  and attributes within "SUMOXMLDefinition". Still, it maybe helps to avoid typos.
 *
 * This class implements the SAX-callback and offers a new set of callbacks
 *  which must be implemented by derived classes. Instead of XMLCh*-values,
 *  element names are supplied to the derived classes as enums (SumoXMLTag).
 *
 * Also, this class allows to retrieve attributes using enums (SumoXMLAttr) within
 *  the implemented "myStartElement" method.
 *
 * Basically, GenericSAXHandler is not derived within SUMO directly, but via SUMOSAXHandler
 *  which knows all tags/attributes used by SUMO. It is still kept separate for
 *  an easier maintainability and later extensions.
 */
class GenericSAXHandler : public DefaultHandler
{
public:
    /**
     * @struct Attr
     * @brief An attribute name and its numerical representation
     */
    struct Attr {
        /// The xml-attribute-name (ascii)
        const char *name;
        /// The numerical representation of the attribute
        SumoXMLAttr key;
    };

    /**
     * @struct Tag
     * @brief A tag (element) name with its numerical representation
     */
    struct Tag {
        /// The xml-element-name (ascii)
        const char *name;
        /// The numerical representation of the element
        SumoXMLTag key;
    };


public:
    /** constructor */
    GenericSAXHandler() throw();


    /**
     * @brief Constructor
     *
     * This constructor gets the lists of known tag and attribute names with
     *  their enums (sumotags and sumoattrs in most cases). The list is closed
     *  by a tag/attribute with the enum-value SUMO_TAG_NOTHING/SUMO_ATTR_NOTHING,
     *  respectively.
     *
     * The attribute names are converted into XMLCh* and stored within an
     *  internal container. This container is cleared within the destructor.
     *
     * @param[in] tags The list of known tags
     * @param[in] attrs The list of known attributes
     *
     * @todo Why are both lists non-const and given as pointers?
     */
    GenericSAXHandler(Tag *tags, Attr *attrs) throw();


    /** @brief Destructor */
    virtual ~GenericSAXHandler() throw();


    /**
     * @brief The inherited method called when a new tag opens
     *
     * The method parses the supplied XMLCh*-qname using the internal name/enum-map
     *  to obtain the enum representation of the attribute name.
     *
     * Then, "myStartElement" is called supplying the enumeration value, the
     *  string-representation of the name and the attributes.
     *
     * @todo recheck/describe encoding of the string-representation
     * @todo do not generate and report the string-representation
     */
    void startElement(const XMLCh* const uri, const XMLCh* const localname,
                      const XMLCh* const qname, const Attributes& attrs);


    /**
     * @brief The inherited method called when characters occured
     *
     * The retrieved characters are converted into a string and appended into a
     *  private buffer. They are reported as soon as the element ends.
     *
     * @todo recheck/describe what happens with characters when a new element is opened
     * @todo describe characters processing in the class' head
     */
    void characters(const XMLCh* const chars, const unsigned int length);


    /**
     * @brief The inherited method called when a tag is being closed
     *
     * This method calls the user-implemented methods myCharacters with the previously
     *  collected and converted characters.
     *
     * Then, myEndElement is called, supplying it the qname converted to its enum-
     *  and string-representations.
     *
     * @todo recheck/describe encoding of the string-representation
     * @todo do not generate and report the string-representation
     */
    void endElement(const XMLCh* const uri, const XMLCh* const localname,
                    const XMLCh* const qname);


/*
    //{ methods for retrieving attribute values
    /**
     * @brief Returns the information whether the named (by its enum-value) attribute is within the current list
     /
    bool hasAttribute(const Attributes &attrs, SumoXMLAttr id) throw();

    /**
     * @brief Returns the information whether the named attribute is within the current list
     /
    bool hasAttribute(const Attributes &attrs, const XMLCh * const id) throw();


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
     /
    bool getBool(const Attributes &attrs, SumoXMLAttr id) const throw(EmptyData, BoolFormatException);

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
     /
    bool getBoolSecure(const Attributes &attrs, SumoXMLAttr id, bool val) const throw(EmptyData);


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
     /
    int getInt(const Attributes &attrs, SumoXMLAttr id) const throw(EmptyData, NumberFormatException);

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
     /
    int getIntSecure(const Attributes &attrs, SumoXMLAttr id, int def) const throw(EmptyData, NumberFormatException);


    /**
     * @brief Returns the string-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2str.
     *  If the attribute is ==0, TplConvert<XMLCh>::_2str throws an
     *  EmptyData-exception which is passed.
     *
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     /
    std::string getString(const Attributes &attrs, SumoXMLAttr id) const throw(EmptyData);

    /**
     * @brief Returns the string-value of the named (by its enum-value) attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2strSec.
     *  If the attribute is ==0, TplConvert<XMLCh>::_2strSec returns the default value.
     *
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     /
    std::string getStringSecure(const Attributes &attrs, SumoXMLAttr id,
                                const std::string &str) const throw(EmptyData);


    /**
     * @brief Returns the string-value of the named attribute
     *
     * Tries to retrieve the attribute from the the attribute list. The retrieved
     *  attribute  (which may be 0) is then parsed using TplConvert<XMLCh>::_2strSec.
     *  If the attribute is ==0, TplConvert<XMLCh>::_2strSec returns the default value.
     *
     * @exception EmptyData If the attribute is not known or the attribute value is an empty string
     /
    std::string getStringSecure(const Attributes &attrs, const XMLCh * const id,
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
     /
    SUMOReal getFloat(const Attributes &attrs, SumoXMLAttr id) const throw(EmptyData, NumberFormatException);

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
     /
    SUMOReal getFloatSecure(const Attributes &attrs, SumoXMLAttr id, SUMOReal def) const throw(EmptyData, NumberFormatException);

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
     /
    SUMOReal getFloat(const Attributes &attrs, const XMLCh * const id) const throw(EmptyData, NumberFormatException);
    //}
    */

protected:
    /**
     * @brief Callback method for an opening tag to implement by derived classes
     *
     * Called by "startElement" (see there).
     * @param[in] element The element that contains the characters, given as a SumoXMLTag
     * @param[in] attrs The SAX-attributes, wrapped as SUMOSAXAttributes
     * @exceptions ProcessError These method may throw a ProcessError if something fails
     */
    virtual void myStartElement(SumoXMLTag element,
                                const SUMOSAXAttributes &attrs) throw(ProcessError);


    /**
     * @brief Callback method for characters to implement by derived classes
     *
     * Called by "endElement" (see there).
     * @param[in] element The opened element, given as a SumoXMLTag
     * @param[in] chars The complete embedded character string
     * @exceptions ProcessError These method may throw a ProcessError if something fails
     */
    virtual void myCharacters(SumoXMLTag element,
                              const std::string &chars) throw(ProcessError);


    /** @brief Callback method for a closing tag to implement by derived classes
     *
     * Called by "endElement" (see there).
     * @param[in] element The closed element, given as a SumoXMLTag
     * @exceptions ProcessError These method may throw a ProcessError if something fails
     */
    virtual void myEndElement(SumoXMLTag element) throw(ProcessError);


private:
    /**
     * @brief converts from c++-string into unicode
     *
     * @todo recheck encoding
     * @param[in] name The string to convert
     * @return The string converted into a XMLCh-string
     */
    XMLCh *convert(const std::string &name) const throw();


    /**
     * @brief Converts a tag from its string into its numerical representation
     *
     * Returns the enum-representation stored for the given tag. If the tag is not
     *  known, SUMO_TAG_NOTHING is returned.
     * @param[in] tag The string to convert
     * @return The SumoXMLTag-value that represents the string, SUMO_TAG_NOTHING if the named attribute is not known
     */
    SumoXMLTag convertTag(const std::string &tag) const throw();


private:
    /// @name attributes parsing
    //@{

    // the type of the map from ids to their unicode-string representation
    typedef std::map<SumoXMLAttr, XMLCh*> AttrMap;

    // the map from ids to their unicode-string representation
    AttrMap myPredefinedTags;

    /// the map from ids to their string representation
    std::map<SumoXMLAttr, std::string> myPredefinedTagsMML;
    //@}


    /// @name elements parsing
    //@{

    // the type of the map that maps tag names to ints
    typedef std::map<std::string, SumoXMLTag> TagMap;

    // the map of tag names to their internal numerical representation
    TagMap myTagMap;
    //@}

    /// A list of character strings obtained so far to build the complete characters string at the end
    std::vector<std::string> myCharactersVector;


};


// ===========================================================================
// declarations
// ===========================================================================

/// The names of SUMO-XML elements
extern GenericSAXHandler::Tag sumotags[];

/// The names of SUMO-XML attributes
extern GenericSAXHandler::Attr sumoattrs[];


#endif

/****************************************************************************/


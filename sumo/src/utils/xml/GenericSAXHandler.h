/****************************************************************************/
/// @file    GenericSAXHandler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A handler which converts occuring elements and attributes into enums
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
class GenericSAXHandler : public DefaultHandler {
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


    /**
     * @brief Assigning a parent handler which is enabled when the specified tag is closed
     */
    void registerParent(const SumoXMLTag tag, GenericSAXHandler* handler);


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

    GenericSAXHandler* myParentHandler;
    SumoXMLTag myParentIndicator;


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


#ifndef XMLConvert_h
#define XMLConvert_H
/***************************************************************************
                          XMLConvert.h
			  Some static methods which help while working
			  with XML
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
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
// Revision 1.2  2002/05/14 04:55:40  dkrajzew
// Unexisting files are now catched independent to the Xerces-error mechanism; error report generation moved to XMLConvert
//
// Revision 1.1.1.1  2002/04/08 07:21:25  traffic
// new project name
//
// Revision 2.1  2002/03/20 08:23:17  dkrajzew
// XERCES-builtin functions for string parsing replaced by own methods
//
// Revision 2.0  2002/02/14 14:43:29  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.1  2002/02/13 15:48:21  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// ------------------------------------------
// moved to 'utils'
// ------------------------------------------
// Revision 1.7  2001/12/06 13:38:05  traffic
// files for the netbuilder
//
// Revision 1.5  2001/08/16 12:53:59  traffic
// further exception handling (now validated) and new comments
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <sax/AttributeList.hpp>
#include <sax/SAXException.hpp>
#include <sax/SAXParseException.hpp>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * XMLConvert
 * This class is needed as an interface between the XML-attributes received 
 * from the Xerces-XML-Parser and the application
 */
class XMLConvert {
 public:
    /** converts the given 0-terminated xml-string into a c-string; 
        throws XMLUngivenParameterException when the xml-string is 0 */
    static char *_2char(const XMLCh *inp);
    /** converts the given xml-string with the given length into a c-string; 
        throws XMLUngivenParameterException when the xml-string is 0 */
    static char *_2char(const XMLCh *inp, const unsigned int length);
    /** converts the given 0-terminated xml-string into a c++-string; 
        throws XMLUngivenParameterExceptionwhen the xml-string is 0 */
    static std::string _2str(const XMLCh *inp);
    /** converts the given xml-string with the given length into a c++-string; 
        throws XMLUngivenParameterExceptionwhen the xml-string is 0 */
    static std::string _2str(const XMLCh *inp, const unsigned int length);
    /** converts the given 0-terminated xml-string into a long; 
        throws XMLUngivenParameterException when the xml-string is 0 */
    static long _2long(const XMLCh *inp);
    /** converts the given xml-string with the given length into a long; 
        throws XMLUngivenParameterException when the xml-string is 0 */
    static long _2long(const XMLCh *inp, const unsigned int length);
    /** converts the given 0-terminated xml-string into an int; 
        throws XMLUngivenParameterException when the xml-string is 0 */
    static int _2int(const XMLCh *inp);
    /** converts the given xml-string with the given length into an int; 
        throws XMLUngivenParameterException when the xml-string is 0 */
    static int _2int(const XMLCh *inp, const unsigned int length);
    /** converts the given 0-terminated xml-string into a float; 
        throws XMLUngivenParameterException when the xml-string is 0 */
    static float _2float(const XMLCh *inp);
    /** converts the given xml-string with the given length into a float; 
        throws XMLUngivenParameterException when the xml-string is 0 */
    static float _2float(const XMLCh *inp, const unsigned int length);
    /** converts the given xml-string into a bool; 
        throws XMLUngivenParameterException when the xml-string is 0 */
    static bool _2bool(const XMLCh *inp);
    /** build the error message from the given descriptions and the 
	occured exception */
    static std::string buildErrorMessage(const std::string &file,
					 const std::string &type, 
					 const SAXParseException& exception);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "XMLConvert.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:



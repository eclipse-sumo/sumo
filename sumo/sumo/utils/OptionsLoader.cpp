/***************************************************************************
                          OptionsLoader.cpp
			  Loads a configuration (XML) using a SAX-Parser
			  The class itself is a derivation of the
			  SAX-HandlerBase
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.4  2002/06/10 08:33:23  dkrajzew
// Parsing of strings into other data formats generelized; Options now recognize false numeric values; documentation added
//
// Revision 1.4  2002/06/10 06:54:30  dkrajzew
// Conversion of strings (XML and c-strings) to numerical values generalized; options now recognize false numerical input
//
// Revision 1.3  2002/05/14 04:45:50  dkrajzew
// Bresenham added; some minor changes; windows eol removed
//
// Revision 1.2  2002/04/26 10:08:39  dkrajzew
// Windows eol removed
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:01  dkrajzew
// new version-free project name
//
// Revision 1.5  2002/04/09 12:20:37  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.4  2002/03/22 10:59:38  dkrajzew
// Memory leak tracing added; ostrstreams replaces by ostringstreams
//
// Revision 1.3  2002/03/20 08:50:37  dkrajzew
// Revisions patched
//
// Revision 1.2  2002/03/20 08:42:59  dkrajzew
// Better SAX-usage (incorporating the length information for character-fields)
//
// Revision 1.1  2002/02/13 15:48:19  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <iostream>
#include <vector>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include "TplConvert.h"
#include "OptionsLoader.h"
#include "OptionsCont.h"
#include "UtilExceptions.h"

#include "../utils/TplConvert.cpp"

/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
OptionsLoader::OptionsLoader(OptionsCont *oc, const char *file, bool warn, bool verbose) : _error(false), _warn(warn), _file(file), _verbose(verbose), _options(oc), _item() {
}

OptionsLoader::~OptionsLoader() {
}

void OptionsLoader::startElement(const XMLCh* const name, AttributeList& attributes) {
  _item = TplConvert<XMLCh>::_2str(name);
  if(_item=="configuration"||_item=="files"||_item=="defaults"||_item=="reports")
  _item = "";
}

void OptionsLoader::characters(const XMLCh* const chars, const unsigned int length) {
    if(_item.length()==0) return;
    string value = TplConvert<XMLCh>::_2str(chars, length);
    size_t index = value.find_first_not_of("\n\t \a");
    if(index==string::npos) return;
        if(value.length()>0) {
        try {
            bool wasDefault;
            if(_options->isBool(_item)) {
                if(value=="0"||value=="false"||value=="FALSE")
                    wasDefault = _options->set(_item, false);
                else
                    wasDefault = _options->set(_item, true);
            } else {
                wasDefault = _options->set(_item, value);
            }
            if(!wasDefault) {
                _error = true;
            }
        } catch (InvalidArgument &e) {
            _error = true;
        }
    }
}

void OptionsLoader::endElement(const XMLCh* const name) {
    _item = "";
}

void OptionsLoader::warning(const SAXParseException& exception) {
  cerr << "Warning: " << TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
  cerr << " (At line/column " << exception.getLineNumber()+1 << '/' << exception.getColumnNumber();
  _error = true;
}

void OptionsLoader::error(const SAXParseException& exception) {
  cerr << "Error: " + TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
  cerr << " (At line/column " << exception.getLineNumber()+1 << '/' << exception.getColumnNumber();
  _error = true;
}

void OptionsLoader::fatalError(const SAXParseException& exception) {
  cerr << "Error: " + TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
  cerr << " (At line/column " << exception.getLineNumber()+1 << '/' << exception.getColumnNumber();
  _error = true;
}

bool OptionsLoader::errorOccured() {
    return _error;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "OptionsLoader.icc"
//#endif

// Local Variables:
// mode:C++
// End:



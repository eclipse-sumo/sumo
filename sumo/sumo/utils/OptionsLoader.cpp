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
// Revision 1.1  2002/04/08 07:21:25  traffic
// Initial revision
//
// Revision 2.1  2002/03/20 08:50:10  dkrajzew
// The langth of the characters-field is now used
//
// Revision 2.0  2002/02/14 14:43:28  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
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
#include "XMLConvert.h"
#include "OptionsLoader.h"
#include "OptionsCont.h"
#include "UtilExceptions.h"

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
  _item = XMLConvert::_2str(name);
  if(_item=="configuration"||_item=="files"||_item=="defaults"||_item=="reports")
  _item = "";
}

void OptionsLoader::characters(const XMLCh* const chars, const unsigned int length) {
  if(_item.length()==0) return;
  string value = XMLConvert::_2str(chars, length);
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
  cerr << "Warning: " << XMLConvert::_2str(exception.getMessage()) << endl;
  cerr << " (At line/column " << exception.getLineNumber()+1 << '/' << exception.getColumnNumber();
  _error = true;
}

void OptionsLoader::error(const SAXParseException& exception) {
  cerr << "Error: " + XMLConvert::_2str(exception.getMessage()) << endl;
  cerr << " (At line/column " << exception.getLineNumber()+1 << '/' << exception.getColumnNumber();
  _error = true;
}

void OptionsLoader::fatalError(const SAXParseException& exception) {
  cerr << "Error: " + XMLConvert::_2str(exception.getMessage()) << endl;
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



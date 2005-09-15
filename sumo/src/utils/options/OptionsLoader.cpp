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
// Revision 1.6  2005/09/15 12:21:19  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 09:28:01  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2004/11/23 10:36:02  dkrajzew
// debugging
//
// Revision 1.3  2004/07/02 09:41:39  dkrajzew
// debugging the repeated setting of a value
//
// Revision 1.2  2003/02/07 10:51:59  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:58:18  dkrajzew
// initial release for utilities that handle program options
//
// Revision 1.8  2002/07/31 17:30:06  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.8  2002/07/11 07:42:59  dkrajzew
// Usage of relative pathnames within configuration files implemented
//
// Revision 1.7  2002/06/21 10:47:47  dkrajzew
// inclusion of .cpp-files in .cpp files removed
//
// Revision 1.6  2002/06/17 15:16:41  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.5  2002/06/11 15:58:26  dkrajzew
// windows eol removed
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
//#include <iostream>
#include <vector>
#include <sax/HandlerBase.hpp>
#include <sax/AttributeList.hpp>
#include <sax/SAXParseException.hpp>
#include <sax/SAXException.hpp>
#include <utils/convert/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include "OptionsLoader.h"
#include "OptionsCont.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
/*
#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG
*/

/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
   #define _INC_MALLOC       // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
OptionsLoader::OptionsLoader(OptionsCont *oc,
                             const char *file, bool verbose)
    : _error(false), _file(file), _verbose(verbose),
    _options(oc), _item()
{
}

OptionsLoader::~OptionsLoader()
{
}

void OptionsLoader::startElement(const XMLCh* const name,
                                 AttributeList& attributes)
{
    _item = TplConvert<XMLCh>::_2str(name);
    if( _item=="configuration" ||
        _item=="files" ||
        _item=="defaults" ||
        _item=="reports") {
        _item = "";
    }
}

void OptionsLoader::characters(const XMLCh* const chars,
                               const unsigned int length)
{
    if(_item.length()==0) {
        return;
    }
    string value = TplConvert<XMLCh>::_2str(chars, length);
    size_t index = value.find_first_not_of("\n\t \a");
    if(index==string::npos) {
        return;
    }
    if(value.length()>0) {
        try {
            bool isWriteable;
            if(_options->isBool(_item)) {
                if(value=="0"||value=="false"||value=="FALSE") {
                    isWriteable = setSecure(_item, false);
                } else {
                    isWriteable = setSecure(_item, true);
                }
            } else {
                if(_options->isFileName(_item)) {
                    StringTokenizer st(value, ';');
                    string conv;
                    while(st.hasNext()) {
                        if(conv.length()!=0) {
                            conv += ';';
                        }
                        string tmp = st.next();
                        if(!FileHelpers::isAbsolute(value)) {
                            tmp =
                                FileHelpers::getConfigurationRelative(
                                    _file, tmp);
                        }
                        conv += tmp;
                    }
                    isWriteable = setSecure(_item, conv);
                } else {
                    isWriteable = setSecure(_item, value);
                }
            }
            if(!isWriteable) {
                _error = true;
            }
        } catch (InvalidArgument e) {
            MsgHandler::getErrorInstance()->inform(
                e.msg());
            _error = true;
        }
    }
}


bool
OptionsLoader::setSecure(const std::string &name, bool value)
{
    if(_options->isWriteable(name)) {
        _options->set(name, value);
        return true;
    }
    return false;
}


bool
OptionsLoader::setSecure(const std::string &name, const std::string &value)
{
    if(_options->isWriteable(name)) {
        _options->set(name, value);
        return true;
    }
    return false;
}


void
OptionsLoader::endElement(const XMLCh* const name)
{
    _item = "";
}


void
OptionsLoader::warning(const SAXParseException& exception)
{
    WRITE_WARNING(TplConvert<XMLCh>::_2str(exception.getMessage()));
    WRITE_WARNING( \
        " (At line/column " \
        + toString<int>(exception.getLineNumber()+1) + '/' \
        + toString<int>(exception.getColumnNumber()) + ").");
    _error = true;
}


void
OptionsLoader::error(const SAXParseException& exception)
{
    MsgHandler::getErrorInstance()->inform(
        TplConvert<XMLCh>::_2str(exception.getMessage()));
    MsgHandler::getErrorInstance()->inform(
        " (At line/column "
        + toString<int>(exception.getLineNumber()+1) + '/'
        + toString<int>(exception.getColumnNumber()) + ").");
    _error = true;
}


void
OptionsLoader::fatalError(const SAXParseException& exception)
{
    MsgHandler::getErrorInstance()->inform(
        TplConvert<XMLCh>::_2str(exception.getMessage()));
    MsgHandler::getErrorInstance()->inform(
        " (At line/column "
        + toString<int>(exception.getLineNumber()+1) + '/'
        + toString<int>(exception.getColumnNumber()) + ").");
    _error = true;
}


bool
OptionsLoader::errorOccured()
{
    return _error;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:



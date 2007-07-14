/****************************************************************************/
/// @file    OptionsLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Loads a configuration (XML) using a SAX-Parser
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
//#include <iostream>
#include <vector>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include "OptionsLoader.h"
#include "OptionsCont.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
/*
#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS
*/


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
OptionsLoader::OptionsLoader(const char *file, bool verbose)
        : myError(false), myFile(file), myVerbose(verbose),
        myOptions(OptionsCont::getOptions()), myItem()
{}


OptionsLoader::~OptionsLoader()
{}


void OptionsLoader::startElement(const XMLCh* const name,
                                 AttributeList& /*attributes*/)
{
    myItem = TplConvert<XMLCh>::_2str(name);
    /*
    if( myItem=="configuration" ||
        myItem=="files" ||
        myItem=="defaults" ||
        myItem=="reports") {
        myItem = "";
    }
    */
}


void OptionsLoader::characters(const XMLCh* const chars,
                               const unsigned int length)
{
    if (myItem.length()==0) {
        return;
    }
    string value = TplConvert<XMLCh>::_2str(chars, length);
    size_t index = value.find_first_not_of("\n\t \a");
    if (index==string::npos) {
        return;
    }
    if (value.length()>0) {
        try {
            bool isWriteable;
            if (myOptions.isBool(myItem)) {
                if (value=="0"||value=="false"||value=="FALSE") {
                    isWriteable = setSecure(myItem, false);
                } else {
                    isWriteable = setSecure(myItem, true);
                }
            } else {
                if (myOptions.isFileName(myItem)) {
                    StringTokenizer st(value, ';');
                    string conv;
                    while (st.hasNext()) {
                        if (conv.length()!=0) {
                            conv += ';';
                        }
                        string tmp = st.next();
                        if (!FileHelpers::isAbsolute(value)) {
                            tmp = FileHelpers::getConfigurationRelative(myFile, tmp);
                        }
                        conv += tmp;
                    }
                    isWriteable = setSecure(myItem, conv);
                } else {
                    isWriteable = setSecure(myItem, value);
                }
            }
            if (!isWriteable) {
                MsgHandler::getErrorInstance()->inform("Could not set option '" + myItem + "' (probably defined twice).");
                myError = true;
            }
        } catch (InvalidArgument e) {
            MsgHandler::getErrorInstance()->inform(e.what());
            myError = true;
        }
    }
}


bool
OptionsLoader::setSecure(const std::string &name, bool value)
{
    if (myOptions.isWriteable(name)) {
        myOptions.set(name, value);
        return true;
    }
    return false;
}


bool
OptionsLoader::setSecure(const std::string &name, const std::string &value)
{
    if (myOptions.isWriteable(name)) {
        myOptions.set(name, value);
        return true;
    }
    return false;
}


void
OptionsLoader::endElement(const XMLCh* const /*name*/)
{
    myItem = "";
}


void
OptionsLoader::warning(const SAXParseException& exception)
{
    WRITE_WARNING(TplConvert<XMLCh>::_2str(exception.getMessage()));
    WRITE_WARNING(\
                  " (At line/column " \
                  + toString<int>(exception.getLineNumber()+1) + '/' \
                  + toString<int>(exception.getColumnNumber()) + ").");
    myError = true;
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
    myError = true;
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
    myError = true;
}


bool
OptionsLoader::errorOccured()
{
    return myError;
}



/****************************************************************************/


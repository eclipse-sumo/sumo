/****************************************************************************/
/// @file    AttributesReadingGenericSAX2Handler.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A combination between a GenericSAXHandler and an AttributesHandler
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
#ifndef AttributesReadingGenericSAX2Handler_h
#define AttributesReadingGenericSAX2Handler_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GenericSAX2Handler.h"
#include "AttributesHandler.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AttributesReadingGenericSAX2Handler
 * This class is a combination of an AttributesHandler and a
 * GenericSAX2Handler allowing direct access to and handling of XML-tags
 * (elements) and attributes.
 * By now, almost every class that handles XML-data is derived from this
 * class.
 */
class AttributesReadingGenericSAX2Handler : public AttributesHandler,
            public GenericSAX2Handler
{
public:
    /** constructor */
    AttributesReadingGenericSAX2Handler()
    { }

    /** parametrised constructor */
    AttributesReadingGenericSAX2Handler(GenericSAX2Handler::Tag *tags,
                                        int noTags, AttributesHandler::Attr *attrs, int noAttrs)
            :  AttributesHandler(attrs, noAttrs),
            GenericSAX2Handler(tags, noTags)
    { }

    /** destructor */
    virtual ~AttributesReadingGenericSAX2Handler()
    { }

};


#endif

/****************************************************************************/


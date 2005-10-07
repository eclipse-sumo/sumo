#ifndef AttributesReadingGenericSAX2Handler_h
#define AttributesReadingGenericSAX2Handler_h
//---------------------------------------------------------------------------//
//                        AttributesReadingGenericSAX2Handler.h -
//  A combination between a GenericSAXHandler and an AttributesHandler
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.8  2005/10/07 11:47:41  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/15 12:22:26  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.6  2005/04/28 09:02:50  dkrajzew
// level3 warnings removed
//
// Revision 1.5  2004/11/23 10:36:50  dkrajzew
// debugging
//
// Revision 1.4  2003/08/18 12:49:31  dkrajzew
// xerces 2.2 and later compatibility patched
//
// Revision 1.3  2003/03/19 08:12:16  dkrajzew
// false order of upper class instantiation patched
//
// Revision 1.2  2003/02/07 10:53:52  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "GenericSAX2Handler.h"
#include "AttributesHandler.h"

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class AttributesReadingGenericSAX2Handler
 * This class is a combination of an AttributesHandler and a
 * GenericSAX2Handler allowing direct access to and handling of XML-tags
 * (elements) and attributes.
 * By now, almost every class that handles XML-data is derived from this
 * class.
 */
class AttributesReadingGenericSAX2Handler : public AttributesHandler,
                                            public GenericSAX2Handler {
public:
    /** constructor */
    AttributesReadingGenericSAX2Handler() { }

    /** parametrised constructor */
    AttributesReadingGenericSAX2Handler(GenericSAX2Handler::Tag *tags,
        int noTags, AttributesHandler::Attr *attrs, int noAttrs)
        :  AttributesHandler(attrs, noAttrs),
           GenericSAX2Handler(tags, noTags) { }

    /** destructor */
    virtual ~AttributesReadingGenericSAX2Handler() { }

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


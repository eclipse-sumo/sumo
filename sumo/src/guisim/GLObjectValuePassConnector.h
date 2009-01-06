/****************************************************************************/
/// @file    GLObjectValuePassConnector.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// »missingDescription«
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
#ifndef GLObjectValuePassConnector_h
#define GLObjectValuePassConnector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/gui/globjects/GUIGlObject.h>
#include <microsim/MSUpdateEachTimestep.h>
#include <utils/common/ValueSource.h>
#include <utils/common/ValueRetriever.h>


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGlObject;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GLObjectValuePassConnector
 */
template<typename T>
class GLObjectValuePassConnector :
            public MSUpdateEachTimestep<GLObjectValuePassConnector<T> >
{
public:
    GLObjectValuePassConnector(GUIGlObject &o,
                               ValueSource<T> *source,
                               ValueRetriever<T> *retriever)
            : myObject(o), mySource(source), myRetriever(retriever) { }

    virtual ~GLObjectValuePassConnector() {
        delete mySource;
    }

    virtual bool updateEachTimestep(void) {
        if (!myObject.active()) {
            return false;
        }
        myRetriever->addValue(mySource->getValue());
        return true;
    }

private:
    GUIGlObject &myObject;
    ValueSource<T> *mySource;
    ValueRetriever<T> *myRetriever;

};


#endif

/****************************************************************************/


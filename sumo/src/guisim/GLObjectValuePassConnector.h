/****************************************************************************/
/// @file    GLObjectValuePassConnector.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Class passing values from a gl-object to another object
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
 * @brief Class passing values from a gl-object to another object
 * @see MSUpdateEachTimestep
 * @see GUIGlObject
 */
template<typename T>
class GLObjectValuePassConnector :
            public MSUpdateEachTimestep<GLObjectValuePassConnector<T> > {
public:
    /** @brief Constructor
     * @param[in] o The object to get the value from
     * @param[in] source The method for obtaining the value
     * @param[in] retriever The object to pass the value to
     */
    GLObjectValuePassConnector(GUIGlObject &o,
                               ValueSource<T> *source,
                               ValueRetriever<T> *retriever) throw()
            : myObject(o), mySource(source), myRetriever(retriever) { }


    /// @brief Destructor
    virtual ~GLObjectValuePassConnector() throw() {
        delete mySource;
    }


    /** @brief Passes the value to the retriever
     *
     * Retrieves the value from the object, in the case the object is active.
     *  Passes the value to the retriever.
     * @see GUIGlObject::active
     */
    virtual bool updateEachTimestep() throw() {
        if (!myObject.active()) {
            return false;
        }
        myRetriever->addValue(mySource->getValue());
        return true;
    }

private:
    /// @brief The object to get the values of (the object that must be active)
    GUIGlObject &myObject;

    /// @brief The source for values
    ValueSource<T> *mySource;

    /// @brief The destination for values
    ValueRetriever<T> *myRetriever;


};


#endif

/****************************************************************************/


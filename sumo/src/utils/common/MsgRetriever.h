/****************************************************************************/
/// @file    MsgRetriever.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 17 Jun 2003
/// @version $Id$
///
// Abstract class that defines the behaviour of a message retriever
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
#ifndef MsgRetriever_h
#define MsgRetriever_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MsgRetriever
 * @brief Abstract class that defines the behaviour of a message retriever
 * 
 * This abstract class defines the interfaces to something capable to retrieve
 *  messages reported to the MsgHandler. MsgRetriever may be added to MsgHandler
 *  and retrieve information from the application. An example usage is passing
 *  information (and warnings, errors) to the text output in guisim.
 *
 * Basically, only one method has to be implemented, "inform".
 */
class MsgRetriever
{
public:
    /** @brief Add a new message to process
     *
     * @param[in] msg The message to process
     */
    virtual void inform(const std::string &msg) = 0;


    /// @brief Destructor
    virtual ~MsgRetriever() { }


protected:
    /// @brief Standard constructor
    MsgRetriever() { }

};


#endif

/****************************************************************************/


#ifndef MSTriggeredReader_h
#define MSTriggeredReader_h
//---------------------------------------------------------------------------//
//                        MSTriggeredReader.h -
//  The basic class for classes that read triggers
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
// Revision 1.3  2003/10/01 11:30:41  dkrajzew
// hierarchy problems patched
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <microsim/MSNet.h>
#include <helpers/Command.h>
#include <utils/common/FileErrorReporter.h>
#include <helpers/PreStartInitialised.h>

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSTriggeredReader
 * Superclass for structures that read from a file where the times the next
 * reading is performed are triggered by events,
 */
class MSTriggeredReader : public PreStartInitialised {
public:
    /// Destructor
    virtual ~MSTriggeredReader();

protected:
    /// Constructor (for an abstract class)
    MSTriggeredReader(MSNet &net);

    /// Initialises the structure
    virtual void init(MSNet &net) = 0;

    /// Processes the next event
    virtual void processNext() = 0;

    /// Reads from the file
    virtual bool readNextTriggered() = 0;

protected:
    /**
     * Event type to trigger the execution of the derived strctures
     */
    class MSTriggerCommand : public Command
    {
    public:
        /// Constructor
        MSTriggerCommand(MSTriggeredReader &parent);

        /// virtual destructor
        virtual ~MSTriggerCommand( void );

        /** Execute the command and return an offset for recurring commands
            or 0 for single-execution commands. */
        virtual MSNet::Time execute();

    private:
        /// The parent reader
        MSTriggeredReader &_parent;
    };

public:
    /** @brief The trigger command executes private methods
        both the "processNext" and the "readNextTriggered" are executed where
        one of them must set "_offset" */
    friend class MSTriggeredReader::MSTriggerCommand;

protected:
    /// The offset to the next event
    MSNet::Time _offset;

private:
    /// invalidated default constructor
    MSTriggeredReader();

    /// invalidated copy constructor
    MSTriggeredReader(const MSTriggeredReader &s);

    /// invalidated assignment operator
    const MSTriggeredReader &operator=(const MSTriggeredReader &s);

};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSTriggeredReader.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


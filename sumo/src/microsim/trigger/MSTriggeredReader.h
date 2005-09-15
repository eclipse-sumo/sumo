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
// Revision 1.1  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.3  2005/02/01 09:49:25  dksumo
// got rid of MSNet::Time
//
// Revision 1.2  2005/01/06 10:48:07  dksumo
// 0.8.2.1 patches
//
// Revision 1.1  2004/10/22 12:49:31  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.3  2003/10/01 11:30:41  dkrajzew
// hierarchy problems patched
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <microsim/MSNet.h>
#include <utils/helpers/Command.h>
#include <utils/common/FileErrorReporter.h>


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSTriggeredReader
 * Superclass for structures that read from a file where the times the next
 * reading is performed are triggered by events,
 */
class MSTriggeredReader {
public:
    /// Destructor
    virtual ~MSTriggeredReader();

protected:
    /// Constructor (for an abstract class)
    MSTriggeredReader(MSNet &net);

    /** @brief Processes the next event
        Returns true if the next element shall be read (a new timestemp is then available also)
        Returns false if the action could not be accomplished; this method is then called in the next step again */
    virtual bool processNext() = 0;

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
        virtual SUMOTime execute();

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
    SUMOTime _offset;

private:
    /// invalidated default constructor
    MSTriggeredReader();

    /// invalidated copy constructor
    MSTriggeredReader(const MSTriggeredReader &s);

    /// invalidated assignment operator
    const MSTriggeredReader &operator=(const MSTriggeredReader &s);

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:


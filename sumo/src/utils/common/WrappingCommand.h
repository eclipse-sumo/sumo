/****************************************************************************/
/// @file    WrappingCommand.h
/// @author  Christian Roessel
/// @date    Thu, 20 Dec 2001
/// @version $Id:WrappingCommand.h 4699 2007-11-09 14:05:13Z dkrajzew $
///
// A wrapper for a Command function
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
#ifndef WrappingCommand_h
#define WrappingCommand_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "Command.h"


// ===========================================================================
// class definition
// ===========================================================================
/**
 * @class WrappingCommand
 * @brief A wrapper for a Command function
 *
 * In order to ease life, this class may encapsulate a method of a class which
 *  in order to be used as a Command. This allows to use a member methods
 *  of a class to be called as Commands are, avoiding that the instance itself
 *  is destroyed by the EventHandler.
 *
 * Because in some cases, the Command may live longer than the instance class,
 *  a boolean value indicates that the Command is "descheduled". It should
 *  be set via "deschedule" as soon as the class instance of which a method
 *  is encapsulated is destroyed and forces that the command (calling of this
 *  instace's method) is not executed.
 *
 * @see Design Patterns, Gamma et al.
 * @see Command
 * @see MSEventControl
 */
template< class T  >
class WrappingCommand : public Command
{
public:
    /// @brief Type of the function to execute.
    typedef SUMOTime(T::* Operation)(SUMOTime);


public:
    /**
     * @brief Constructor.
     *
     * @param[in] receiver Pointer to object of type T that will receive a call to one of it's methods.
     * @param[in] operation The objects' method that will be called on execute()
     */
    WrappingCommand(T* receiver, Operation operation) throw()
            : myReceiver(receiver), myOperation(operation),
            myAmDescheduledByParent(false) {}


    /// @brief Destructor
    ~WrappingCommand() throw() {}


    /** @brief Marks this Command as being descheduled
     *
     * A simple boolean marker ("myAmDescheduledByParent") is set which
     *  prevents this command from being executed.
     */
    void deschedule() {
        myAmDescheduledByParent = true;
    }



    /// @name Derived from Command
    /// @{

    /** @brief Executes the command.
     *
     * If the command is not descheduled, the stored method of the stored instance
     *  is called.
     *
     * @param[in] currentTime The current simulation time
     * @return The time after which the command shall be executed again, 0 if this command shall be descheduled.
     * @exception ProcessError Derived actions may throw this exception
     */
    SUMOTime execute(SUMOTime currentTime) throw(ProcessError) {
        // do not execute if the command was descheduled
        if (myAmDescheduledByParent) {
            return 0;
        }
        // execute if stil valid
        return (myReceiver->*myOperation)(currentTime);
    }
    /// @}


private:
    /// @brief The object the action is directed to.
    T* myReceiver;

    /// @brief The object's operation to perform.
    Operation myOperation;

    /// @brief Whether this command was descheduled (is invalid) and shall not be executed
    bool myAmDescheduledByParent;


};


#endif

/****************************************************************************/


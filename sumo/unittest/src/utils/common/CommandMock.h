#ifndef Command_Mock_h
#define Command_Mock_h

#include <utils/common/Command.h>

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class CommandMock
 * Mock Implementation for Unit Tests
 */
class CommandMock : public Command {
public:
    /** @brief Constructor
     */
	CommandMock() throw(){
		hasRun = false;
	}

    /// @brief Destructor
	~CommandMock() throw(){}

	/** @brief Executes the command.*/
    SUMOTime execute(SUMOTime currentTime) throw(ProcessError) {
		hasRun = true;
		return currentTime;
	} 

	/** return true, when execute was called, otherwise false */
	bool isExecuteCalled() {
		return hasRun;
	}

private:
	bool hasRun;  

};


#endif

/****************************************************************************/


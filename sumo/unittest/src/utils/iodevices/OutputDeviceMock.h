#ifndef OutputDeviceMock_h
#define OutputDeviceMock_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fstream>
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class OutputDeviceMock
 * Mock Implementation for Unit Tests 
 *
 */
class OutputDeviceMock : public OutputDevice {
public:
    /** @brief Constructor    
     */
	OutputDeviceMock() throw(IOError){}

    /// @brief Destructor
	~OutputDeviceMock() throw() {}


    /** @brief Returns the current content as a string   
     */
     std::string OutputDeviceMock::getString() throw() {
    return myStream.str();
}

protected:    
    /** @brief Returns the associated ostream    
     */
	std::ostream & OutputDeviceMock::getOStream() throw() {
		return myStream;
	}

private:
    /// the string stream
    std::ostringstream myStream;

};


#endif

/****************************************************************************/


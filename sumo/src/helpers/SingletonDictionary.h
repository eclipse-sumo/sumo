#ifndef SingletonDictionary_H
#define SingletonDictionary_H

/**
 * @file   SingletonDictionary.h
 * @author Christian Roessel
 * @date   Started on Wed May  7 12:46:35 2003
 * $Revision$ from $Date$ by $Author$
 *
 * @brief Contains the SingletonDictiaonary implementation.
 */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//


// $Log$
// Revision 1.7  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.6  2003/06/06 15:36:15  roessel
// Refined documentation.
//
// Revision 1.5  2003/06/06 13:26:40  roessel
// GPL notice added.
//
// Revision 1.4  2003/06/06 13:21:22  roessel
// Documentation refined.
//
// Revision 1.3  2003/06/06 13:16:13  roessel
// Documentation updated. vectorM not static any longer.
//
// Revision 1.2  2003/06/06 10:46:44  dkrajzew
// missing cassert-inclusion added
//
// Revision 1.1  2003/05/21 16:21:45  dkrajzew
// further work detectors
//


#include <cassert>
#include <stdexcept>
#include "Dictionary.h"


/**
 * Runtime-Exception class. An instance of SingletonNotCreated is thrown, if
 * SingletonDictionary::getInstance() is called without a previous call to
 * SingletonDictionary::create()
 *
 * @todo Move into a separate file
 */
class SingletonNotCreated : public std::runtime_error
{
public:
    /// Sole constructor
    SingletonNotCreated( const std::string& msg = "" ) : runtime_error( msg )
        {}
};


/**
 * Singleton class that holds key-value pairs and has distinct insert-
 * and find modes.
 *
 * @see Dictionary
 */
template< typename Key, typename Value >
class SingletonDictionary : public Dictionary< Key, Value >
{
public:
    /**
     * Create the sole instance of this class. The normal way for a singleton
     * class instance to be created is through the getInstance()-method. But
     * we need to pass the CleanupMode-parameter once. Calling this method
     * more than once will throw an assertion.
     *
     * @see getInstance()
     */
    static void create( void )
        {
            assert( instanceM == 0 );
            instanceM = new SingletonDictionary( );
        }

    /**
     * Get the sole instance of this class. Instead of creating this instance
     * in the first call, here the sole instance is created by calling
     * create().
     *
     * @see create()
     * @return A pointer to the sole instance of this class.
     */
    static SingletonDictionary* getInstance( void )
        {
            if ( instanceM == 0 ) {
                throw SingletonNotCreated(
                    "Instance of SingletonDictionary has not been created.");
            }
            return instanceM;
        }

    /// Destructor. Resets member instanceM.
    ~SingletonDictionary( void )
        {
            instanceM = 0;
        }
protected:
    /**
     * Constructor. Only called by create(). Set's operation mode to INSERT.
     *
     * @return A pointer to the sole instance of this class.
     * @see create()
     */
    SingletonDictionary( void )
        : Dictionary< Key, Value >( )
        {
            operationModeM = INSERT;
        }

    /// Not implemented copy-constructor
    SingletonDictionary( const SingletonDictionary& );
    /// Not implemented assignment-operator
    SingletonDictionary& operator=( const SingletonDictionary& );

private:
    static SingletonDictionary* instanceM; /**< Pointer to the sole
                                            * instance of the class */

};

//---------------------------------------------------------------------------//

// initialize static member
template< typename Key, typename Value >
SingletonDictionary< Key, Value >*
SingletonDictionary< Key, Value >::instanceM = 0;



// Local Variables:
// mode:C++
// End:

#endif // SingletonDictionary_H










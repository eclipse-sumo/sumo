#ifndef SingletonDictionary_H
#define SingletonDictionary_H

/**
 * @file   SingletonDictionary.h
 * @author Christian Roessel
 * @date   Wed May  7 12:46:35 2003
 * @version Revision $Revision$ from $Date$ by $Author$
 *
 * @brief
 *
 *
 */


// $Log$
// Revision 1.2  2003/06/06 10:46:44  dkrajzew
// missing cassert-inclusion added
//
// Revision 1.1  2003/05/21 16:21:45  dkrajzew
// further work detectors
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <cassert>
#include <stdexcept>
#include "Dictionary.h"


class SingletonNotCreated : public std::runtime_error
{
public:
    SingletonNotCreated( const std::string& msg = "" ) : runtime_error( msg )
        {}
};


/**
 * @class SingletonDictionary is derived from Dictionary. The only difference
 * is that SingletonDictionary is a global accessible Singleton.
 *
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
     * in the first call, here the sole instance is created by calling create.
     *
     * @see create()
     * @return A pointer to the sole instance of this class.
     */
    static SingletonDictionary* getInstance( void )
        {
            if ( instanceM == 0 ) {
                throw SingletonNotCreated();
            }
            return instanceM;
        }

    /// Destructor.
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










#ifndef Dictionary_H
#define Dictionary_H

/**
 * @file   Dictionary.h
 * @author Christian Roessel
 * @date   Started on Wed May  7 12:46:55 2003
 * $Revision$ from $Date$ by $Author$
 *
 * @brief Contains the Dictionary implementation.
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
// Revision 1.8  2003/09/05 15:03:34  dkrajzew
// tried to improve the building/deletion usage
//
// Revision 1.7  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.6  2003/06/06 13:26:40  roessel
// GPL notice added.
//
// Revision 1.5  2003/06/06 13:21:23  roessel
// Documentation refined.
//
// Revision 1.4  2003/06/06 13:16:13  roessel
// Documentation updated. vectorM not static any longer.
//
// Revision 1.3  2003/06/06 10:47:16  dkrajzew
// destructor changed to virtual
//
// Revision 1.2  2003/06/04 16:09:16  roessel
// setFindMode creates now a static vector which is returned by reference
// from getStdVector.
//
// Revision 1.1  2003/05/21 16:21:45  dkrajzew
// further work detectors
//


#include <map>
#include <vector>

/**
 * Class that holds key-value pairs and has distinct insert- and find modes.
 *
 * The difference to a std::map is that this class has an INSERT and a FIND
 * operation mode which cannot be mixed. On construction the mode is set
 * to insert until setFindMode() is called. During INSERT you can call
 * only the methods isInsertSuccess() and setFindMode(). During FIND the
 * methods getStdVector(), getValue() and the dtor are allowed.
 *
 * @note If you store pointers as Value, the object they are pointing to
 * is not deleted during the dtor call.
 */
template< typename Key, typename Value >
class Dictionary
{
public:
    /// Modes of operation.
    enum Mode {
        INSERT = 0,             /**< Insert-mode for inserting key-value pairs
                                 * until mode is switched by setFindMode()  */
        FIND                    /**< Find-mode is used after finishing
                                 * insertion. */
    };

    /**
     * Destructor.
     * @note If you are storing pointers you need to delete them seperately.
     * E.g. get the pointers with getStdVector() and delete them in a loop.
     * @see getStdVector()
     */
    virtual ~Dictionary( void )
        {
            assert( operationModeM == FIND );
            mapM.clear();
            vectorM.clear();
        }
    /**
     * Constructor. Sets the operationMode to INSERT.
     *
     */
    Dictionary() :
        mapM(  ),
        operationModeM( INSERT ),
        vectorM( )
        {}

    /**
     * Tries to insert a key-value pair into the dictionary. A call to this
     * method is allowed only in operationMode INSERT.
     *
     * @param aKey Key to be inserted.
     * @param aValue Corresponding value.
     *
     * @return True if insert was successful, false otherwise, i.e. a pair
     * with the same key is already in the Dictionary.
     */
    bool isInsertSuccess( Key aKey, Value aValue )
        {
            assert( operationModeM == INSERT );
			return mapM.insert( std::make_pair( aKey, aValue ) ).second;
       	}

    /**
     * Switches the operationMode from INSERT to FIND. From now on, no
     * additional inserts are allowed. The value-vector is filled with
     * the objects currently in the Dictionay.
     */
    void setFindMode( void )
        {
            //assert( operationModeM == INSERT );
            operationModeM = FIND;
            vectorM.reserve( mapM.size() );
            for ( MapIt it = mapM.begin(); it != mapM.end(); ++it ) {
                vectorM.push_back( it->second );
            }
        }

    /**
     * Definition of the returntype of getStdVector
     * @see getStdVector()
     */
    typedef std::vector< Value > ValueVector;

    /**
     * During FIND mode, get a sorted vector of the values in the Dictionary.
     *
     * @return Reference to the sorted vector of values in the Dictionary.
     */
    ValueVector& getStdVector()
        {
            assert( operationModeM == FIND );
            return vectorM;
        }
    /**
     * During FIND mode, get the value corresponding to aKey
     *
     * @param aKey Search for this key in Dictionary.
     *
     * @return Value corresponding to aKey if aKey is in Dictionary, 0
     * otherwise.
     */
    Value getValue( Key aKey )
        {
            assert( operationModeM == FIND );
            MapIt it = mapM.find( aKey );
            if ( it != mapM.end() ) {
                return it->second;
            }
            else {
                return 0;
            }
        }

    Mode getMode() const {
        return operationModeM;
    }


protected:
    /// Type of the internal map
    typedef std::map< Key, Value > Map;

    /// Type of the internal maps iterator.
    typedef typename Map::iterator MapIt;

    Map mapM; /**< Map to store the key-value pairs. */


    /**
     * Current mode of operation. Is set to INSERT by ctor and once switched to
     * FIND by setFindMode() when insertion is finished.
     *
     * @see Mode
     * @see setFindMode()
     */
    Mode operationModeM;

    /**
     * Vector the stores the sorted values of the Dictionary during FIND mode.
     * Is filled in setFindMode()
     * @setFindMode()
     */
    ValueVector vectorM;

private:

    /// Hidden copy-ctor.
    Dictionary( const Dictionary& );
    /// Hidden assignment-operator.
    Dictionary& operator=( const Dictionary& );
};

// // static member initialization
// template< typename Key, typename Value >
// Dictionary< Key, Value >::ValueVector
// Dictionary< Key, Value >::vectorM;


// Here I tried to do a specialization for all pointer types, but I failed
// on the casts in getStdVector and the dtor (deleting of void* is undefined).
// Why was there a need for specialization?
// 1. Avoiding code-bloat
// 2. I wanted the pointers in the map to be deleted on deletion of the
//    Dictionary.
// It worked except of the deletion and the getStdVector, which indeed is a
// useful method.

// template< typename Key >
// class Dictionary< Key, void* >
// {
// public:
//     /// Destructor.
//     ~Dictionary( void )
//         {
//             cout << "dtor class Dictionary< Key, void* >" << endl;

// //             assert( operationModeM == FIND );
// //             for ( MapIt it = mapM.begin(); it != mapM.end(); ++it ) {
// //                 delete it->second;
// //             }
//             mapM.clear();
//         }
//     /// Constructor
//     Dictionary( void )
//         {
//             operationModeM = INSERT;
//         }

//     bool isInsertSuccess( Key aKey, void* aValue )
//         {
//             assert( operationModeM == INSERT );
//             return mapM.insert( std::make_pair( aKey, aValue ) ).second;
//         }

//     void setFindMode( void )
//         {
//             assert( operationModeM == INSERT );
//             operationModeM = FIND;
//         }

//     std::vector< void* > getStdVector()
//         {
//             assert( operationModeM == FIND );
//             std::vector< void* > vec;
//             vec.reserve( mapM.size() );
//             for ( MapIt it = mapM.begin(); it != mapM.end(); ++it ) {
//                 vec.push_back( it->second );
//             }
//             return vec;
//         }

//     void* getValue( Key aKey )
//         {
//             assert( operationModeM == FIND );
//             MapIt it = mapM.find( aKey );
//             if ( it != mapM.end() ) {
//                 return it->second;
//             }
//             else {
//                 return 0;
//             }
//         }

// private:   // private methods

//     /// Not implemented copy-constructor
//     Dictionary( const Dictionary& );
//     /// Not implemented assignment-operator
//     Dictionary& operator=( const Dictionary& );

// protected: // protected members
//     std::map< Key, void* > mapM; /**< Map to store the key-value pairs. */

//     /// The type of an interator to the key-value pair map (for brevity)
//     typedef typename std::map< Key, void* >::iterator MapIt;

//     /// Modes of operation are defined here.
//     enum Mode {
//         INSERT = 0,             /**< Insert-mode for inserting key-value pairs
//                                  * until mode is switched by setFindMode()  */
//         FIND                    /**< Find-mode is used after finishing insertion. */
//     };

//     /**
//      * Current mode of operation. Is set to INSERT by ctor and once switched to
//      * FIND when insertion is finished.
//      *
//      * @see Mode
//      * @see setFindMode()
//      */
//     Mode operationModeM;
// };



// template< typename Key, typename Value >
// class Dictionary< Key, Value* > : private Dictionary< Key, void* >
// {
// public:
//     typedef Dictionary< Key, void* > Base;

//     ~Dictionary( void )
//         {
//             //std::vector< Value*, std::allocator< Value* > > values = getStdVector();



//         }
//     Dictionary( void ) : Base()
//         {
//         }
//     bool isInsertSuccess( Key aKey, Value* aValue )
//         {
//             return Base::isInsertSuccess( aKey, static_cast<void*>( aValue ) );
//         }
//     void setFindMode( void )
//         {
//             Base::setFindMode();
//         }
//     std::vector< Value* > getStdVector()
//         {
//  //            typedef Value* _Tp;
// //             typedef std::allocator< Value* > _Alloc;
// //             return static_cast< std::vector(
// //                 //std::_Vector_base<_Tp, _Alloc>::allocator_type& =
// //                 std::_Vector_base<_Tp, _Alloc>::allocator_type() ) >
// //                 ( Base::getStdVector() ) ;
//         }
//     Value* getValue( Key aKey )
//         {
//             return static_cast< Value* >( Base::getValue( aKey ) );
//         }

// };



// Local Variables:
// mode:C++
// End:

#endif // Dictionary_H


















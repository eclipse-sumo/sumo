#ifndef MSDETECTORCONTAINER_H
#define MSDETECTORCONTAINER_H

/**
 * @file   MSDetectorContainer.h
 * @author Christian Roessel
 * @date   Started Fri Sep 26 19:11:26 2003
 * @version $Id$
 * @brief
 *
 *
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

#include "MSDetectorContainerBase.h"
#include "MSVehicle.h"
#include "MSPredicates.h"
#include <algorithm>
#include <functional>
#include <list>

template< typename InnerContainer >
struct MSDetectorContainer : public MSDetectorContainerBase
{
    typedef typename InnerContainer::const_iterator ContainerConstIt;
    typedef InnerContainer InnerCont;

    void enterDetectorByMove( MSVehicle* veh )
        {
            typedef typename InnerContainer::value_type ContainerItem;
            containerM.push_front( ContainerItem( veh ) );
        }

    void enterDetectorByEmitOrLaneChange( MSVehicle* veh )
        {
            typedef typename InnerContainer::value_type ContainerItem;
            typedef typename InnerContainer::iterator ContainerIt;
            typedef typename Predicate::PosGreaterC< ContainerItem >
                PosGreaterPredicate;
            ContainerIt insertIt =
                std::find_if( containerM.begin(), containerM.end(),
                              std::bind2nd(
                                  PosGreaterPredicate(), veh->pos() ) );
            containerM.insert( insertIt, ContainerItem( veh ) );
        }

    void leaveDetectorByMove( MSVehicle* veh )
        {
            typedef typename InnerContainer::value_type ContainerItem;
            typedef typename InnerContainer::iterator ContainerIt;
            typedef typename Predicate::VehEqualsC< ContainerItem >
                ErasePredicate;
            ContainerIt eraseIt =
                std::find_if( containerM.begin(), containerM.end(),
                              std::bind2nd(
                                  ErasePredicate(), veh ) );
            assert(containerM.size()>0);
            assert(eraseIt!=containerM.end());
            containerM.erase( eraseIt );        
        }

    void leaveDetectorByLaneChange( MSVehicle* veh )
        {
            typedef typename InnerContainer::value_type ContainerItem;
            typedef typename InnerContainer::iterator ContainerIt;
            typedef typename Predicate::VehEqualsC< ContainerItem >
                ErasePredicate;
            ContainerIt eraseIt =
                std::find_if( containerM.begin(), containerM.end(),
                              std::bind2nd(
                                  ErasePredicate(), veh ) );
            assert(containerM.size()>0);
            assert(eraseIt!=containerM.end());
            containerM.erase( eraseIt );
        }

    virtual void update( void )
        {}

    MSDetectorContainer( void )
        : containerM()
        {}

    virtual ~MSDetectorContainer( void )
        {
            containerM.clear();
        }

    InnerContainer containerM;
};

namespace DetectorContainer
{
    typedef MSDetectorContainer< std::list< MSVehicle* > > Vehicles;
}

// #include <string>
// template< >
// struct MSDetectorContainer< std::list< MSVehicle* > > : public MSDetectorContainerBase
// {
//     typedef std::list< MSVehicle* >::const_iterator ContainerConstIt;
//     typedef std::list< MSVehicle* > InnerContainer;
//     typedef InnerContainer InnerCont;

//     void printContainer(void)
//         {
//             int i = 0;
//             for ( InnerContainer::iterator it = containerM.begin();
//                   it != containerM.end(); ++it ){
//                 std::cout << i << "-pos: " << (*it)->pos() << " from veh "
//                           << (*it)->id() << std::endl;
//                 ++i;
//             }
//         }
    
//     void enterDetectorByMove( MSVehicle* veh )
//         {
//             if (veh->id() == std::string("42")) {
//                 std::cout << "42 enterDetectorByMove"
//                           << " at step "
//                           << MSNet::getInstance()->getCurrentTimeStep()
//                           << std::endl;
//                 printContainer();
//             }
            
//             typedef InnerContainer::value_type ContainerItem;
//             containerM.push_front( ContainerItem( veh ) );
// //             std::cout << "insertByMove " << veh->id()
// //                       << " at step "
// //                       << MSNet::getInstance()->getCurrentTimeStep()
// //                       << std::endl;
// //             printContainer();
//         }

//     void enterDetectorByEmitOrLaneChange( MSVehicle* veh )
//         {
//             if (veh->id() == std::string("42")) {
//                 std::cout << "42 enterDetectorByMove"
//                           << " at step "
//                           << MSNet::getInstance()->getCurrentTimeStep()
//                           << std::endl;
//                 printContainer();
//             }           
//             typedef InnerContainer::value_type ContainerItem;
//             typedef InnerContainer::iterator ContainerIt;
//             typedef Predicate::PosGreaterC< ContainerItem >
//                 PosGreaterPredicate;
//             ContainerIt insertIt =
//                 std::find_if( containerM.begin(), containerM.end(),
//                               std::bind2nd(
//                                   PosGreaterPredicate(), veh->pos() ) );
//             containerM.insert( insertIt, ContainerItem( veh ) );
// //             std::cout << "insertByEmitOrLaneCh " << veh->id()
// //                       << " at step "
// //                       << MSNet::getInstance()->getCurrentTimeStep()
// //                       << std::endl;
// //             printContainer();
//         }

//     void leaveDetectorByMove( MSVehicle* veh )
//         {
//             if (veh->id() == std::string("42")) {
//                 std::cout << "42 enterDetectorByMove"
//                           << " at step "
//                           << MSNet::getInstance()->getCurrentTimeStep()
//                           << std::endl;
//                 printContainer();
//             }
//             typedef InnerContainer::value_type ContainerItem;
//             typedef InnerContainer::iterator ContainerIt;
//             typedef Predicate::VehEqualsC< ContainerItem >
//                 ErasePredicate;

// //             std::cout << "leaveByMove " << veh->id()
// //                       << " at step "
// //                       << MSNet::getInstance()->getCurrentTimeStep()
// //                       << std::endl;
// //             containerM.pop_back();
//             ContainerIt eraseIt =
//                 std::find_if( containerM.begin(), containerM.end(),
//                               std::bind2nd(
//                                   ErasePredicate(), veh ) );
//             assert(containerM.size()>0);
//             assert(eraseIt!=containerM.end());
//             containerM.erase( eraseIt );           
//         }

//     void leaveDetectorByLaneChange( MSVehicle* veh )
//         {
//             if (veh->id() == std::string("42")) {
//                 std::cout << "42 enterDetectorByMove"
//                           << " at step "
//                           << MSNet::getInstance()->getCurrentTimeStep()
//                           << std::endl;
//                 printContainer();
//             }
//             typedef InnerContainer::value_type ContainerItem;
//             typedef InnerContainer::iterator ContainerIt;
//             typedef Predicate::VehEqualsC< ContainerItem >
//                 ErasePredicate;
// //             std::cout << "leaveBylaneCh " << veh->id()
// //                       << " at step "
// //                       << MSNet::getInstance()->getCurrentTimeStep()
// //                       << std::endl;
// //             std::cout << "size " << containerM.size() << std::endl;
//             ContainerIt eraseIt =
//                 std::find_if( containerM.begin(), containerM.end(),
//                               std::bind2nd(
//                                   ErasePredicate(), veh ) );
//             assert(containerM.size()>0);
//             assert(eraseIt!=containerM.end());
//             containerM.erase( eraseIt );
//         }

//     virtual void update( void )
//         {}

//     MSDetectorContainer( void )
//         : containerM()
//         {}

//     virtual ~MSDetectorContainer( void )
//         {
//             containerM.clear();
//         }

//     InnerContainer containerM;



// };

#endif // MSDETECTORCONTAINER_H

// Local Variables:
// mode:C++
// End:

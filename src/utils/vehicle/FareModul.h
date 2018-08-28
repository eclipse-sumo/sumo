/****************************************************************************/
//Lizenzfrage ungeklärt
/****************************************************************************/
/// @file    FareModul.h
/// @author Ricardo Euler
/// @date    Thu, 17 August 2018
/// @version $Id$
///
// Fare Modul for calculating prices during intermodal routing
/****************************************************************************/

#include <cassert>
#include <string>
#include <vector>
#include "FareToken.h"
#include <utils/common/ToString.h> //needed by IntermodalEdge.h
#include <microsim/MSEdge.h>
#include "IntermodalEdge.h"
#include "StopEdge.h"
#include "PedestrianEdge.h"
#include "PublicTransportEdge.h"
#include "AccessEdge.h"
#include "FareZones.h"
#ifndef SUMO_FAREMODUL_H
#define SUMO_FAREMODUL_H


class ZoneCounter
{
public:
  
  explicit ZoneCounter(unsigned int ct) :
      myCount(ct) {
    
  }
  
  inline  void addZone(int zoneNumber)
  {
    uint64_t repNum = fareZoneToRep[ zoneNumber ];
    //assert power of 2
    assert( bitcount(repNum) == 1 );
    myCount = myCount | repNum;
  }
  
  
  int numZones() const{
    return bitcount(myCount);
  }

  
private:
   inline int bitcount(unsigned long int intVal ) const
  {
    int count = 0;
    uint64_t counter = intVal;
    
    while( counter != 0 )
    {
      counter = counter & (counter - 1);
      ++count;
    }
    return count;
  }

private:
  uint64_t myCount;
  
  
};



/**
 * A fare state collects all the information that is necessary to compute the price. Is used as an edge label
 * in IntermodalRouter
 */
struct FareState
{
  
  friend class FareModul;
  
public:
  
  /** default constructor for unlabeled edges**/
  explicit  FareState():
    myFareToken( FareToken::None ),
    myCounter( std::numeric_limits<int>::max() ),
    myTravelledDistance(std::numeric_limits<double>::max() ),
    myVisistedStops(std::numeric_limits<int>::max() )
  {
  };
  
  /**
   *
   * @param token
   */
  explicit FareState(FareToken token):
  myFareToken( token ),
  myCounter(0),
  myTravelledDistance(0),
  myVisistedStops(0) {}
  
  /** Destructor **/
  ~FareState() = default;
  
  /**
   * returns true if fare state is set and not on default
   * @return if state is set
   */
      bool isValid() const {
    return !(myFareToken == FareToken::None);
  }
  
private:
  
  /** fare token **/
  FareToken myFareToken;
  /** zone counter **/
  ZoneCounter myCounter;
  /** travelled distance in km**/
  double myTravelledDistance;
  /**num of visited stops**/
  int myVisistedStops;
  
};



struct Prices
{
  /** Prices for zones **/
  std::vector<double> zonePrices = {1.9,3.4,4.9,6.2,7.7,9.2};
  double  halle = 2.3;
  double leipzig = 2.7;
  double t1 = 1.5;
  double t2 = 1.6;
  double t3 = 1.6;
  double shortTrip = 1.6;
  double shortTripLeipzig = 1.9;
  double shortTripHalle= 1.7;
  double maxPrice = 10.6;
};


/**
 * The fare modul responsible for calculating prices
 */
class FareModul : public EffortCalculator<IntermodalEdge<MSEdge, MSLane, MSJunction, SUMOVehicle>>
{
protected:
  typedef IntermodalEdge<MSEdge, MSLane, MSJunction, SUMOVehicle> _IntermodalEdge;
  typedef StopEdge<MSEdge, MSLane, MSJunction, SUMOVehicle> _StopEdge;
  typedef PublicTransportEdge<MSEdge, MSLane, MSJunction, SUMOVehicle> _PublicTransportEdge;
  typedef PedestrianEdge<MSEdge, MSLane, MSJunction, SUMOVehicle> _PedestrianEdge;
  typedef AccessEdge<MSEdge, MSLane, MSJunction, SUMOVehicle> _AccessEdge;
public:
  
  /** Constructor ***/
  explicit FareModul() :
  myFareStates(),
  myEdges(nullptr)
  {};
  
   /**Implementation of EffortCalculator **/
  void init(const std::vector<_IntermodalEdge*>& edges) override
  {
    myFareStates.resize(edges.size());
    myEdges = &edges;
  }
  
  /**Implementation of EffortCalculator **/
  double getEffort(_IntermodalEdge const *  edge ) const override
  {
    double  effort = 0;
    FareState const & state =  myFareStates.at( (size_t) edge->getNumericalID() );
    if( state.isValid() ){
      effort = computePrice( state );
    }
    else{
      effort = std::numeric_limits<double>::max();
    }
    return effort;
  }
  
  /** Implementation of EffortCalculator **/
  void update(_IntermodalEdge const *  edge, _IntermodalEdge const *  prev)  override
  {
    std::string const & edgeType = edge->getLine();
    
    //get propagated fare state
    FareState const & state  = myFareStates.at( (size_t) prev->getNumericalID() );
    
    //treat  public transport edges
    if( edgeType.c_str()[0] != '!')
    {
      auto publicTransportEdge = dynamic_cast<_PublicTransportEdge const *>(edge);
      updateFareState(state,*publicTransportEdge);
      return;
    }
    
    //treat stop edges
    if( edgeType ==  "!stop" )
    {
      auto stopEdge = dynamic_cast<_StopEdge const *>(edge);
      updateFareState(state,*stopEdge);
      return;
    }
    
    //treat ped edges
    if( edgeType == "!ped")  {
      auto pedestrianEdge = dynamic_cast<_PedestrianEdge const *>(edge);
      updateFareState(state,*pedestrianEdge);
      return;
    }
    
    if( edgeType == "!access" ) {
      
      auto accessEdge = dynamic_cast<_AccessEdge const *>(edge);
      updateFareState(state,*accessEdge);
      return;
    }
    
    updateFareState(state,*edge);
    
  }
  
  /** Implementation of EffortCalculator
   *  _IntermodalEdge should be an Connector Edge  **/
  void setInitialState(_IntermodalEdge const * edge) override
  {
    assert( edge->getLine() == "!connector");
    
    int id = edge->getNumericalID();
    
    myFareStates[id] = FareState(FareToken::Free);
    
  }


private:
  /** List of all fare states **/
  std::vector<FareState> myFareStates;
  
  /** List of all edges **/
  std::vector<_IntermodalEdge*> const * myEdges;
  
  /** List of the prices **/
  Prices prices;
  
      double computePrice(FareState fareState) const
      {
        switch(fareState.myFareToken)
        {
          case FareToken ::H:
            return prices.halle;
          case FareToken ::L:
            return prices.leipzig;
          case FareToken ::T1:
            return prices.t1;
          case FareToken ::T2:
            return prices.t2;
          case FareToken ::T3:
            return prices.t3;
          case FareToken::U:
            return prices.zonePrices[0];
          case FareToken ::Z:
            return prices.zonePrices[fareState.myCounter.numZones() - 1];
          case FareToken ::M:
            return prices.maxPrice;
          case FareToken ::K:
            return prices.shortTrip;
          case FareToken ::KL:
            return prices.shortTripLeipzig;
          case FareToken ::KH:
            return prices.shortTripHalle;
          case FareToken ::KHU:
            return prices.shortTripHalle;
          case FareToken ::KLU:
            return prices.shortTripLeipzig;
          case FareToken::Free:
            return 0;
          case FareToken::ZU  :
          case FareToken::None:
            assert(false);
            
        }

      }

      
  /** Collects faretoken at stopedge and determines new fare state **/
  inline void updateFareState( FareState const & currentFareState, _StopEdge const & e );

  /**updates the travelled distance **/
  inline void updateFareState( FareState const & currentFareState, _PedestrianEdge const & e );
  
  /**Destroys short-trip prices if it is the second time accessing public transport **/
  inline void updateFareState( FareState const & currentFareState, _AccessEdge const & e);
  
  /**Only propagates the fare state w/o any changes **/
  inline void updateFareState( FareState const & currentFareState, _IntermodalEdge const & e);
  
  /**Only propagates the fare state w/o any changes **/
  inline void updateFareState( FareState const & currentFareState, _PublicTransportEdge const & e );
  
};


void FareModul::updateFareState( FareState const & currentFareState, _StopEdge const & e )
{
  
  FareToken  collectedToken = e.getFareToken();
  
  //if station has no fare information, just propagate
  if( collectedToken  == FareToken::None ){
    std::cout<<"Progagating fare state for stop w/o a price!"<<std::endl;
    return;
  }
  
  FareToken const & token = currentFareState.myFareToken;
  
  FareState & stateAtE = myFareStates[e.getNumericalID()];
  
  stateAtE = currentFareState;
  
  stateAtE.myCounter.addZone( e.getFareZone() );

  switch (token)
  {
    case FareToken ::Free:
      stateAtE.myFareToken = e.getStartToken();
      break;
    case FareToken::M :
      break;

    case FareToken::Z :
      if( currentFareState.myCounter.numZones() > 6 )
        stateAtE.myFareToken = FareToken::M;
      break;

    case FareToken::T1 :
    case FareToken::T2 :
    case FareToken::T3 :
      if( collectedToken == FareToken::Z )
        stateAtE.myFareToken = currentFareState.myTravelledDistance<=4 ? FareToken::K : FareToken::Z;
      break;
    case FareToken::U :
      if( collectedToken == FareToken::H)
        stateAtE.myFareToken = FareToken::H;
      if( collectedToken == FareToken::L)
        stateAtE.myFareToken = FareToken::H;
      if( collectedToken == FareToken::Z)
        stateAtE.myFareToken = FareToken::Z;
      break;
    case FareToken::H:
    case FareToken::L:
      if(collectedToken ==FareToken::Z)
        stateAtE.myFareToken = FareToken::Z;

    case FareToken::KH:
      if( currentFareState.myVisistedStops <= 4 ){
        if( collectedToken != FareToken::H)
          stateAtE.myFareToken = FareToken::KHU;
      }
      else
      {
        if( collectedToken == FareToken::H )
          stateAtE.myFareToken=FareToken ::H;
        if (collectedToken == FareToken::Z)
          stateAtE.myFareToken = FareToken ::Z;
        if (collectedToken == FareToken::U)
          stateAtE.myFareToken = FareToken ::U;
      }
      break;
    case FareToken::KL:
      if( currentFareState.myVisistedStops <= 4 ){
        if( collectedToken != FareToken::L)
          stateAtE.myFareToken = FareToken::KLU;
      }
      else
      {
        if( collectedToken == FareToken::L )
          stateAtE.myFareToken=FareToken ::L;
        if (collectedToken == FareToken::Z)
          stateAtE.myFareToken = FareToken ::Z;
        if (collectedToken == FareToken::U)
          stateAtE.myFareToken = FareToken ::U;
      }
      break;
    case FareToken::K:
      if( currentFareState.myTravelledDistance > 4  )
      {
        if( collectedToken == FareToken::U )
          stateAtE.myFareToken = FareToken ::U;
        if( collectedToken == FareToken::Z)
        {
          stateAtE.myFareToken = FareToken ::Z;
        }
      }
      break;
    case FareToken::KHU :
    case FareToken::KLU :
      if(currentFareState.myVisistedStops > 4 )
      {
        if( collectedToken == FareToken::U )
        stateAtE.myFareToken = FareToken::U;
      }
      else
      {
        stateAtE.myFareToken = FareToken::Z;
      }
      break;
    case FareToken::ZU :
      if( collectedToken == FareToken::U ){
        stateAtE.myFareToken=FareToken::U;
      }
      else {
        stateAtE.myFareToken=FareToken::Z;
      }

      break;
    case FareToken::None:
      std::cout<<"Reached invalid position in fareToken selection!"<<std::endl;
      assert(false);
      break;
  }
  stateAtE.myVisistedStops++;
};


void FareModul::updateFareState(FareState const & currentFareState, _PedestrianEdge const & e)
{
  
  //only propagates the fare state
  FareState & stateAtE = myFareStates[e.getNumericalID()];
  
  stateAtE = currentFareState;
  
}



void FareModul::updateFareState(FareState const & currentFareState, _PublicTransportEdge const & e ) {
  
  
  if( currentFareState.myFareToken == FareToken::None )
    return;
  
  FareState & stateAtE = myFareStates[e.getNumericalID()];
  
  stateAtE = currentFareState;
  
  stateAtE.myTravelledDistance += e.getLength();
}

void FareModul::updateFareState(FareState const & currentFareState, _IntermodalEdge const & e ) {
  
  if( currentFareState.myFareToken == FareToken::None )
    return;

  FareState & stateAtE = myFareStates[e.getNumericalID()];

  stateAtE = currentFareState;

}

void FareModul::updateFareState(FareState const & currentFareState, const _AccessEdge &e) {
  
  FareToken const & token = currentFareState.myFareToken;
  
  FareState & stateAtE = myFareStates[e.getNumericalID()];
  
  stateAtE = currentFareState;
  
  switch (token){
    
    case FareToken::Free ://we have not yet taken public transport
      break;
    case  FareToken::K :
      stateAtE.myFareToken = FareToken::Z;
      break;
    case  FareToken::KH :
      stateAtE.myFareToken = FareToken::H;
      break;
    case  FareToken::KL :
      stateAtE.myFareToken = FareToken::L;
      break;
    case  FareToken::KLU :
      stateAtE.myFareToken = FareToken::ZU;
      break;
    case  FareToken::KHU:
      stateAtE.myFareToken = FareToken::ZU;
      break;
    default:
      return;
  }
  

}



#endif //SUMO_FAREMODUL_H

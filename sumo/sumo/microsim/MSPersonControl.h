#ifndef MSPersonWalkingControl_h
#define MSPersonWalkingControl_h
/***************************************************************************
                          MSPersonWalkingControl.h
			  The container for people walking
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// $Log$
// Revision 1.1  2002/04/08 07:21:23  traffic
// Initial revision
//
// Revision 2.0  2002/02/14 14:43:19  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.5  2002/02/05 13:51:52  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.4  2001/12/07 11:11:55  traffic
// Inner class SameDepartureTimeCont must be declared as public to gain 
// access by the inner class equal and my_greater. fails on SunCC only.
//
// Revision 1.3  2001/11/15 17:12:14  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.2  2001/11/14 10:49:08  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/23 09:32:26  traffic
// person route implementation
//
//

#include <vector>
#include "MSNet.h"

class MSPerson;

/**
  * The class is used to handle person which are not using a transportation system but are walking or waiting while both is processed by waiting for the arrival time / the time the waiting is over.
  */
class MSPersonControl {
public:
  /**
    * The class holds the list of persons which all arrive/stop waiting at the specified time
    */
  class SameDepartureTimeCont {
  private:
    /// the time of the arrival at the destination / the end of the waiting
    unsigned int m_uiArrivalTime;
    /// the list of persons walking/waiting
    MSNet::PersonCont *m_pPersons;
  public:
    /// constructor
    SameDepartureTimeCont();
    /// constructor
    SameDepartureTimeCont(unsigned int time);
    /// destructor
    ~SameDepartureTimeCont();
    /// adds a person to the list of walking/waiting persons
    void add(MSPerson *person);
    /// extends the list of walking/waiting persons by the given list of persons
    void add(MSNet::PersonCont *cont);
    /// returns the arrival time
    unsigned int getTime();
    /// returns the container of persons walking/waiting
    MSNet::PersonCont *getPersons();
  };

private:
  /**
   * The class that performs the checking whether the current item in a search
   * is smaller or equal to a time
   */
  class my_greater : public std::unary_function<SameDepartureTimeCont, bool> {
  public:
    my_greater(unsigned int value) : m_value(value){}
    bool operator() (SameDepartureTimeCont arg) const { return m_value > arg.getTime(); }
  private:
    unsigned int m_value;
  };

private:
  /**
   * The class that performs the checking whether the current item in a search
   * is equal to a time
   */
  class equal : public std::unary_function<SameDepartureTimeCont, bool> {
  public:
    equal(unsigned int value) : m_value(value){}
    bool operator() (SameDepartureTimeCont arg) const { return m_value == arg.getTime(); }
  private:
    unsigned int m_value;
  };

  public:
  typedef vector<SameDepartureTimeCont> WaitingPersons;
private:
  /// the list of walking persons togethe 
  WaitingPersons m_pWaiting;
public:
  /// constructor (the walking person are passed)
  MSPersonControl(WaitingPersons &cont);
  /// destructor
  ~MSPersonControl();
  /// adds a single person to the list of walking persons
  void add(MSNet::Time now, MSPerson *person);
  /// returns the list of persons which waiting or walking period is over
  MSNet::PersonCont *getPersons(MSNet::Time time);
private:
  /// adds the list of persons walking until the specified time
  void push(unsigned int time, MSNet::PersonCont *list);
  /// adds the timedlist of persons walking until the specified time
  void push(SameDepartureTimeCont cont);
};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

//#ifndef DISABLE_INLINE
//#include "MSPersonControl.iC"
//#endif

#endif

// Local Variables:
// mode:C++
// End:


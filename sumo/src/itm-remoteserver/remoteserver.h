#ifndef __REMOTESERVER_H_
#define __REMOTESERVER_H_

// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4290)
#endif

#include "itm-remoteserver/storage.h"
#include "utils/common/SUMOTime.h"

#include <map>
#include <string>

namespace itm
  {
  class RemoteException
    {
    private:
      std::string what_;

    public:
      RemoteException( std::string what )
          : what_(what)
      {}
      std::string what() const
        {
          return what_;
        }
    };

// RemoteServer
// Allows communication of sumo with external program. The external
// program will control sumo.
// @author: Thimor Bohn <bohn@itm.uni-luebeck.de>
  class RemoteServer
    {
    public:
      // Constructor
      // @param port defines on which port the server is listening on
      // @param endTime defines at which time the simulation stops
      // @param penetration defines how many vehicles are included
      RemoteServer(int port, SUMOTime endTime, float penetration);

      // Destructor
      // final cleanup
      virtual ~RemoteServer(void);

      // start server
      void run();

      // process command simStep
      // step forward in simulation until targetTime or endTime reached
      // report node positions if wanted
      // @param in contains unparsed parameters targetTime, ResultType
      // @param out contains node positions ready for output
      void simStep(Storage &in, Storage &out) throw(RemoteException);

    private:
      // port on which server is listening on
      int port_;

      // simulation end time
      SUMOTime endTime_;

      // penetration rate, measurement of equipped vehicles in simulation
      float penetration_;

      //  maps all internal vehicle ids to external id if equipped else to -1
      std::map<std::string, int> equippedVehicles_;

      // hold number of all equipped vehicles
      int numEquippedVehicles_;
    };
}


#endif


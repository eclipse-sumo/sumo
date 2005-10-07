//---------------------------------------------------------------------------

#ifndef MSDevice_CPhoneH
#define MSDevice_CPhoneH
//---------------------------------------------------------------------------
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <utils/helpers/Command.h>


// class in order to expand the MSVehicle-class concerning the availability and the usage state
// of cellphones carried along.
//class MSDevice_CPhoneBroadcastCell;
class MSDevice_CPhone
{
public:
    enum State {
        STATE_OFF,
        STATE_IDLE,
        STATE_CONNECTED
    };

    struct CPhoneBroadcastCell {
        int m_CellID;
        int m_LoS;          //Level of Service
    };

    MSDevice_CPhone();
    ~MSDevice_CPhone();
    const std::vector<CPhoneBroadcastCell*> &GetProvidedCells() const;
    State GetState() const;
    int SetProvidedCells(const std::vector<CPhoneBroadcastCell*> &ActualCells);
    //int SetState(int ActualState);

    SUMOTime changeState();

protected:
    class MyCommand : public Command
    {
    public:
        /// Constructor
        MyCommand(MSDevice_CPhone &parent);

        /// virtual destructor
        virtual ~MyCommand( void );

        /** Execute the command and return an offset for recurring commands
            or 0 for single-execution commands. */
        virtual SUMOTime execute();

    private:
        /// The parent reader
        MSDevice_CPhone &myParent;
    };

private:

  //the State the cellphone (if available) is in
  //0: no cellphone; 1: turned off; 2: idle ; 3: connected
    int m_PhoneCount;
    State m_State;
  //the best 6 of the available broadcast cells; index "0" represents the actual serving cell
    std::vector<CPhoneBroadcastCell*> m_ProvidedCells;

};
#endif

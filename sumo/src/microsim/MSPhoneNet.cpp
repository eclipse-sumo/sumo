#include "MSPhoneNet.h"


MSPhoneNet::MSPhoneNet(){
	_read_Stat_File=false;
	OptionsCont &oc = OptionsSubSys::getOptions();
	std::string fname = oc.getString("ss2-cellload-file");
	if(fname.size()>0){
		_fstat.open( fname.c_str(), std::ios_base::in);
		if(_fstat.is_open()){
			_read_Stat_File=true;

		}
		_currTime = 0;
	}
	_LAIntervall = 300;
	_CellIntervall = 300;
}

MSPhoneNet::~MSPhoneNet(){
    std::map< int, MSPhoneCell* >::iterator cit;
    for ( cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end(); cit++ )
        delete cit->second;
    std::map< int, MSPhoneLA* >::iterator lit;
    for ( lit = _mMSPhoneLAs.begin(); lit != _mMSPhoneLAs.end(); lit++ )
        delete lit->second;
}
    
MSPhoneCell*
MSPhoneNet::getMSPhoneCell( int id ){
   // std::map< int, MSPhoneCell* >::iterator cit;
    if ( _mMSPhoneCells.find( id ) != _mMSPhoneCells.end() )
        return _mMSPhoneCells[id];
    else
        return 0;
}

MSPhoneCell*
MSPhoneNet::getcurrentVehicleCell( std::string id ){
	MSPhoneCell * ret = 0;
	//std::map< int, MSPhoneCell* >::iterator cit;
	for ( cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end() ; cit++ ){
		if ( cit->second->getCall( id ) ){
			ret =  cit->second;
			break;
		}
	}
	return ret;
}
MSPhoneLA*
MSPhoneNet::getcurrentVehicleLA( std::string id ){
	MSPhoneLA * ret = 0;
	for(lit=_mMSPhoneLAs.begin(); lit!=_mMSPhoneLAs.end(); lit++){
		if(lit->second->getCall(id))
			ret = lit->second;
		break;
	}
	return ret;
}

void
MSPhoneNet::addMSPhoneCell( int id )
{
    MSPhoneCell* c = new MSPhoneCell( id );
    _mMSPhoneCells[id] = c;

}

void
MSPhoneNet::addMSPhoneCell( int id, int la )
{
	cit = _mMSPhoneCells.find( id );
    if ( cit == _mMSPhoneCells.end() )
    {
		MSPhoneCell* c = new MSPhoneCell( id );
		_mMSPhoneCells[id] = c;
		_mCell2LA[id] = la;
	}
    lit = _mMSPhoneLAs.find( la );
    if ( lit == _mMSPhoneLAs.end() )
    {
		MSPhoneLA* l = new MSPhoneLA( la, 0 );
		_mMSPhoneLAs[la] = l;
	}
}

void
MSPhoneNet::remMSPhoneCell( int id )
{
    std::map< int, MSPhoneCell* >::iterator cit = _mMSPhoneCells.find( id );
    if ( cit != _mMSPhoneCells.end() )
    {
        delete cit->second;
        _mMSPhoneCells.erase( id );
    }
}

MSPhoneLA*
MSPhoneNet::getMSPhoneLA( int id )
{

    std::map<int, int>::iterator it;
	it = _mCell2LA.find(id);
	if ( it != _mCell2LA.end()){
		if ( _mMSPhoneLAs.find( it->second ) != _mMSPhoneLAs.end() )
			return _mMSPhoneLAs[it->second];
	    else
	        return 0;
	}
	else
		return 0;
}

void
MSPhoneNet::addMSPhoneLA( int id, int dir )
{
    MSPhoneLA* la = new MSPhoneLA( id, dir );
    _mMSPhoneLAs[id] = la;
}

void
MSPhoneNet::remMSPhoneLA( int id )
{
    std::map< int, MSPhoneLA* >::iterator lit = _mMSPhoneLAs.find( id );
    if ( lit != _mMSPhoneLAs.end() )
    {
        delete lit->second;
        _mMSPhoneLAs.erase( id );
    }
}

void
MSPhoneNet::writeOutput( SUMOTime t ){
	if ( MSCORN::wished( MSCORN::CORN_OUT_CELL_TO_SS2 ) && ( t % _CellIntervall  ) == 0 ){
		std::map< int, MSPhoneCell* >::iterator cit;
		for ( cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end(); cit++ ){
			cit->second->writeOutput( t);
		}
    }
	if( MSCORN::wished( MSCORN::CORN_OUT_LA_TO_SS2 ) && ( t % _LAIntervall ) == 0 ){
		std::map< int, MSPhoneLA* >::iterator lit;
		for ( lit = _mMSPhoneLAs.begin(); lit != _mMSPhoneLAs.end(); lit++ ){
			lit->second->writeOutput( t );
		}
	}
	/*the same but for the sql version*/
	if ( MSCORN::wished( MSCORN::CORN_OUT_CELL_TO_SS2_SQL ) && ( t % _CellIntervall  ) == 0 ){
		std::map< int, MSPhoneCell* >::iterator cit;
		for ( cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end(); cit++ ){
			cit->second->writeSQLOutput( t);
		}
    }
	if( MSCORN::wished( MSCORN::CORN_OUT_LA_TO_SS2_SQL ) && ( t % _LAIntervall ) == 0 ){
		std::map< int, MSPhoneLA* >::iterator lit;
		for ( lit = _mMSPhoneLAs.begin(); lit != _mMSPhoneLAs.end(); lit++ ){
			lit->second->writeSQLOutput( t );
		}
	}
	setCellStatData();
}

void
MSPhoneNet::setCellStatData(){
	if(_read_Stat_File){
		SUMOTime time=0;
		while(_currTime == time ){
		std::string bline;
		char *buffer;
		if(_fstat.eof())
			return;
		std::getline(_fstat, bline);
		strcpy(buffer, bline.c_str());
		}
	}
}
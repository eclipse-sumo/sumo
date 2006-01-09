#include "DFRONet.h"

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)


#include <iostream>

DFRONet::DFRONet()
{
	ro = NULL;
}

DFRONet::DFRONet(RONet * Ro)
{
	ro = Ro;
}

DFRONet::~DFRONet()
{
	if ( ro != NULL )
		delete ro;
}

void
DFRONet::getApproach()
{
	std::vector<ROEdge *> r = ro->getMyEdgeCont()->getTempVector();
	std::vector<ROEdge *>::iterator rit = r.begin();
	for ( ; rit != r.end(); rit++ )
	{
		size_t i = 0;
        size_t length_size = (*rit)->getNoFollowing();
        for(i=0; i<length_size; i++)
		{
            ROEdge *help = (*rit)->getFollower(i);

			if(approaching.find(help->getID())==approaching.end()) {
                approaching[help->getID()] = std::vector<std::string>();
            }
			approaching[help->getID()].push_back((*rit)->getID());

            if(approached.find(help->getID())==approached.end()) {
                approached[help->getID()] = std::vector<std::string>();
            }
			approached[(*rit)->getID()].push_back(help->getID());

		}
	}

	//debug
	std::cout << "approaching" << std::endl;
	std::map<std::string, std::vector<std::string> >::iterator it;
	for ( it = this->approaching.begin(); it != this->approaching.end(); it++ )
	{
		std::cout << it->first << std::endl;
		std::vector<std::string>::iterator ti;
		for ( ti = it->second.begin(); ti != it->second.end(); ti++ )
		{
			std::cout << "\t" << (*ti) << std::endl;
		}
	}
	std::cout << "approached" << std::endl;
	//std::map<std::string, std::vector<std::string> >::iterator it;
	for ( it = this->approached.begin(); it != this->approached.end(); it++ )
	{
		std::cout << it->first << std::endl;
		std::vector<std::string>::iterator ti;
		for ( ti = it->second.begin(); ti != it->second.end(); ti++ )
		{
			std::cout << "\t" << (*ti) << std::endl;
		}
	}

}
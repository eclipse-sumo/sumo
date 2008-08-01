/****************************************************************************/
/// @file    tracitestclient_main.h
/// @author  Friedemann Wesner <wesner@itm.uni-luebeck.de>
/// @date    2008/04/07
///
/// Main method for TraCITestClient
/****************************************************************************/

#include <iostream>
#include <string>
#include "TraCITestClient.h"

using namespace testclient;

//const int PORT = 8888;

int main (int argc, char* argv[]) {
	std::string defFile = "";
	std::string outFileName = "testclient_out.txt";
	int port = -1;
	std::string host = "localhost";
	TraCITestClient* client;

	if ((argc == 1) || (argc % 2 == 0)) {
		std::cout << "Usage: TraciTestClient -def <definition_file>  -p <remote port>"
				<< "[-h <remote host>] [-o <outputfile name>]" << std::endl;
		return 0;
	}

	for (int i=1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg.compare("-def") == 0) {
			defFile = argv[i+1];
			i++;
		} else if (arg.compare("-o") == 0) {
			outFileName = argv[i+1];
			i++;
		} else if (arg.compare("-p") == 0) {
			port = atoi(argv[i+1]);
			i++;
		} else if (arg.compare("-h") == 0) {
			host = argv[i+1];
			i++;
		} else {
			std::cout << "unknown parameter: " << argv[i] << std::endl;
			return 1;
		}
	}

	if (port == -1) {
		std::cout << "Missing port" << std::endl;
	}
	if (defFile.compare("") == 0) {
		std::cout << "Missing definition file" << std::endl;
	}

	client = new TraCITestClient(outFileName);
	client->run(defFile, port, host);
	delete client;

	return 0;
}
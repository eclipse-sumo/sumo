#ifndef ODMatrix_h
#define ODMatrix_h

# include <iostream>
# include <sstream>
# include <fstream>
# include <vector>
# include <cstdlib>
# include <ctime>
# include <string>

typedef int (*CMPFUN)(int, int); 

using namespace std;


/// OD attributes input
class OD_IN {
public:
  string from;
  string to;
  unsigned short int how_many; 
};


/// OD attributes output
class OD_OUT {
public:
  string from;
  string to;
  unsigned int time; 
};

#endif

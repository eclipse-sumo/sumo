#include <iostream>
#include "OutputDevice_File.h"

using namespace std;

OutputDevice_File::OutputDevice_File(std::ofstream *strm)
    : myFileStream(strm)
{
}


OutputDevice_File::~OutputDevice_File()
{
    delete myFileStream;
}


bool
OutputDevice_File::ok()
{
    return myFileStream->good();
}


void
OutputDevice_File::close()
{
    myFileStream->close();
}


std::ostream &
OutputDevice_File::getOStream()
{
    return *myFileStream;
}


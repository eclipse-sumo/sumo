#include <iostream>
#include "OutputDevice_COUT.h"

using namespace std;

OutputDevice_COUT::OutputDevice_COUT()
{
}


OutputDevice_COUT::~OutputDevice_COUT()
{
}


bool
OutputDevice_COUT::ok()
{
    return true;
}


void
OutputDevice_COUT::close()
{
    throw 1;
}

std::ostream &
OutputDevice_COUT::getOStream()
{
    return cout;
}


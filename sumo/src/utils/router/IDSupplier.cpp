#include <string>
#include <sstream>
#include "IDSupplier.h"

using namespace std;

IDSupplier::IDSupplier(const std::string &prefix, long begin)
    : _current(begin), _prefix(prefix)
{
}

IDSupplier::~IDSupplier()
{
}

std::string
IDSupplier::getNext()
{
    ostringstream strm;
    strm << _prefix << _current++;
    return strm.str();
}

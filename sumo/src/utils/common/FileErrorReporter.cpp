#include <string>
#include <utils/common/SErrorHandler.h>
#include "FileErrorReporter.h"

using namespace std;

FileErrorReporter::FileErrorReporter(const std::string &file)
    : _file(file)
{
}


FileErrorReporter::~FileErrorReporter()
{
}


void
FileErrorReporter::addError(const std::string &filetype,
                            const std::string &msg)
{
    SErrorHandler::add(
        string("The ") + filetype + string(" '") + _file
        + string("' is corrupt:"));
    SErrorHandler::add(msg);
}


void
FileErrorReporter::setFileName(const std::string &file)
{
    _file = file;
}


string
FileErrorReporter::getFileName() const
{
   return _file;
}

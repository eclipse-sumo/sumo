#ifndef FileErrorReporter_h
#define FileErrorReporter_h

#include <string>

class FileErrorReporter {
protected:
    std::string _file;
public:
    FileErrorReporter(const std::string &file="");
    virtual ~FileErrorReporter();
    void addError(const std::string &filetype, const std::string &msg);
    void setFileName(const std::string &file);
    /** returns the name of the parsed file */
    std::string getFileName() const;
};

#endif


#pragma once
#include <config.h>
#include <fstream> 
#include <memory>
#include <string>

#ifdef HAVE_PARQUET
#include <arrow/io/file.h>
#include <arrow/util/config.h>

#include <parquet/api/reader.h>
#include <parquet/exception.h>
#include <parquet/stream_writer.h>
#endif


class StreamDevice {
public:

    /// @brief The type of the stream
    enum Type {
        OSTREAM, // std::ostream (or std::ofstream)
        COUT, // std::cout
        PARQUET // parquet::StreamWriter
    };

    // create a constructor that a type and raw write access
    StreamDevice(Type type, bool access) : rawWriteAccess(access), myType(type)  {};
    // create a default constructor
    StreamDevice() = default;

    /// @brief Destructor
    virtual ~StreamDevice() = default;

    /// @brief is the stream ok
    /// @return true if the stream is ok
    virtual bool ok() = 0;

    /// @brief flush the stream
    /// @return this 
    virtual StreamDevice& flush() = 0;

    /// @brief close the stream
    virtual void close() = 0;

    /// @brief is the stream good
    /// @return true if the stream is good
    virtual bool good() = 0;

    /// @brief read the stream into a string
    /// @return the string 
    virtual std::string str() = 0;

    /// @brief  set the precision
    /// @param precision 
    virtual void setPrecision(int precision) = 0;

    /// @brief get the precision
    /// @return the precision
    virtual int precision() = 0;

    /// @brief implement a stream operator
    virtual operator std::ostream& () = 0;

    /// @brief write a string to the stream
    /// @param s the string to write
    virtual void str(const std::string& s) = 0;

    /// @brief write an endline to the stream
    /// @return this
    virtual StreamDevice& endLine() = 0;

    /// @brief set the output stream flags
    /// @param flags the flags to set
    virtual void setOSFlags(std::ios_base::fmtflags flags) = 0;

    /// @brief get the type of the stream
    virtual Type type() const {
        return myType;
    };

    /// @brief allow raw output
    bool allowRaw() const {
        return rawWriteAccess;
    }

    /// @brief define the behavior of a cast to std::ostream
    virtual std::ostream& getOStream() {
        throw std::runtime_error("Not implemented");
    }

protected:
    /// @brief allow raw write access
    bool rawWriteAccess = false;
    /// @brief the type of the stream
    Type myType = Type::OSTREAM;
        
};

class OStreamDevice : public StreamDevice {
public:

    // write a constructor that takes a std::ofstream
    OStreamDevice(std::ofstream* stream) : StreamDevice(Type::OSTREAM, true), myStream(std::move(stream)) {}
    OStreamDevice(std::ostream* stream) : StreamDevice(Type::OSTREAM, true), myStream(std::move(stream)) {}
    OStreamDevice(std::ofstream stream) : StreamDevice(Type::OSTREAM, true), myStream(new std::ofstream(std::move(stream))) {}
    OStreamDevice(std::basic_ostream<char> stream) : StreamDevice(Type::OSTREAM, true), myStream(&stream) {}

    virtual ~OStreamDevice() override  = default;

    bool ok() override {
        return myStream->good();
    }

    StreamDevice& flush() override {
        myStream->flush();
        return *this;
    }

    void close() override {
        myStream->flush();
    }

    template <typename T>
    StreamDevice& print(const T& t) {
        (*myStream) << t;
        return *this;
    }

    void setPrecision(int precision) override {
        (*myStream) << std::setprecision(precision);
    }

    void setOSFlags(std::ios_base::fmtflags flags) override {
        myStream->setf(flags);
    }

    int precision() override {
        return (int)myStream->precision();
    }

    bool good() override {
        return myStream->good();
    }

    std::string str() override {
        // Try casting to ostringstream
        if (auto* oss_ptr = dynamic_cast<std::ostringstream*>(myStream.get())) {
            return oss_ptr->str();
        }
        
        // Try casting to stringstream
        if (auto* ss_ptr = dynamic_cast<std::stringstream*>(myStream.get())) {
            return ss_ptr->str();
        }
        
        // If it's neither, we need to use a more general approach
        std::ostringstream oss;
        oss << myStream->rdbuf();
        return oss.str();
    }

    void str(const std::string& s) override {
        (*myStream) << s;
    }

    operator std::ostream& () override {
        return *myStream;
    }

    StreamDevice& endLine() override {
        (*myStream) << std::endl;
        return *this;
    }

    // get the type of the stream
    Type type() const override {
        return Type::OSTREAM;
    }

    std::ostream& getOStream() override {
        return *myStream;
    }

private:
    std::unique_ptr<std::ostream> myStream;
}; // Add the missing semicolon here


class COUTStreamDevice : public StreamDevice {
public:

    // write a constructor that takes a std::ofstream
    COUTStreamDevice() : StreamDevice(Type::COUT, true), myStream(std::cout) {};
    COUTStreamDevice(std::ostream& stream) : StreamDevice(Type::COUT, true), myStream(stream) {};

    virtual ~COUTStreamDevice() override = default;

    bool ok() override {
        return myStream.good();
    }

    StreamDevice& flush() override {
        myStream.flush();
        return *this;
    }

    void close() override {
        (void)(this->flush());
    }

    template <typename T>
    StreamDevice& print(const T& t) {
        myStream << t;
        return *this;
    }

    void setPrecision(int precision) override {
        myStream << std::setprecision(precision);
    }

    void setOSFlags(std::ios_base::fmtflags flags) override {
        myStream.setf(flags);
    }

    int precision() override {
        return static_cast<int>(myStream.precision());
    }

    bool good() override {
        return myStream.good();
    }

    std::string str() override {
        return "";
    }

    void str(const std::string& s) override {
        myStream << s;
    }

    operator std::ostream& () override {
        return myStream;
    }

    StreamDevice& endLine() override {
        myStream << std::endl;
        return *this;
    }

    std::ostream& getOStream() override {
        return myStream;
    }

private:

    std::ostream& myStream;

}; // Add the missing semicolon here

class ParquetStream : public StreamDevice {

#ifdef HAVE_PARQUET

public:

    ParquetStream(std::unique_ptr<parquet::ParquetFileWriter> file) : StreamDevice(Type::PARQUET, false) {
        myStream = std::make_unique<parquet::StreamWriter>(std::move(file));
    };

    virtual ~ParquetStream() = default;

    bool ok() override {
        return true;
    }

    bool good() override {
        // check that the stream is not null
        return myStream != nullptr;
    }

    StreamDevice& flush() override {
        // do nothing
        return *this;
    }

    void close() override {
        myStream->EndRowGroup();
        myStream.release();
    }

    void setPrecision(int precision) override {
        UNUSED_PARAMETER(precision);
    }

    std::string str() override {
        return "";
    }
    
    template <typename T>
    void print(const T& t) {
        (*myStream) << t;
    }

    // protect this to only allow types that are supported by parquet
    

    void setOSFlags(std::ios_base::fmtflags flags) override {UNUSED_PARAMETER(flags);}

    operator std::ostream& () override {
        throw std::runtime_error("Not implemented");
    }

    void str(const std::string& s) override {
        UNUSED_PARAMETER(s);
        throw std::runtime_error("Not implemented");
    };

    StreamDevice& endLine() override {
        myStream->EndRow();
        return *this;
    }

    // get the type of the stream
    Type type() const override {
        return Type::PARQUET;
    }

    int precision() override {
        return 0;
    }

private:
    std::unique_ptr<parquet::StreamWriter> myStream;

#endif
};

// implement a templated stream operator. The base class does nothing
template <typename T>
StreamDevice& operator<<(StreamDevice& stream, const T& t) {
    switch (stream.type()) {
    case StreamDevice::Type::OSTREAM:
        static_cast<OStreamDevice*>(&stream)->print(t);
        break;
    case StreamDevice::Type::COUT:
        static_cast<COUTStreamDevice*>(&stream)->print(t);
        break;
    case StreamDevice::Type::PARQUET:
#ifdef HAVE_PARQUET
        static_cast<ParquetStream*>(&stream)->print(t);
        // throw std::runtime_error("Parquet not supported in this build");
#else
        throw std::runtime_error("Parquet not supported in this build");
#endif
        break;
    default:
        // assert that this does not happen
        throw std::runtime_error("Unknown stream type in StreamDevice");
    }
    return stream;
}

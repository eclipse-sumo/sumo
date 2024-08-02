
#pragma once
#include <config.h>
#include <fstream> 

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

    enum Type {
        FILE,
        STRING,
        PARQUET
    };

    virtual ~StreamDevice() = default;

    // implement a stream interface
    virtual bool ok() = 0;

    virtual StreamDevice& flush() = 0;

    virtual void close() = 0;

    virtual bool good() = 0;

    virtual std::string str() = 0;

    // set precision
    virtual int precision() = 0;

    // implement a stream operator
    virtual operator std::ostream& () = 0;

    virtual void str(const std::string& s) {};

    virtual StreamDevice& endLine() = 0;

    // get the type of the stream
    virtual Type type() const = 0;

    virtual bool allowRaw() const {
        return false;
    }
};


class FileDevice : public StreamDevice {
public:

    // write a constructor that takes a std::ofstream
    FileDevice(std::ofstream* stream) : myStream(stream) {};
    FileDevice(std::ostream* stream) : myStream(stream) {};
    // constructor that takes a ofstream by value
    FileDevice(std::ofstream stream) : myStream(new std::ofstream(std::move(stream))) {};

    virtual ~FileDevice() = default;

    bool ok() override {
        return myStream->good();
    }

    StreamDevice& flush() override {
        myStream->flush();
        return *this;
    }

    void close() override {
        return;
    }

    template <typename T>
    StreamDevice& print(const T& t) {
        (*myStream) << t;
        return *this;
    }

    int precision() override {
        return (int)myStream->precision();
    }

    bool good() override {
        return myStream->good();
    }

    std::string str() override {
        return "";
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
        return Type::FILE;
    }

    bool allowRaw() const override {
        return true;
    }



private:
    const std::unique_ptr<std::ostream> myStream;
};


// create a class for std::ostringstream
class StringStream : public StreamDevice {
public:

    StringStream() = default;

    virtual ~StringStream() = default;

    bool ok() override {
        return myStream->good();
    }

    StreamDevice& flush() override {
        // do nothing
        myStream->flush();
        return *this;
    }

    void close() override {
        return;
    }

    template <typename T>
    StreamDevice& print(const T& t) {
        (*myStream) << t;
        return *this;
    }

    int precision() override {
        return (int)myStream->precision();
    }

    bool good() override {
        return true;
    }

    std::string str() override {
        return myStream->str();
    }

    operator std::ostream& () override {
        return *myStream;
    }

    void str(const std::string& s) override {
        myStream->str(s);
    };

    StreamDevice& endLine() override {
        (*myStream) << std::endl;
        return *this;
    }

    // get the type of the stream
    Type type() const override {
        return Type::STRING;
    }

    bool allowRaw() const override {
        return true;
    }

private:
    std::unique_ptr<std::ostringstream> myStream;
};


class ParquetStream : public StreamDevice {

#ifdef HAVE_PARQUET

public:

    ParquetStream(std::unique_ptr<parquet::ParquetFileWriter> file) : myStream(std::move(file)) {};

    virtual ~ParquetStream() = default;

    bool ok() override {
        return true;
    }

    bool good() override {
        return true;
    }

    StreamDevice& flush() override {
        // do nothing
        return *this;
    }

    void close() override {

    }

    // define the << operator for parquet::StreamWriter
    template <typename T>
    StreamDevice& log(const T& t) {
        myStream << t;
        return *this;
    }

    int precision() override {
        return 0;
    }

    std::string str() override {
        return "";
    }

    template <typename T>
    void print(const T& t) {
        myStream << t;

    }

    void print(const std::__1::__iom_t2& t) {
        // do nothing
        int i = 0;
    }

    void print(const std::__iom_t5& s) {};

    operator std::ostream& () override {
        throw std::runtime_error("Not implemented");
    }

    void str(const std::string& s) override {
        throw std::runtime_error("Not implemented");
    };

    StreamDevice& endLine() override {
        myStream.EndRow();
        return *this;
    }

    // get the type of the stream
    Type type() const override {
        return Type::PARQUET;
    }

    bool allowRaw() const override {
        return false;
    }

    bool closeStream() {
        myStream.EndRowGroup();
        return true;
    }

private:
    parquet::StreamWriter myStream;

#endif
};

// implement a templated stream operator. The base class does nothing
template <typename T>
StreamDevice& operator<<(StreamDevice& stream, const T& t) {
    switch (stream.type()) {
    case StreamDevice::Type::FILE:
        static_cast<FileDevice*>(&stream)->print(t);
        break;
    case StreamDevice::Type::STRING:
        static_cast<StringStream*>(&stream)->print(t);
        break;
    case StreamDevice::Type::PARQUET:
        static_cast<ParquetStream*>(&stream)->print(t);
        break;
    default:
        throw std::runtime_error("Unknown stream type");
    }
    return stream;
}


// StreamDevice& operator<<(StreamDevice& stream, const std::__1::__iom_t2& t) {
//     switch (stream.type()) {
//     case StreamDevice::Type::FILE:
//         static_cast<FileDevice&>(stream).print(t);
//         break;
//     case StreamDevice::Type::STRING:
//         static_cast<StringStream&>(stream).print(t);
//         break;
//     case StreamDevice::Type::PARQUET:
//         break;
//     default:
//         throw std::runtime_error("Unknown stream type");
//     }
//     return stream;
// }
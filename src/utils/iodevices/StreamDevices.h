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
    // Constructors for different writer types
    explicit StreamDevice(std::ostream& stream)
        : type_(OSTREAM), ostream_(&stream) {
    }

    explicit StreamDevice(std::unique_ptr<std::ostream> stream)
        : type_(OSTREAM), owned_stream_(std::move(stream)), ostream_(owned_stream_.get()) {
    }

#ifdef HAVE_PARQUET
    explicit StreamDevice(parquet::StreamWriter&& writer)
        : type_(PARQUET), parquet_writer_(std::move(writer)) {
    }
#endif

    ~StreamDevice() = default;

    // Core interface
    bool good() const {
        switch (type_) {
        case OSTREAM:
            return ostream_ && ostream_->good();
#ifdef HAVE_PARQUET
        case PARQUET:
            return parquet_writer_ != nullptr;
#endif
        }
        return false;
    }

    void flush() {
        switch (type_) {
        case OSTREAM:
            if (ostream_) ostream_->flush();
            break;
#ifdef HAVE_PARQUET
        case PARQUET:
            // Parquet doesn't need explicit flush
            break;
#endif
        }
    }

    void close() {
        switch (type_) {
        case OSTREAM:
            if (ostream_) {
                ostream_->flush();
                if (auto* file = dynamic_cast<std::ofstream*>(ostream_)) {
                    file->close();
                }
            }
            break;
#ifdef HAVE_PARQUET
        case PARQUET:
            if (parquet_writer_) {
                parquet_writer_->EndRowGroup();
            }
            break;
#endif
        }
    }

    // Template operator<< - handles all types
    template <typename T>
    StreamDevice& operator<<(const T& value) {
        switch (type_) {
        case OSTREAM:
            if (ostream_) *ostream_ << toString(value);
            break;
#ifdef HAVE_PARQUET
        case PARQUET:
            if (parquet_writer_) *parquet_writer_ << value;
            break;
#endif
        }
        return *this;
    }

    // Special handling for stream manipulators
    StreamDevice& operator<<(std::ostream& (*manip)(std::ostream&)) {
        switch (type_) {
        case OSTREAM:
            if (ostream_) *ostream_ << manip;
            break;
#ifdef HAVE_PARQUET
        case PARQUET:
            // Handle common manipulators for parquet
            if (manip == static_cast<std::ostream & (*)(std::ostream&)>(std::endl)) {
                if (parquet_writer_) parquet_writer_->EndRow();
            }
            break;
#endif
        }
        return *this;
    }

    int precision() const {
        switch (type_) {
        case OSTREAM:
            return ostream_ ? static_cast<int>(ostream_->precision()) : 6;
#ifdef HAVE_PARQUET
        case PARQUET:
            return 6; // Default precision for Parquet
#endif
        }
        return 6;
    }

    void precision(int prec) {
        switch (type_) {
        case OSTREAM:
            if (ostream_) ostream_->precision(prec);
            break;
#ifdef HAVE_PARQUET
        case PARQUET:
            // Parquet doesn't have precision concept, ignore
            break;
#endif
        }
    }

    std::ostream* getUnderlyingOStream() {
        switch (type_) {
        case OSTREAM:
            return ostream_;
#ifdef HAVE_PARQUET
        case PARQUET:
            return nullptr; // No underlying ostream for Parquet
#endif
        }
        return nullptr;
    }

private:
    // Conditional enum definition
#ifdef HAVE_PARQUET
    enum Type { OSTREAM, PARQUET };
#else
    enum Type { OSTREAM };
#endif

    Type type_;
    std::unique_ptr<std::ostream> owned_stream_;  // For owned streams
    std::ostream* ostream_ = nullptr;
#ifdef HAVE_PARQUET
    std::unique_ptr<parquet::StreamWriter> parquet_writer_;
#endif
};

class StreamFactory {
public:
    static std::unique_ptr<StreamDevice> createFile(const std::string& filename) {
        auto file = std::make_unique<std::ofstream>(filename);
        if (!file->is_open()) {
            return nullptr;
        }
        return std::make_unique<StreamDevice>(std::move(file));
    }

    static std::unique_ptr<StreamDevice> createConsole() {
        return std::make_unique<StreamDevice>(std::cout);
    }

    static std::unique_ptr<StreamDevice> createCERR() {
        return std::make_unique<StreamDevice>(std::cerr);
    }

    static std::unique_ptr<StreamDevice> createString() {
        auto ss = std::make_unique<std::stringstream>();
        return std::make_unique<StreamDevice>(std::move(ss));
    }

#ifdef HAVE_PARQUET
    static std::unique_ptr<StreamDevice> createParquet(parquet::StreamWriter& writer) {
        return std::make_unique<StreamDevice>(writer);
    }
#endif
};
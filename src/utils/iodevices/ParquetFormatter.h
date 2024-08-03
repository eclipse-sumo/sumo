/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2012-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    PlainXMLFormatter.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2012
///
// Output formatter for Parquet output
/****************************************************************************/
#pragma once
#include <config.h>

#ifdef HAVE_PARQUET
// parquet-cpp
#include <arrow/io/file.h>
#include <arrow/util/config.h>

#include <parquet/api/reader.h>
#include <parquet/exception.h>
#include <parquet/stream_writer.h>

#include "OutputFormatter.h"
#include <utils/common/ToString.h>
#include "StreamDevices.h"

#define PARQUET_TESTING


// Helper function to determine if a type is a fixed-length character array
template <typename T>
struct is_fixed_char_array : std::false_type {};

template <std::size_t N>
struct is_fixed_char_array<char[N]> : std::true_type {};

// Helper template for the static_assert
template <typename T>
constexpr bool always_false = false;

// Overloaded function for different types
template <typename T>
void AppendField(parquet::schema::NodeVector& fields, const T& val, const std::string& field_name) {

    if constexpr (std::is_same_v<T, std::string>) {
        fields.push_back(parquet::schema::PrimitiveNode::Make(
            field_name, parquet::Repetition::OPTIONAL, parquet::Type::BYTE_ARRAY,
            parquet::ConvertedType::UTF8));
    }
    else if constexpr (std::is_same_v<T, char>) {
        fields.push_back(parquet::schema::PrimitiveNode::Make(
            field_name, parquet::Repetition::REQUIRED, parquet::Type::FIXED_LEN_BYTE_ARRAY,
            parquet::ConvertedType::NONE, 1));
    }
    else if constexpr (is_fixed_char_array<T>::value) {
        fields.push_back(parquet::schema::PrimitiveNode::Make(
            field_name, parquet::Repetition::REQUIRED, parquet::Type::FIXED_LEN_BYTE_ARRAY,
            parquet::ConvertedType::NONE, sizeof(T)));
    }
    else if constexpr (std::is_same_v<T, int8_t>) {
        fields.push_back(parquet::schema::PrimitiveNode::Make(
            field_name, parquet::Repetition::REQUIRED, parquet::Type::INT32,
            parquet::ConvertedType::INT_8));
    }
    else if constexpr (std::is_same_v<T, uint16_t>) {
        fields.push_back(parquet::schema::PrimitiveNode::Make(
            field_name, parquet::Repetition::REQUIRED, parquet::Type::INT32,
            parquet::ConvertedType::UINT_16));
    }
    else if constexpr (std::is_same_v<T, int32_t>) {
        fields.push_back(parquet::schema::PrimitiveNode::Make(
            field_name, parquet::Repetition::REQUIRED, parquet::Type::INT32,
            parquet::ConvertedType::INT_32));
    }
    else if constexpr (std::is_same_v<T, uint64_t>) {
        fields.push_back(parquet::schema::PrimitiveNode::Make(
            field_name, parquet::Repetition::OPTIONAL, parquet::Type::INT64,
            parquet::ConvertedType::UINT_64));
    }
    else if constexpr (std::is_same_v<T, double>) {
        fields.push_back(parquet::schema::PrimitiveNode::Make(
            field_name, parquet::Repetition::REQUIRED, parquet::Type::DOUBLE,
            parquet::ConvertedType::NONE));
    }
    else if constexpr (std::is_same_v<T, std::chrono::microseconds>) {
        fields.push_back(parquet::schema::PrimitiveNode::Make(
            field_name, parquet::Repetition::REQUIRED, parquet::Type::INT64,
            parquet::ConvertedType::TIMESTAMP_MICROS));
    }
    else if constexpr (std::is_same_v<T, std::chrono::milliseconds>) {
        fields.push_back(parquet::schema::PrimitiveNode::Make(
            field_name, parquet::Repetition::REQUIRED, parquet::Type::INT64,
            parquet::ConvertedType::TIMESTAMP_MILLIS));
    }
    else {
        // warn
        fmt::print("Unsupported type for AppendField\n");
    }
    // else {
    //     static_assert(always_false<T>, "Unsupported type for AppendField");
    // }
}

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TypedAttribute
 * @brief A class to represent an attribute with a specific type
 *
 * This class is used to represent an attribute of an XML XMLElement with a specific type.
 */
 // Base class
class AttributeBase {
public:
    AttributeBase(std::string name) : name_(std::move(name)) {}
    virtual ~AttributeBase() = default;

    const std::string& getName() const { return name_; }

    // Pure virtual function for printing
    virtual void print(StreamDevice& os) const = 0;

private:
    std::string name_;
};

// Helper function to convert various types to Parquet-compatible types
template <typename T>
auto convertToParquetType(const T& value) {
    if constexpr (std::is_same_v<T, unsigned long>) {
        if constexpr (sizeof(unsigned long) <= sizeof(uint32_t)) {
            return static_cast<uint32_t>(value);
        } else {
            return static_cast<uint64_t>(value);
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        return value;
    } else if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_signed_v<T>) {
            if constexpr (sizeof(T) <= 1) return static_cast<int8_t>(value);
            else if constexpr (sizeof(T) <= 2) return static_cast<int16_t>(value);
            else if constexpr (sizeof(T) <= 4) return static_cast<int32_t>(value);
            else return static_cast<int64_t>(value);
        } else {
            if constexpr (sizeof(T) <= 1) return static_cast<uint8_t>(value);
            else if constexpr (sizeof(T) <= 2) return static_cast<uint16_t>(value);
            else if constexpr (sizeof(T) <= 4) return static_cast<uint32_t>(value);
            else return static_cast<uint64_t>(value);
        }
    } else if constexpr (std::is_floating_point_v<T>) {
        if constexpr (sizeof(T) <= 4) return static_cast<float>(value);
        else return static_cast<double>(value);
    } else if constexpr (std::is_same_v<T, std::chrono::milliseconds> || 
                         std::is_same_v<T, std::chrono::microseconds>) {
        return value;
    } else if constexpr (std::is_same_v<T, char>) {
        return value;
    } else if constexpr (std::is_array_v<T>) {
        // try the toString function
        return  toString(value);
    } else if constexpr (std::is_same_v<T, const char*> || 
                         std::is_same_v<T, std::string> || 
                         std::is_same_v<T, std::string_view>) {
        return std::string_view(value);
    } else {
        // For any other type, convert to string
        return toString(value);
    }
}

template <typename T, typename = void>
class Attribute : public AttributeBase {
public:
    Attribute(const std::string& name, const T& value)
        : AttributeBase(name), value_(convertToParquetType(value)) {}

    void print(StreamDevice& os) const override {
        os << value_;
    }

private:
    decltype(convertToParquetType(std::declval<T>())) value_;
};

// Specialization for numeric types
template <typename T>
class Attribute<T, std::enable_if_t<std::is_arithmetic_v<T>>> : public AttributeBase {
public:
    Attribute(const std::string& name, const T& value)
        : AttributeBase(name), value_(convertToParquetType(value)) {}

    void print(StreamDevice& os) const override {
        os << value_;
    }

private:
    decltype(convertToParquetType(std::declval<T>())) value_;
};


class XMLElement {
public:
    /// @brief Constructor
    explicit XMLElement(std::string name) : myName(std::move(name)), beenWritten(false) {}

    /// @brief Destructor
    virtual ~XMLElement() = default;

    /// @brief Move constructor
    XMLElement(XMLElement&& other) noexcept = default;


    /// @brief Move assignment operator
    XMLElement& operator=(XMLElement&& other) noexcept = default;

    /// @brief  Add an attribute to the XMLElement
    /// @param attr The attribute to add
    void addAttribute(std::unique_ptr<AttributeBase> attr) {
        myAttributes.push_back(std::move(attr));
    }

    // define a comparison operator (just checks the name)
    bool operator==(const XMLElement& other) const {
        return myName == other.myName;
    }

        // define a comparison operator (just checks the name)
    bool operator==(const std::string& other) const {
        return myName == other;
    }

    /// @brief a method to write the XMLElement to a stream using the << operator
    friend StreamDevice& operator<<(StreamDevice& into, const XMLElement& elem) {
        for (const auto& attr : elem.myAttributes) {
            attr->print(into);
        }
        return into;
    }

    /// @brief a method to check whether the XMLElement has been written
    bool written() const {
        return beenWritten;
    }

    /// @brief a method to set the XMLElement as written
    void setWritten() {
        beenWritten = true;
    }

    /// @brief get the attributes
    const std::vector<std::unique_ptr<AttributeBase>>& getAttributes() const {
        return myAttributes;
    }

    /// @brief get the name of the element
    const std::string& getName() const {
        return myName;
    }

protected:
    /// @brief The name of the XMLElement
    std::string myName;

    /// @brief stores whether the XMLElement has been written
    bool beenWritten;

    /// @brief a store for the attributes
    std::vector<std::unique_ptr<AttributeBase>> myAttributes;
};

/**
 * @class PlainXMLFormatter
 * @brief Output formatter for plain XML output
 *
 * PlainXMLFormatter format XML like output into the output stream.
 */
class ParquetFormatter : public OutputFormatter {
public:
    /// @brief Constructor
    ParquetFormatter() {};

    /// @brief Destructor
    virtual ~ParquetFormatter() { }

    /** @brief Writes an XML header with optional configuration
     *
     * If something has been written (myXMLStack is not empty), nothing
     *  is written and false returned.
     *
     * @param[in] into The output stream to use
     * @param[in] rootXMLElement The root XMLElement to use
     * @param[in] attrs Additional attributes to save within the rootXMLElement
     * @todo Describe what is saved
     */
     // turn off the warning for unused parameters
    [[maybe_unused]]
    bool writeXMLHeader(StreamDevice& into, const std::string& rootXMLElement,
        const std::map<SumoXMLAttr, std::string>& attrs,
        bool includeConfig = true) override {
        return 0;
    };


    /** @brief Opens an XML tag
     *
     * An indentation, depending on the current xml-XMLElement-stack size, is written followed
     *  by the given xml XMLElement ("<" + xmlXMLElement)
     * The xml XMLElement is added to the stack, then.
     *
     * @param[in] into The output stream to use
     * @param[in] xmlXMLElement Name of XMLElement to open
     * @return The OutputDevice for further processing
     */
    void openTag(StreamDevice& into, const std::string& xmlXMLElement) override {
#ifdef PARQUET_TESTING
        // assert that the stack does not contain the XMLElement
        assert(std::find(myXMLStack.begin(), myXMLStack.end(), xmlXMLElement) == myXMLStack.end());
#endif
        myXMLStack.push_back(XMLElement(xmlXMLElement));
    }

    /** @brief Opens an XML tag
     *
     * Helper method which finds the correct string before calling openTag.
     *
     * @param[in] into The output stream to use
     * @param[in] xmlXMLElement Id of the XMLElement to open
     */
    inline void openTag(StreamDevice& into, const SumoXMLTag& xmlXMLElement) override {
        openTag(into, toString(xmlXMLElement));
    };


    /** @brief Closes the most recently opened tag
     *
     * @param[in] into The output stream to use
     * @return Whether a further XMLElement existed in the stack and could be closed
     * @todo it is not verified that the topmost XMLElement was closed
     */
    inline bool closeTag(StreamDevice& into, const std::string& comment = "") override {
        if (myXMLStack.empty()) {
            return false;
        }
        
        // only check the last XMLElement
        if (!myXMLStack.back().written()) {
            for (auto& elem : myXMLStack) {
                into << elem;
                elem.setWritten();
            }
            // close the row
            into.endLine();
        }
        // pop the last XMLElement and remove from memory
        myXMLStack.pop_back();
        return false;
    };


    /** @brief writes a preformatted tag to the device but ensures that any
     * pending tags are closed
     * @param[in] into The output stream to use
     * @param[in] val The preformatted data
     */
    void writePreformattedTag(StreamDevice& into, const std::string& val) override {
        // don't take any action
        return;
    };

    /** @brief writes arbitrary padding
     */
    inline void writePadding(StreamDevice& into, const std::string& val) override {};


    /** @brief writes an arbitrary attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    template <class T>
    void writeAttr(StreamDevice& into, const std::string& attr, const T& val) {
        std::unique_ptr<AttributeBase> typed_attr = std::make_unique<Attribute<T>>(attr, val);
        this->myXMLStack.back().addAttribute(std::move(typed_attr));
        if (!sharedNodeVector && this->fields.find(attr) == this->fields.end()) {
            // add the field to the schema
            AppendField(myNodeVector, val, attr);
            this->fields.insert(attr);
        }
    }

    /** @brief returns the node vector
     * @return const parquet::schema::NodeVector&
    */
    inline const parquet::schema::NodeVector& getNodeVector() {
        sharedNodeVector = true;
        return myNodeVector;
    }

    bool wroteHeader() const override {
        return !myXMLStack.empty();
    }

    /**
     * @brief Get the Stack object
     *
     * @return std::vector<_Tag *>&
     */
    inline std::vector<XMLElement>& getStack() {
        return myXMLStack;
    }

    /**
     * @brief Write the header. (This has no effect for the ParquetFormatter)
     *
     * @param Return success
     */
    inline bool writeHeader(StreamDevice& into, const SumoXMLTag& rootElement) override { return true; };

    
    template <typename T>
    void writeRaw(StreamDevice& into, T& val) {
        throw std::runtime_error("writeRaw not implemented for ParquetFormatter");
    }


private:
    /// @brief The stack of begun xml XMLElements.
    /// We don't need to store the full XMLElement, just the value
    std::vector<XMLElement> myXMLStack;

    /// @brief The parquet node vector
    parquet::schema::NodeVector myNodeVector;

    /// @brief flag to determin if we have shared NodeVector
    bool sharedNodeVector{false};

    // @brief the set of unique fields
    std::set<std::string> fields;
};
// ===========================================================================
#endif // HAVE_PARQUET
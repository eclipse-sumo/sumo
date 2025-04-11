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

#include <optional>
#include <stdexcept>
#include <set>
#include <limits>
#include <map>
#include <iostream> // Include for std::cerr

#include "OutputFormatter.h"
#include <utils/common/ToString.h>
#include "StreamDevices.h"
#include <utils/common/MsgHandler.h>


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
    UNUSED_PARAMETER(val);
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
        // Default to string (UTF8) for any unhandled types to ensure compatibility
        fields.push_back(parquet::schema::PrimitiveNode::Make(
            field_name, parquet::Repetition::OPTIONAL, parquet::Type::BYTE_ARRAY,
            parquet::ConvertedType::UTF8));
    }
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
        // have to take a copy of the string, to ensure its lifetime is long enough
        return std::string(value);
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
        if (value_){
            os << *value_;
        } else{
            assert(false);
        }
    }

private:
    std::optional<decltype(convertToParquetType(std::declval<T>()))> value_;
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
    
    /// @brief clear the attributes vector
    void clearAttributes() {
        myAttributes.clear();
    }

    /// @brief set the attributes vector (takes ownership)
    void setAttributes(std::vector<std::unique_ptr<AttributeBase>>&& attrs) {
        myAttributes = std::move(attrs);
    }

    /// @brief swap attributes with another vector
    void swapAttributes(std::vector<std::unique_ptr<AttributeBase>>& out) {
        out.swap(myAttributes);
    }

protected:
    /// @brief The name of the XMLElement
    std::string myName;

    /// @brief stores whether the XMLElement has been written
    bool beenWritten;

    /// @brief a store for the attributes
    std::vector<std::unique_ptr<AttributeBase>> myAttributes;
};

// Helper function to safely convert various types to double
// Needed for interval begin/end times which might be passed as different types
template <typename T>
double convertToDouble(const T& value) {
    if constexpr (std::is_arithmetic_v<T>) {
        return static_cast<double>(value);
    } else if constexpr (std::is_convertible_v<T, std::string_view>) {
        try {
            // Use stod for robust string-to-double conversion
            return std::stod(std::string(value));
        } catch (const std::invalid_argument& e) {
            return std::numeric_limits<double>::quiet_NaN(); // Return NaN on failure
        } catch (const std::out_of_range& e) {
            return std::numeric_limits<double>::quiet_NaN(); // Return NaN on failure
        }
    } else {
        // Fallback: try converting to string first, then to double
        try {
            return std::stod(toString(value));
        } catch (...) {
            return std::numeric_limits<double>::quiet_NaN(); // Return NaN on failure
        }
    }
}

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
    virtual ~ParquetFormatter() = default;

    /// @brief Lock the schema to prevent clearing
    void lockSchema() {
        schemaLocked = true;
    }

    /// @brief Unlock the schema to allow clearing
    void unlockSchema() {
        schemaLocked = false;
    }
    
    /// @brief Enable actual data writing
    void enableDataWrite() {
        dataWriteEnabled = true;
    }
    
    /// @brief Disable actual data writing (use for schema setup only)
    void disableDataWrite() {
        dataWriteEnabled = false;
    }

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
    bool writeXMLHeader(StreamDevice& into, const std::string& rootXMLElement,
        const std::map<SumoXMLAttr, std::string>& attrs,
        bool includeConfig = true) override {
        UNUSED_PARAMETER(into);
        UNUSED_PARAMETER(rootXMLElement);
        UNUSED_PARAMETER(attrs);
        UNUSED_PARAMETER(includeConfig);
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
        UNUSED_PARAMETER(into);
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
        UNUSED_PARAMETER(comment);
        if (myXMLStack.empty()) {
            return false;
        }

        // Get the element being closed
        XMLElement& currentElement = myXMLStack.back();
        bool isIntervalTag = currentElement.getName() == toString(SUMO_TAG_INTERVAL);
        bool isTimestepTag = currentElement.getName() == toString(SUMO_TAG_TIMESTEP);

        // If data writing is disabled, we're just setting up the schema
        // so don't write anything to the output
        if (!dataWriteEnabled) {
            myXMLStack.pop_back();
            // Clear interval attributes when interval tag is popped during schema setup
            if (isIntervalTag) {
                currentIntervalBegin.reset();
                currentIntervalEnd.reset();
                currentIntervalId.reset();
            } else if (isTimestepTag) {
                currentTimeValue.reset();
            }
            // Return true if the stack is still not empty (indicating more tags to close)
            return !myXMLStack.empty();
        }

        // Only write attributes if the element hasn't been processed yet and it's not an interval/timestep tag
        // (interval/timestep tags don't represent rows in Parquet)
        if (!currentElement.written() && !isIntervalTag && !isTimestepTag && into.type() == StreamDevice::Type::PARQUET) {
            // Create a temporary map to hold attributes by name for easy lookup
            std::map<std::string, std::unique_ptr<AttributeBase>> attributeMap;
            
            // First add the interval attributes if they exist
            if (currentIntervalId.has_value()) {
                attributeMap["id"] = std::make_unique<Attribute<std::string>>("id", toString(*currentIntervalId));
            }
            if (currentIntervalBegin.has_value()) {
                attributeMap["begin"] = std::make_unique<Attribute<std::string>>("begin", toString(*currentIntervalBegin));
            }
            if (currentIntervalEnd.has_value()) {
                attributeMap["end"] = std::make_unique<Attribute<std::string>>("end", toString(*currentIntervalEnd));
            }
            // Add the timestep time value if it exists
            if (currentTimeValue.has_value()) {
                attributeMap["time"] = std::make_unique<Attribute<std::string>>("time", toString(*currentTimeValue));
            }
            
            // Move the original element attributes into the map, potentially overwriting interval ones if names clash
            std::vector<std::unique_ptr<AttributeBase>> originalAttrs;
            currentElement.swapAttributes(originalAttrs);
            for (auto& attr : originalAttrs) {
                 attributeMap[attr->getName()] = std::move(attr); // Move into map
            }
            
            // Create the final attributes vector, ordered according to the schema (myNodeVector)
            std::vector<std::unique_ptr<AttributeBase>> finalAttributes;
            for (const auto& node : myNodeVector) {
                const std::string& fieldName = node->name();
                auto it = attributeMap.find(fieldName);
                if (it != attributeMap.end()) {
                    // Attribute exists, move it to the final vector
                    finalAttributes.push_back(std::move(it->second));
                    attributeMap.erase(it); // Remove from map
                } else {
                    // Attribute is missing, add a default/empty one
                    finalAttributes.push_back(std::make_unique<Attribute<std::string>>(fieldName, ""));
                }
            }
            
            // Add any remaining attributes from the map (shouldn't happen if schema is complete)
            for (auto& pair : attributeMap) {
                 finalAttributes.push_back(std::move(pair.second));
                 std::cerr << "Warning: Attribute '" + pair.first + "' was present in data but not found in Parquet schema.\n";
            }

            // Set the ordered attributes
            currentElement.setAttributes(std::move(finalAttributes));
            
            // Now write all attributes to the stream
            into << currentElement;
            currentElement.setWritten(); // Mark this element's attributes as processed for this row

            // Close the row in the Parquet stream
            into.endLine();
        } else if (!currentElement.written() && !isIntervalTag && !isTimestepTag) {
            // Non-Parquet output or during schema setup
            into << currentElement;
            currentElement.setWritten();
            into.endLine();
        }

        // Pop the element from the stack
        myXMLStack.pop_back();
        
        // Clear interval attributes when interval tag is closed
        if (isIntervalTag) {
            currentIntervalBegin.reset();
            currentIntervalEnd.reset();
            currentIntervalId.reset();
        } else if (isTimestepTag) {
            currentTimeValue.reset();
        }

        // Return true if the stack is still not empty (indicating more parent tags exist)
        return !myXMLStack.empty();
    };


    /** @brief writes a preformatted tag to the device but ensures that any
     * pending tags are closed
     * @param[in] into The output stream to use
     * @param[in] val The preformatted data
     */
    void writePreformattedTag(StreamDevice& into, const std::string& val) override {
        // don't take any action
        UNUSED_PARAMETER(into);
        UNUSED_PARAMETER(val);
        return;
    };

    /** @brief writes arbitrary padding
     */
    inline void writePadding(StreamDevice& into, const std::string& val) override {
        UNUSED_PARAMETER(into);
        UNUSED_PARAMETER(val);
    };


    /** @brief writes an arbitrary attribute
     *
     * @param[in] into The output stream to use
     * @param[in] attr The attribute (name)
     * @param[in] val The attribute value
     */
    template <class T>
    void writeAttr(StreamDevice& into, const std::string& attr, const T& val) {
        UNUSED_PARAMETER(into);
        
        // Check if we are inside the 'interval' tag and capture specific attributes
        if (!myXMLStack.empty() && myXMLStack.back().getName() == toString(SUMO_TAG_INTERVAL)) {
            if (attr == "begin") {
                // Ensure value is converted to double for time
                try {
                    currentIntervalBegin = convertToDouble(val);
                } catch (const std::invalid_argument& e) {
                    // Handle or log error if conversion fails
                    currentIntervalBegin.reset();
                }
                // Continue to add to interval element's attribute list
            } else if (attr == "end") {
                try {
                    currentIntervalEnd = convertToDouble(val);
                } catch (const std::invalid_argument& e) {
                    currentIntervalEnd.reset();
                }
                // Continue to add to interval element's attribute list
            } else if (attr == "id") {
                // ID should be string
                currentIntervalId = toString(val);
                // Continue to add to interval element's attribute list
            }
        } 
        // Check if we are inside the 'timestep' tag and capture time attribute
        else if (!myXMLStack.empty() && myXMLStack.back().getName() == toString(SUMO_TAG_TIMESTEP)) {
            if (attr == "time") {
                // Store the time value as string
                currentTimeValue = toString(val);
            }
        }
        
        // Always convert values to string format for consistency across types
        // This avoids type mismatches when writing different types to the same column
        auto stringVal = toString(val);
        std::unique_ptr<AttributeBase> typed_attr = std::make_unique<Attribute<std::string>>(attr, stringVal);
        
        if (!sharedNodeVector && this->fields.find(attr) == this->fields.end()) {
            // Add all fields as string type for maximum compatibility
            parquet::schema::NodePtr stringNode = parquet::schema::PrimitiveNode::Make(
                attr, parquet::Repetition::OPTIONAL, parquet::Type::BYTE_ARRAY,
                parquet::ConvertedType::UTF8);
            myNodeVector.push_back(stringNode);
            this->fields.insert(attr);
            // Store field type for reference
            this->fieldTypes[attr] = parquet::Type::BYTE_ARRAY;
        }
        
        if (!myXMLStack.empty()) {
            this->myXMLStack.back().addAttribute(std::move(typed_attr));
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
    inline bool writeHeader([[maybe_unused]] StreamDevice& into, [[maybe_unused]] const SumoXMLTag& rootElement) override { return true; };

    
    template <typename T>
    void writeRaw(StreamDevice& into, T& val) {
        UNUSED_PARAMETER(into);
        UNUSED_PARAMETER(val);
        throw std::runtime_error("writeRaw not implemented for ParquetFormatter");
    }

    int getDepth() const {
        return static_cast<int>(myXMLStack.size());
    }

    void clearStack() {
        myXMLStack.clear();
        if (!schemaLocked) {
            myNodeVector.clear();
            fields.clear();
            fieldTypes.clear();
        }
        // Clear interval attributes
        currentIntervalBegin.reset();
        currentIntervalEnd.reset();
        currentIntervalId.reset();
        currentTimeValue.reset();
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

    /// @brief flag to determin if we have locked the schema
    bool schemaLocked{false};

    /// @brief flag to enable/disable data writing
    bool dataWriteEnabled{true};
    
    /// @brief map to track field types in the schema
    std::map<std::string, parquet::Type::type> fieldTypes;
    
    // Store interval attributes to add to each row
    std::optional<double> currentIntervalBegin;
    std::optional<double> currentIntervalEnd;
    std::optional<std::string> currentIntervalId;
    std::optional<std::string> currentTimeValue;
};
// ===========================================================================
#endif // HAVE_PARQUET
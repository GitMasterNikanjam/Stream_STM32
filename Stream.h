#pragma once

// ####################################################################################################
// Include libraries:

#include <cstring>  // Provides functions for manipulating C-style strings (e.g., strcpy(), strlen(), strcat()).
#include <cstdio>   // Includes functions for input/output operations, such as formatted string printing (printf(), sprintf()).
#include <cstdint>  // Defines fixed-width integer types (int32_t, uint64_t, etc.) and limits for platform-independent integer handling.
#include <string>   // Provides the std::string class for working with dynamic strings in C++

// ###################################################################################################
// Define global macros:


// ###################################################################################################
// Data type enumaration and value union :

enum dataTypeEnum
{
    noneType,
    uint8Type,
    uint16Type,
    uint32Type,
    uint64Type,
    int8Type,
    int16Type,
    int32Type,
    int64Type,
    floatType,
    doubleType,
    charType,
    stringType,
    boolType
};

/**
 * @brief Union to store different data types efficiently
 *  */ 
union dataValueUnion 
{
    uint8_t uint8Value;
    uint16_t uint16Value;
    uint32_t uint32Value;
    uint64_t uint64Value;
    int8_t int8Value;
    int16_t int16Value;
    int32_t int32Value;
    int64_t int64Value;
    float floatValue;
    double doubleValue;
    char stringValue[8];
    bool boolValue;
};

// ####################################################################################################
// Public General functions

/**
 * @namespace Stream_utility
 * @brief set of functions for working with C-style strings, performing operations such as trimming, splitting, validating formats, and type conversions.
 */
namespace Stream_utility
{

/**
 * @brief Function to trim leading and trailing spaces from a string.
 * @param data is the Input string that wanted to trim
 * @return Trimed string.
 */
void trimString(char* data);

/**
 * @brief Function to split a string by a delimiter in two section and return a splited strings.
 * @param firstSection is the first part of splited string.
 * @param secondSection is the second part of splited string.
 * @return true if splited succeeded.
 */
bool splitString(const char* data, char delimiter, char* firstSection = nullptr, char* secondSection = nullptr);

/**
 * @brief Function to check if a string is empty or contains only spaces.
 * @return true if succeeded.
 */
bool isWhitespaceOnly(const char* data);

/**
 * @brief Function to validate a string has the expected sections that splited by ',' character.
 * @note if any sections be empty space it returns false.
 * @return true if succeeded.
 */
bool validateRow(const char* data, size_t expectedColumnCount);

/**
 * @brief Check string format for boolean (true/false).
 * @param str can be: {true, false, TRUE, FALSE}
 * @return true if succeeded.
 */
bool isBoolean(const char* str);

/**
 * @brief Check string format for double.
 * @return true if succeeded.
 */
bool isDouble(const char* str);

/**
 * @brief Check string format for float.
 * @return true if succeeded.
 */
bool isFloat(const char* str);

/**
 * @brief Check string format for int64.
 * @return true if succeeded.
 */
bool isInt64(const char* str);

/**
 * @brief Check string format for int32.
 * @return true if succeeded.
 */
bool isInt32(const char* str);

/**
 * @brief Check string format for int16.
 * @return true if succeeded.
 */
bool isInt16(const char* str);

/**
 * @brief Check string format for int8.
 * @return true if succeeded.
 */
bool isInt8(const char* str);

/**
 * @brief Check string format for uint64.
 * @return true if succeeded.
 */
bool isUInt64(const char* str);

/**
 * @brief Check string format for uint32.
 * @return true if succeeded.
 */
bool isUInt32(const char* str);

/**
 * @brief Check string format for uint16.
 * @return true if succeeded.
 */
bool isUInt16(const char* str);

/**
 * @brief Check string format for uint8.
 * @return true if succeeded.
 */
bool isUInt8(const char* str);

/**
 * @brief Convert double value to string with certain precision.
 * @param value: double value.
 * @param precision: number of characters after the decimal point.
 * @return string representation of the double value.
 */
void decimalToString(double value, uint8_t precision, char* output);

/**
 * @brief Convert float value to string with certain precision.
 * @param value: float value.
 * @param precision: number of characters after the decimal point.
 * @return string representation of the float value.
 */
void decimalToString(float value, uint8_t precision, char* output);

/**
 * @brief Function to check if all characters in a string are valid numbers.
 * @note - Intiger and decimal numbers are acceptable.
 * 
 * @note - '-' or '+' character at the first of string are acceptable.
 * @return true if succeeded.
 */
bool isNumber(const char* str);

/**
 * @brief Function to check a string are valid intiger number.
 * @note - '-' or '+' character at the first of string are acceptable.
 * @return true if succeeded.
 */
bool isIntiger(const char* str);

/**
 * @brief Function to check if a string ends with a given suffix.
 * @return true if succeeded.
 */
bool endsWith(const char* str, const char* suffix);

/**
 * @brief Convert string to intiger number.
 * @param num is the number for  store intiger value.
 * @return true if str is intiger.
 */
bool stringToInt32(const char* str, int32_t* num);

/**
 * @brief Convert string to float number.
 * @param num is the number for store float value.
 * @return true if str is float.
 */
bool stringToFloat(const char* str, float* num);

/**
 * @brief Convert string to double number.
 * @param num is the number for store double value.
 * @return true if str is double.
 */
bool stringToDouble(const char* str, double* num);

/**
 * @brief Check data value in string format for certain data type in string format.
 * @param data: is the value of a data in string format.
 * @param type can be: {uint8, uint16, uint32, uint64, int8, int16, int32, int64, float, double, string, bool}
 * @return true if data type is correct.
 *  */ 
bool checkValueType(const char *data, const char *type);

/**
 * @brief Check data value in string format for certain data type in data type enumaration.
 * @param data: is the value of a data in string format.
 * @param type: is the data type enumaration. eg: dataType_uint8, dataType_float, ... .
 * @return true if data type is correct.
 *  */ 
bool checkValueType(const char *data, const dataTypeEnum type);

/// @brief Helper function to convert dataValueUnion to string based on ParamType_t
std::string dataValueToString(const dataValueUnion& value, const dataTypeEnum type);

}

// ######################################################################################################
// Public Classes:

/**
 * @class Stream
 * @brief This class can be used as an object for data receive and transmit flow management.
 */
class Stream
{
public:

    /// @brief Last error code number occurred for the object.
    int8_t errorCode;

    /**
     * @brief Constructor. Init some variables and parameters. Init TX/RX buffers.
     * @param txBuffer: Transmit buffer pointer.
     * @param txBufferSize: Transmit buffer size.
     * @param rxBuffer: Recieve buffer pointer.
     * @param rxBufferSize: Recieve buffer size.
     */
    Stream(char* txBuffer = nullptr, size_t txBufferSize = 0, char* rxBuffer = nullptr, size_t rxBufferSize = 0);

    /**
     * Destructor.
     */
    ~Stream();

    /**
     * @brief Set transmit buffer.
     * @param txBuffer: Transmit buffer pointer.
     * @param txBufferSize: Transmit buffer size.
     */
    void setTxBuffer(char* txBuffer, size_t txBufferSize);

    /**
     * @brief Set receive buffer.
     * @param rxBuffer: Recieve buffer pointer.
     * @param rxBufferSize: Recieve buffer size.
     */
    void setRxBuffer(char* rxBuffer, size_t rxBufferSize);
    
    /**
     * @brief Return current TxBuffer size.
     */
    size_t getTxBufferSize();

    /**
     * @brief Return currect RxBuffer size.
     */
    size_t getRxBufferSize();

    /**
     * @brief Return TxBuffer pointer.
     */
    const char* getTxBuffer();
    
    /**
     * @brief Return RxBuffer pointer.
     */
    const char* getRxBuffer();

    /**
     * @brief Clear all data on the TxBuffer.
     */
    void clearTxBuffer();

    /**
     * @brief Clear all data on the RxBuffer.
     */
    void clearRxBuffer();

    /**
     * @brief Remove front certain number elements from TX buffer and remove them.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Not enough data in the buffer to remove"
     *  */
    bool removeFrontTxBuffer(size_t dataSize = 1);

    /**
     * @brief Remove front certain number elements from RX buffer and remove them.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Not enough data in the buffer to remove"
     *  */
    bool removeFrontRxBuffer(size_t dataSize = 1);

    /**
     * @brief Write data Tx buffer.
     * @note - Clear any previouse data on Tx buffer.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: Data size exceeds TX buffer size."
     */
    bool writeTxBuffer(const char* data, size_t dataSize);

    /**
     * @brief Write data Rx buffer.
     * @note - Clear any previouse data on Rx buffer.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: Data size exceeds RX buffer size."
     */
    bool writeRxBuffer(const char* data, size_t dataSize);

    /**
     * @brief Push back certain number charecter in to TxBuffer.
     * @param data is a char array pointer that you want to push back.
     * @param dataSize is the data length for push back.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Error Stream: TX Buffer Overflow"
     */
    bool pushBackTxBuffer(const char* data, size_t dataSize = 1);

    /**
     * @brief Push back certain number charecter in to TxBuffer.
     * @param data is a string pointer that you want to push back.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Error Stream: TX Buffer Overflow"
     */
    bool pushBackTxBuffer(const std::string* data);

    /**
     * @brief Push back certain number charecter in to RxBuffer.
     * @param data is a char array pointer that you want to push back.
     * @param dataSize is the data length for push back.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Error Stream: RX Buffer Overflow"
     */
    bool pushBackRxBuffer(const char* data, size_t dataSize = 1);

    /**
     * @brief Push back certain number charecter in to RxBuffer.
     * @param data is a string pointer that you want to push back.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Error Stream: RX Buffer Overflow"
     */
    bool pushBackRxBuffer(const std::string* data);

    /**
     * @brief Pop front certain number elements from TX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Not enough data in the buffer to pop"
     *  */
    bool popFrontTxBuffer(std::string* data, size_t dataSize = 1);

    /**
     * @brief Pop front certain number elements from TX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Not enough data in the buffer to pop"
     *  */
    bool popFrontTxBuffer(char* data, size_t dataSize = 1);

    /**
     * @brief Pop front certain number elements from RX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Not enough data in the buffer to pop"
     *  */
    bool popFrontRxBuffer(std::string* data, size_t dataSize = 1);

    /**
     * @brief Pop front certain number elements from RX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Not enough data in the buffer to pop"
     *  */
    bool popFrontRxBuffer(char* data, size_t dataSize = 1);

    /**
     * @brief Pop front all elements from TX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     *  */
    bool popAllTxBuffer(std::string* data);

    /**
     * @brief Pop front all elements from TX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     *  */
    bool popAllTxBuffer(char* data);

    /**
     * @brief Pop front all elements from RX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     *  */
    bool popAllRxBuffer(std::string* data);

    /**
     * @brief Pop front all elements from RX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     *  */
    bool popAllRxBuffer(char* data);

    /**
     * @brief Return data length on TxBuffer.
     * @return true if succeeded.
     */
    size_t availableTx();

    /**
     * @brief Return data length on RxBuffer.
     * @return true if succeeded.
     */
    size_t availableRx();

private:

    /// @brief Transmit buffer pointer.
    char* _txBuffer;

    /// @brief Receive buffer pointer.
    char* _rxBuffer;

    /// @brief Transmit buffer size.
    size_t _txBufferSize;

    /// @brief Receive buffer size.
    size_t _rxBufferSize;

    /// @brief The last character position in the _txBuffer.
    size_t _txPosition;

    /// @brief The last character position in the _rxBuffer.
    size_t _rxPosition;
};



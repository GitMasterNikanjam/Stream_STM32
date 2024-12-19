#pragma once

// ####################################################################################################
// Include libraries:

#include <cstring>  // Provides functions for manipulating C-style strings (e.g., strcpy(), strlen(), strcat()).
#include <cstdio>   // Includes functions for input/output operations, such as formatted string printing (printf(), sprintf()).
#include <cstdint>  // Defines fixed-width integer types (int32_t, uint64_t, etc.) and limits for platform-independent integer handling.
#include <string>   // Provides the std::string class for working with dynamic strings in C++

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
char* trimString(const char* data);

/**
 * @brief Function to trim leading and trailing spaces from a string.
 * @param data is the Input string that wanted to trim
 * @return Trimed string.
 */
std::string trimString(std::string data);

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
     * Constructor. Init some variables and parameters.
     * @note Default TxSize and RxSize is 256 byte.
     */
    Stream();

    /**
     * Destructor. delete buffers.
     */
    ~Stream();

    /**
     * @brief Change transmit buffer size. Delete Tx buffer and Reallocate it.
     * @param txSize: Size of the transmit buffer.
     */
    void setTxBufferSize(size_t txSize);

    /**
     * @brief Change receive buffer size. Delete Rx buffer and Reallocate it.
     * @param rxSize: Size of the transmit buffer.
     */
    void setRxBufferSize(size_t rxSize);

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
     * @brief Write data Tx buffer.
     * @note Clear any previouse data on Tx buffer.
     * @return true if succeeded.
     */
    bool writeTxBuffer(const char* data, size_t dataSize);

    /**
     * @brief Write data Rx buffer.
     * @note Clear any previouse data on Rx buffer.
     * @return true if succeeded.
     */
    bool writeRxBuffer(const char* data, size_t dataSize);

    /**
     * @brief Push back certain number charecter in to TxBuffer.
     * @param data is a char array pointer that you want to push back.
     * @param dataSize is the data length for push back.
     * @return true if succeeded.
     */
    bool pushBackTxBuffer(const char* data, size_t dataSize = 1);

    /**
     * @brief Push back certain number charecter in to TxBuffer.
     * @param data is a string pointer that you want to push back.
     * @return true if succeeded.
     */
    bool pushBackTxBuffer(const std::string* data);

    /**
     * @brief Push back certain number charecter in to RxBuffer.
     * @param data is a char array pointer that you want to push back.
     * @param dataSize is the data length for push back.
     * @return true if succeeded.
     */
    bool pushBackRxBuffer(const char* data, size_t dataSize = 1);

    /**
     * @brief Push back certain number charecter in to RxBuffer.
     * @param data is a string pointer that you want to push back.
     * @return true if succeeded.
     */
    bool pushBackRxBuffer(const std::string* data);

    /**
     * @brief Pop front certain number elements from TX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     *  */
    bool popFrontTxBuffer(std::string* data, size_t dataSize = 1);

    /**
     * @brief Pop front certain number elements from TX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     *  */
    bool popFrontTxBuffer(char* data, size_t dataSize = 1);

    bool removeFrontTxBuffer(size_t dataSize = 1);

    /**
     * @brief Pop front certain number elements from RX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     *  */
    bool popFrontRxBuffer(std::string* data, size_t dataSize = 1);

    /**
     * @brief Pop front certain number elements from RX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     *  */
    bool popFrontRxBuffer(char* data, size_t dataSize = 1);

    /**
     * @brief Pop front all elements from TX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     *  */
    bool popAllTxBuffer(std::string* data);

    /**
     * @brief Pop front all elements from TX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     *  */
    bool popAllTxBuffer(char* data);

    /**
     * @brief Pop front all elements from RX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     *  */
    bool popAllRxBuffer(std::string* data);

    /**
     * @brief Pop front all elements from RX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
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

    /// @brief Transmit buffer.
    char* _txBuffer;

    /// @brief Transmit buffer size.
    size_t _txBufferSize;

    /// @brief Receive buffer.
    char* _rxBuffer;

    /// @brief Receive buffer size.
    size_t _rxBufferSize;

    /// @brief The last character position in the _txBuffer.
    size_t _txPosition;

    /// @brief The last character position in the _rxBuffer.
    size_t _rxPosition;
};



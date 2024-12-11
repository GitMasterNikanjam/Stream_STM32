#pragma once

#include <string>
#include <sstream>                  // String stream for input/output operations on strings
#include <limits>                   // Numeric limits for data types
#include <cctype>                   // Character classification and conversion functions
#include <iomanip>                  // Manipulators for formatted I/O, like std::setprecision
#include <cstring> // For memcpy
#include <cstddef> // For size_t

// ###########################################################################################

namespace Stream_utils
{

/**
 * @ingroup public_general_functions
 * @brief Function to trim leading and trailing spaces from a string.
 * @return Trimed string.
 *  */ 
string trimString(const string &str);

/**
 * @ingroup public_general_functions
 * @brief Function to split a string by a delimiter and return a vector of trimmed strings.
 * @return Splited vector string.
 *  */ 
string splitString(const string &line, char delimiter);

/**
 * @ingroup public_general_functions
 * @brief Function to check if a string is empty or contains only spaces
 * @return true if succeeded.
 *  */ 
bool isWhitespaceOnly(const string& line);

/**
 * @ingroup public_general_functions
 * @brief Function to validate a vector string has the expected number of elements. 
 * @return true if data size is expectedColumnCount and there is no empty data.
 *  */ 
bool validateRow(const string &data, uint16_t expectedColumnCount);

/**
 * @ingroup public_general_functions
 * @brief Check data string for certain type.
 * @param type can be: {uint8, uint16, uint32, uint64, int8, int16, int32, int64, float, double, string, bool}
 * @return true if succeeded.
 *  */ 
bool checkValuetype(const string data, const string type);

/**
 * @ingroup public_general_functions
 * @brief Check string format for bool.
 * @param str can be: {true, false, TRUE, FALSE}
 * @return true if succeeded. 
 *  */ 
bool isBoolean(const string& str);

/**
 * @ingroup public_general_functions
 * @brief Check string format for double.
 * @return true if succeeded. 
 *  */ 
bool isDouble(const string& str);

/**
 * @ingroup public_general_functions
 * @brief Check string format for float.
 * @return true if succeeded. 
 *  */ 
bool isFloat(const string& str);

/**
 * @ingroup public_general_functions
 * @brief Check string format for int64.
 * @return true if succeeded. 
 *  */ 
bool isInt64(const string& str);

/**
 * @ingroup public_general_functions
 * @brief Check string format for int32.
 * @return true if succeeded. 
 *  */ 
bool isInt32(const string& str);

/**
 * @ingroup public_general_functions
 * @brief Check string format for int16.
 * @return true if succeeded. 
 *  */ 
bool isInt16(const string& str);

/**
 * @ingroup public_general_functions
 * @brief Check string format for int8.
 * @return true if succeeded. 
 *  */ 
bool isInt8(const string& str);

/**
 * @ingroup public_general_functions
 * @brief Check string format for uint64.
 * @return true if succeeded. 
 *  */ 
bool isUInt64(const string& str);

/**
 * @ingroup public_general_functions
 * @brief Check string format for uint32.
 * @return true if succeeded. 
 *  */ 
bool isUInt32(const string& str);

/**
 * @ingroup public_general_functions
 * @brief Check string format for uint16.
 * @return true if succeeded. 
 *  */ 
bool isUInt16(const string& str);

/**
 * @ingroup public_general_functions
 * @brief Check string format for uint8.
 * @return true if succeeded. 
 *  */
bool isUInt8(const string& str);

/** 
 * @ingroup public_general_functions
 * @brief Convert double value to string with certain precision.
 * @param value: double value.
 * @param precision: number of character after dot character. 
 * @return string of double value.
*/
string decimalToString(double value, uint8_t precision);

/**
 * @ingroup public_general_functions
 * @brief Convert float value to string with certain precision.
 * @param value: float value.
 * @param precision: number of character after dot character. 
 * @return string of float value.
 */ 
string decimalToString(float value, uint8_t precision);

/**
 * @ingroup public_general_functions
 * @brief Function to check if all cherecters on a string is a valid number. 
 * @note For decimal numbers with "." character returned false.
 * @return true if succeeded.
 *  */ 
bool isNumber(const string &str);

/**
 * @ingroup public_general_functions
 * @brief Function to check if a string ends with a given suffix
 * @return true if succeeded.
 *  */ 
bool endsWith(const string& str, const string suffix);

}

// ###########################################################################################

class Stream
{
    public:

    Stream(uint8_t *txBuffer, uint16_t txSize, uint8_t *rxBuffer, uint16_t rxSize);
    ~Stream();

    uint16_t write(const uint8_t *data, uint16_t size);
    uint16_t read(uint8_t *buffer, uint16_t size);

    void flush();
    bool available() const;


    private:

        uint8_t *_txBuffer;

        uint16_t _txBufferSize;

        uint8_t *_rxBuffer;

        uint16_t _rxBufferSize;

        uint16_t _txPosition;
        uint16_t _rxPosition;
};




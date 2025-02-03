// ####################################################################################################
// Include libraries:

#include "Stream.h"

// #####################################################################################################
// Public General functions

// Manual strnlen() function for Keil
size_t Stream_utility::safe_strnlen(const char* str, size_t max_len) 
{
    size_t len = 0;
    while (len < max_len && str[len] != '\0') 
    {
        ++len;
    }
    return len;
}

void Stream_utility::trimString(char* data, uint32_t max_size) 
{
    if (data == nullptr || data[0] == '\0')
    {
        return;  // If the input is null or empty, nothing to trim
    }

    uint32_t len;

    if(max_size != 0)
    {
        // Ensure null termination
        data[max_size - 1] = '\0';
        len = safe_strnlen(data, max_size);
    }
    else
    {
        len = std::strlen(data);
    }

    uint32_t start = 0;
    
    int32_t end = len - 1;  // Find the end of the string. Use signed index to avoid underflow

    // Trim leading whitespace
    while (start <= end && std::isspace(static_cast<unsigned char>(data[start]))) 
    {
        ++start;
    }

    // If there's nothing left after trimming leading spaces, empty the string
    if (start > end) {
        data[0] = '\0';  // Empty string
        return;
    }

    // Trim trailing whitespace
    while (end >= start && std::isspace(static_cast<unsigned char>(data[end]))) 
    {
        --end;
    }

    // Shift characters and null-terminate. Move trimmed string to the start manually to avoid `memmove`
    std::strcpy(data, data + start);  
    data[end - start + 1] = '\0';  // Null-terminate the string
}

void Stream_utility::trimString(const char* data, char* buffer, uint32_t max_size)
{
    if (data == nullptr || buffer == nullptr || data[0] == '\0')
    {
        return;  // If the input is null or empty, nothing to trim
    }

    uint32_t len;

    if(max_size != 0)
    {
        // Ensure null termination within the max_size limit
        buffer[max_size - 1] = '\0';
        len = safe_strnlen(data, max_size);
    }
    else
    {
        len = std::strlen(data);
    }

    uint32_t start = 0;
    
    int32_t end = len - 1;  // Find the end of the string. Use signed index to avoid underflow

    // Trim leading whitespace
    while (start <= end && std::isspace(static_cast<unsigned char>(data[start]))) 
    {
        ++start;
    }

    // If there's nothing left after trimming leading spaces, empty the string
    if (start > end) {
        buffer[0] = '\0';  // Empty string
        return;
    }

    // Trim trailing whitespace
    while (end >= start && std::isspace(static_cast<unsigned char>(data[end]))) 
    {
        --end;
    }

    // Check if the buffer is large enough for the result
    uint32_t trimmed_len = end - start + 1;
    if (max_size > 0 && trimmed_len >= max_size)
    {
        trimmed_len = max_size - 1;  // Ensure we don't overflow the buffer
    }

    // Copy the trimmed string into the buffer and null-terminate
    std::memcpy(buffer, data + start, trimmed_len);
    buffer[end - start + 1] = '\0';  // Null-terminate the string
}

bool Stream_utility::splitString(const char* data, char delimiter, char* firstSection, char* secondSection) 
{
    if (data == nullptr) 
    {
        return false;
    }

    // Find the delimiter in the string
    const char* delimiterPos = std::strchr(data, delimiter);
    if (delimiterPos == nullptr) 
    {
        // Delimiter not found
        return false;
    }

    // Copy the first section (everything before the delimiter)
    size_t firstLength = delimiterPos - data;
    if(firstSection != nullptr)
    {
        std::strncpy(firstSection, data, firstLength);
        firstSection[firstLength] = '\0';  // Null-terminate the string
    }
    
    // Copy the second section (everything after the delimiter)
    const char* secondStart = delimiterPos + 1;
    if(secondSection != nullptr)
    {
        std::strcpy(secondSection, secondStart);
    }
    
    return true;
}

bool Stream_utility::isWhitespaceOnly(const char* str) 
{
     while (*str) 
     {
        if (*str != ' ' && *str != '\t' && *str != '\n' && *str != '\v' && *str != '\f' && *str != '\r') 
        {
            return false; // If not a whitespace character
        }
        str++;
    }
    return true;
}

bool Stream_utility::validateRow(const char* data, size_t expectedColumnCount) 
{
    char* fields = new char[strlen(data) + 1];
    char* firstSection;

    strcpy(fields, data);

    size_t fieldCount = 0;
    
    while(Stream_utility::splitString(fields, ',', firstSection, fields) == true)
    {
        if(isWhitespaceOnly(firstSection) || isWhitespaceOnly(fields))
        {
            return false;
        }
        fieldCount++;
    }

    if (fieldCount != (expectedColumnCount -1)) 
    {
        return false; // Incorrect number of columns
    }

    return true;
}

bool Stream_utility::isUInt8(const char* str) 
{
    if(isIntiger(str) == false)
    {
        return false;
    }

    unsigned int value = 0;
    if (std::sscanf(str, "%u", &value) == 1 && value <= std::numeric_limits<uint8_t>::max()) 
    {
        return true;
    }
    return false;
}

bool Stream_utility::isUInt16(const char* str) 
{
    if(isIntiger(str) == false)
    {
        return false;
    }

    unsigned int value = 0;
    if (std::sscanf(str, "%u", &value) == 1 && value <= std::numeric_limits<uint16_t>::max()) 
    {
        return true;
    }
    return false;
}

bool Stream_utility::isUInt32(const char* str) 
{
    if (str == nullptr || *str == '\0')  // Ensure non-null and non-empty string
    {
        return false;
    }

    if(isUintiger(str) == false)
    {
        return false;
    }

    char* endPtr;
    unsigned long value = std::strtoul(str, &endPtr, 10);

    // Ensure the whole string is processed and within uint32_t range
    return (*endPtr == '\0' && value <= std::numeric_limits<uint32_t>::max());
}

bool Stream_utility::isUInt64(const char* str) 
{
    if(isIntiger(str) == false)
    {
        return false;
    }

    unsigned long long value = 0;
    if (std::sscanf(str, "%llu", &value) == 1 && value <= std::numeric_limits<uint64_t>::max()) 
    {
        return true;
    }
    return false;
}

bool Stream_utility::isInt8(const char* str) 
{
    if(isIntiger(str) == false)
    {
        return false;
    }

    int value = 0;
    if (std::sscanf(str, "%d", &value) == 1 && value >= std::numeric_limits<int8_t>::min() && value <= std::numeric_limits<int8_t>::max()) 
    {
        return true;
    }
    return false;
}

bool Stream_utility::isInt16(const char* str) 
{
    if(isIntiger(str) == false)
    {
        return false;
    }

    int value = 0;
    if (std::sscanf(str, "%d", &value) == 1 && value >= std::numeric_limits<int16_t>::min() && value <= std::numeric_limits<int16_t>::max()) 
    {
        return true;
    }
    return false;
}

bool Stream_utility::isInt32(const char* str) 
{
    if(isIntiger(str) == false)
    {
        return false;
    }

    long value = 0;
    if (std::sscanf(str, "%ld", &value) == 1 && value >= std::numeric_limits<int32_t>::min() && value <= std::numeric_limits<int32_t>::max()) 
    {
        return true;
    }
    return false;
}

bool Stream_utility::isInt64(const char* str) 
{
    if(isIntiger(str) == false)
    {
        return false;
    }

    long long value = 0;
    if (std::sscanf(str, "%lld", &value) == 1 && value >= std::numeric_limits<int64_t>::min() && value <= std::numeric_limits<int64_t>::max()) 
    {
        return true;
    }
    return false;
}

bool Stream_utility::isFloat(const char* str) 
{
    if(isNumber(str) == false)
    {
        return false;
    }
    
    float value = 0.0f;
    return std::sscanf(str, "%f", &value) == 1;
}

bool Stream_utility::isDouble(const char* str) 
{
    if(isNumber(str) == false)
    {
        return false;
    }

    double value = 0.0;
    return std::sscanf(str, "%lf", &value) == 1;
}

bool Stream_utility::isBoolean(const char* str) 
{
    if (str == nullptr) return false;

    // Compare the string directly with both upper and lowercase versions of "true" and "false"
    return (std::strcmp(str, "true") == 0 || std::strcmp(str, "TRUE") == 0 || std::strcmp(str, "false") == 0 || std::strcmp(str, "FALSE") == 0);
}

void Stream_utility::decimalToString(double value, uint8_t precision, char* output) 
{
    std::sprintf(output, "%.*f", precision, value);
}

void Stream_utility::decimalToString(float value, uint8_t precision, char* output) 
{
    std::sprintf(output, "%.*f", precision, value);
}

bool Stream_utility::isNumber(const char* str) 
{
    if (str == nullptr || *str == '\0') return false; // Null or empty string

    if(*str)
    {
        if ( (*str == '-') || (*str == '+') )
        {
            ++str;
        }
    }

    uint8_t dotNumber = 0;

    while (*str) 
    {
        if(*str == '.')
        {
            dotNumber++;
            if(dotNumber > 1)
            {
                return false;
            }
        }
        else if (*str < '0' || *str > '9')  // Check if the character is not a digit (i.e., not '0' to '9')
        {
            return false; // Return false if it's not a valid digit
        }
        ++str;
    }
    return true; // Return true if all characters are digits
}

bool Stream_utility::isIntiger(const char* str)
{
    if (str == nullptr || *str == '\0') return false; // Null or empty string

    if(*str)
    {
        if ( (*str == '-') || (*str == '+') )
        {
            ++str;
        }
    }

    while (*str) 
    {
        // Check if the character is not a digit (i.e., not '0' to '9')
        if (*str < '0' || *str > '9') 
        {
            return false; // Return false if it's not a valid digit
        }
        ++str;
    }
    return true; // Return true if all characters are digits
}

bool Stream_utility::isUintiger(const char* str)
{
    if (str == nullptr || *str == '\0' || *str == '-') return false; // Null or empty string or minus number

    if(*str)
    {
        if ( (*str == '+') )
        {
            ++str;
        }
    }

    while (*str) 
    {
        // Check if the character is not a digit (i.e., not '0' to '9')
        if (*str < '0' || *str > '9') 
        {
            return false; // Return false if it's not a valid digit
        }
        ++str;
    }
    return true; // Return true if all characters are digits
}

bool Stream_utility::endsWith(const char* str, const char* suffix) 
{
    size_t strLen = std::strlen(str);
    size_t suffixLen = std::strlen(suffix);
    if (strLen >= suffixLen) 
    {
        return std::strcmp(str + strLen - suffixLen, suffix) == 0;
    }
    return false;
}

bool Stream_utility::stringToUint8(const char* str, uint8_t* num)
{
    if(isUInt8(str) == false)
    {
        return false;
    }

    // Use sscanf to convert the string to an integer
    unsigned int temp;
    if (std::sscanf(str, "%u", &temp) != 1) 
    {
        return false; // sscanf failed to parse a valid integer
    }

    // Check if the value fits within the uint8_t range (0-255)
    if (temp > 255) 
    {
        return false; // The value is out of range for uint8_t
    }

    *num = (uint8_t)(temp); // Successfully convert to uint8_t

    return true;   
}

bool Stream_utility::stringToUint16(const char* str, uint16_t* num)
{
    if(isUInt16(str) == false)
    {
        return false;
    }

    // Use sscanf to convert the string to an integer
    unsigned int temp;
    if (std::sscanf(str, "%u", &temp) != 1) 
    {
        return false; // sscanf failed to parse a valid integer
    }

    // Check if the value fits within the uint16_t range
    if (temp > 65535) {
        return false; // The value is out of range for uint8_t
    }

    *num = (uint16_t)(temp); // Successfully convert to uint8_t
    
    return true;   
}

bool Stream_utility::stringToUint32(const char* str, uint32_t* num)
{
    if(isUInt32(str) == false)
    {
        return false;
    }

    // Use sscanf to convert the string to an integer
    unsigned int temp;
    if (std::sscanf(str, "%u", &temp) != 1) 
    {
        return false; // sscanf failed to parse a valid integer
    }

    // Check if the value fits within the uint16_t range
    if (temp > 4294967295) {
        return false; // The value is out of range for uint8_t
    }

    *num = (uint32_t)(temp); // Successfully convert to uint8_t
    
    return true;   
}

bool Stream_utility::stringToUint64(const char* str, uint64_t* num)
{
    if(isUInt64(str) == false)
    {
        return false;
    }

    // Use sscanf to convert the string to an integer
    if (std::sscanf(str, "%llu", num) != 1) 
    {
        return false; // sscanf failed to parse a valid integer
    }
    
    return true;   
}

bool Stream_utility::stringToInt8(const char* str, int8_t* num)
{
    if(isInt8(str) == false)
    {
        return false;
    }

    // Use sscanf to convert the string to an integer
    int temp;
    if (std::sscanf(str, "%d", &temp) != 1) 
    {
        return false; // sscanf failed to parse a valid integer
    }

    // Check if the value fits within the int8_t range (-128 to 127)
    if (temp < -128 || temp > 127) {
        return false; // The value is out of range for int8_t
    }

    *num = (int8_t)(temp); // Successfully convert to int8_t
    
    return true;
}

bool Stream_utility::stringToInt16(const char* str, int16_t* num)
{
    if(isInt16(str) == false)
    {
        return false;
    }

    // Use sscanf to convert the string to an integer
    int temp;
    if (std::sscanf(str, "%d", &temp) != 1) 
    {
        return false; // sscanf failed to parse a valid integer
    }

    // Check if the value fits within the int16_t range (-32768 to 32767)
    if (temp < -32768 || temp > 32767) {
        return false; // The value is out of range for int16_t
    }

    *num = (int16_t)(temp); // Successfully convert to int16_t
    
    return true;
}

bool Stream_utility::stringToInt32(const char* str, int32_t* num)
{
    if(isInt32(str) == false)
    {
        return false;
    }

    // Use sscanf to convert the string to an integer
    if (std::sscanf(str, "%d", num) != 1) 
    {
        return false; // sscanf failed to parse a valid integer
    }

    return true;
}

bool Stream_utility::stringToInt64(const char* str, int64_t* num)
{
    if(isInt64(str) == false)
    {
        return false;
    }

    // Use sscanf to convert the string to an integer
    if (std::sscanf(str, "%lld", num) != 1) 
    {
        return false; // sscanf failed to parse a valid integer
    }

    return true;
}

bool Stream_utility::stringToFloat(const char* str, float* num)
{
    if(isFloat(str) == false)
    {
        return false;
    }

    // Use sscanf to convert the string to an integer
    float temp;
    if (std::sscanf(str, "%f", &temp) != 1) 
    {
        return false; // sscanf failed to parse a valid integer
    }

    *num = temp;
    return true;
}

bool Stream_utility::stringToDouble(const char* str, double* num)
{
    if(isDouble(str) == false)
    {
        return false;
    }

    // Use sscanf to convert the string to an integer
    if (std::sscanf(str, "%lf", num) != 1) 
    {
        return false; // sscanf failed to parse a valid integer
    }

    return true;
}

bool Stream_utility::stringToNumber(const char* str, dataValueUnion* num, dataTypeEnum type)
{
    // Convert value based on the parameter type
    switch (type)
    {
        case uint8Type:
            if(Stream_utility::stringToUint8(str, &num->uint8Value) == false) 
            {
                return false;
            }
        break;
        case uint16Type:
            if(Stream_utility::stringToUint16(str, &num->uint16Value) == false) 
            {
                return false;
            }
        break;
        case uint32Type:
            if(Stream_utility::stringToUint32(str, &num->uint32Value) == false) 
            {
                return false;
            }
        break;
        case uint64Type:
            if(Stream_utility::stringToUint64(str, &num->uint64Value) == false) 
            {
                return false;
            }
        break;
        case int8Type:
            if(Stream_utility::stringToInt8(str, &num->int8Value) == false) 
            {
                return false;
            }
        break;
        case int16Type:
            if(Stream_utility::stringToInt16(str, &num->int16Value) == false) 
            {
                return false;
            }
        break;
        case int32Type:
            if(Stream_utility::stringToInt32(str, &num->int32Value) == false) 
            {
                return false;
            }
        break;
        case int64Type:
            if(Stream_utility::stringToInt64(str, &num->int64Value) == false) 
            {
                return false;
            }
        break;
        case floatType:
            if(Stream_utility::stringToFloat(str, &num->floatValue) == false) 
            {
                return false;
            }
        break;
        case doubleType:
            if(Stream_utility::stringToDouble(str, &num->doubleValue) == false)  
            {
                return false;
            }
        break;
        case boolType:
            // Compare using std::strcmp instead of == for string content comparison
            num->boolValue = (std::string(str) == "true" || std::string(str) == "1");
        break;
        case stringType:
            std::strncpy(num->stringValue, str, sizeof(&num->stringValue) - 1);
            num->stringValue[sizeof(num->stringValue) - 1] = '\0';  // Ensure null termination
        break;
        default:
            return false;
    }

    return true;
}

bool Stream_utility::checkValueType(const char *data, const char *type)
{
    if(strcmp(type, "uint8") == 0)
    {
        return isUInt8(data);
    }
    else if(strcmp(type, "uint16") == 0)
    {
        return isUInt16(data);
    }
    else if(strcmp(type, "uint32") == 0)
    {
        return isUInt32(data);
    }
        else if(strcmp(type, "uint64") == 0)
    {
        return isUInt64(data);
    }
        else if(strcmp(type, "int8") == 0)
    {
        return isInt8(data);
    }
        else if(strcmp(type, "int16") == 0)
    {
        return isInt16(data);
    }
        else if(strcmp(type, "int32") == 0)
    {
        return isInt32(data);
    }
        else if(strcmp(type, "int64") == 0)
    {
        return isInt64(data);
    }
        else if(strcmp(type, "float") == 0)
    {
        return isFloat(data);
    }
        else if(strcmp(type, "double") == 0)
    {
        return isDouble(data);
    }
        else if(strcmp(type, "string") == 0)
    {
        return true;
    }
        else if(strcmp(type, "bool") == 0)
    {
        return isBoolean(data);
    }
    else
    {
        return false;
    }

    return true;
}

bool Stream_utility::checkValueType(const char *data, dataTypeEnum type)
{
    switch(type)
    {
        case uint8Type:
            return isUInt8(data);
        break;
        case uint16Type:
            return isUInt16(data);
        break;
        case uint32Type:
            return isUInt32(data);
        break;
        case uint64Type:
            return isUInt64(data);
        break;
        case int8Type:
            return isInt8(data);
        break;
        case int16Type:
            return isInt16(data);
        break;
        case int32Type:
            return isInt32(data);
        break;
        case int64Type:
            return isInt64(data);
        break;
        case floatType:
            return isFloat(data);
        break;
        case doubleType:
            return isDouble(data);
        break;
        case charType:
            return true;
        break;
        case stringType:
            return true;
        break;
        case boolType:
            return isBoolean(data);
        break;
        default:
            return false;
    }

    return true;
}

std::string Stream_utility::dataValueToString(const dataValueUnion& value, const dataTypeEnum type)
{
    switch (type)
    {
        case uint8Type:
            return std::to_string(value.uint8Value);
        case uint16Type:
            return std::to_string(value.uint16Value);
        case uint32Type:
            return std::to_string(value.uint32Value);
        case int8Type:
            return std::to_string(value.int8Value);
        case int16Type:
            return std::to_string(value.int16Value);
        case int32Type:
            return std::to_string(value.int32Value);
        case floatType:
            return std::to_string(value.floatValue);
        case doubleType:
            return std::to_string(value.doubleValue);
        case boolType:
            return value.boolValue ? "true" : "false";
        case stringType:
            return std::string(value.stringValue);
        default:
            return "Unsupported Type";
    }
}

// ###########################################################################################################
// Stream class:

Stream::Stream(char* txBuffer, size_t txBufferSize, char* rxBuffer, size_t rxBufferSize)
{
    _txBufferSize = txBufferSize;
    _rxBufferSize = rxBufferSize;

    _txBuffer = txBuffer;
    _rxBuffer = rxBuffer;

    _txPosition = 0;
    _rxPosition = 0;

    // Null-terminate the remaining buffer (optional for string usage)
    memset(_txBuffer, '\0', _txBufferSize);
    memset(_rxBuffer, '\0', _rxBufferSize);

    errorCode = 0;
}

Stream::~Stream()
{

}

void Stream::setTxBuffer(char* txBuffer, size_t txBufferSize)
{
    _txBufferSize = txBufferSize;
    _txBuffer = txBuffer;
    _txPosition = 0;
}

void Stream::setRxBuffer(char* rxBuffer, size_t rxBufferSize)
{
    _rxBufferSize = rxBufferSize;
    _rxBuffer = rxBuffer;
    _rxPosition = 0;
}

const char* Stream::getTxBuffer() 
{
    return _txBuffer;
}

size_t Stream::getTxBufferSize() 
{
    return _txBufferSize;
}

const char* Stream::getRxBuffer() 
{
    return _rxBuffer;
}

size_t Stream::getRxBufferSize() 
{
    return _rxBufferSize;
}

void Stream::clearTxBuffer() 
{
    memset(_txBuffer, 0, _txBufferSize); // Zero out TX buffers
    _txPosition = 0;
}

void Stream::clearRxBuffer() 
{
    memset(_rxBuffer, 0, _rxBufferSize); // Zero out RX buffer
    _rxPosition = 0;
}

bool Stream::writeTxBuffer(const char* data, size_t dataSize) 
{
    if (dataSize > _txBufferSize) 
    {
        errorCode = 1;  // "Error Stream: Data size exceeds TX buffer size.";
        return false;
    }
    memcpy(_txBuffer, data, dataSize); // Copy data to TX buffer
    _txPosition = dataSize;

    return true;
}

bool Stream::writeRxBuffer(const char* data, size_t dataSize) 
{
    if (dataSize > _rxBufferSize) 
    {
        errorCode = 1;  // "Error Stream: Data size exceeds RX buffer size.";
        return false;
    }
    memcpy(_rxBuffer, data, dataSize); // Copy data to RX buffer
    _rxPosition = dataSize;

    return true;
}

bool Stream::pushBackTxBuffer(const char* data, size_t dataSize)
{
    if(data == NULL)
    {
        errorCode = 1;  // "Error Stream: data can not be null.";
        return false;
    }

    if(_txPosition + dataSize <= _txBufferSize)
    {
        for (size_t i = 0; i < dataSize; ++i) 
        {
            _txBuffer[_txPosition] = data[i];
            ++_txPosition; // Move position forward after writing
        }
    }
    else
    {
        errorCode = 2;   // "Error Stream: TX Buffer Overflow";
        return false;  
    }

    return true;
}

bool Stream::pushBackRxBuffer(const char* data, size_t dataSize)
{
    if(data == NULL)
    {
        errorCode = 1;  // "Error Stream: data can not be null.";
        return false;
    }

    // Check for buffer overflow
    if (_rxPosition + dataSize > _rxBufferSize) 
    {
        errorCode = 2; // "RX Buffer Overflow."
        return false;
    }

    // Copy the data into the buffer
    std::memcpy(&_rxBuffer[_rxPosition], data, dataSize);
    _rxPosition += dataSize; // Update the position

    return true;
}

bool Stream::pushBackTxBuffer(const std::string* data)
{
    return pushBackTxBuffer(data->c_str(), data->size());
}

bool Stream::pushBackRxBuffer(const std::string* data)
{
    return pushBackRxBuffer(data->c_str(), data->size());
}

bool Stream::popFrontTxBuffer(char* data, size_t dataSize)
{
    if(data == nullptr)
    {
        errorCode = 1;
        return false;
    }

    if (dataSize > _txPosition) 
    {
        errorCode = 2; // Not enough data in the buffer to pop
        return false;
    }

    // Copy the data to the output buffer
    std::memcpy(data, _txBuffer, dataSize);

    // Shift the remaining data in the TX buffer
    std::memmove(_txBuffer, _txBuffer + dataSize, _txPosition - dataSize);

    // Update the TX buffer position
    _txPosition -= dataSize;

    // Null-terminate the remaining buffer (optional for string usage)
    _txBuffer[_txPosition] = '\0';

    return true;
}

bool Stream::removeFrontTxBuffer(size_t dataSize)
{
    if (dataSize > _txPosition) 
    {
        errorCode = 1; // Not enough data in the buffer to pop
        return false;
    }

    // Shift the remaining data in the TX buffer
    std::memmove(_txBuffer, _txBuffer + dataSize, _txPosition - dataSize);

    // Update the TX buffer position
    _txPosition -= dataSize;

    // Null-terminate the remaining buffer (optional for string usage)
    _txBuffer[_txPosition] = '\0';

    return true;
}

bool Stream::removeFrontRxBuffer(size_t dataSize)
{
    if (dataSize > _rxPosition) 
    {
        errorCode = 1; // Not enough data in the buffer to pop
        return false;
    }

    // Shift the remaining data in the TX buffer
    std::memmove(_rxBuffer, _rxBuffer + dataSize, _rxPosition - dataSize);

    // Update the TX buffer position
    _rxPosition -= dataSize;

    // Null-terminate the remaining buffer (optional for string usage)
    _rxBuffer[_rxPosition] = '\0';

    return true;
}

bool Stream::popFrontTxBuffer(std::string* data, size_t dataSize)
{
    if(data == nullptr)
    {
        errorCode = 1;
        return false;
    }

    if (dataSize > _txPosition) 
    {
        errorCode = 2; // Not enough data in the buffer to pop
        return false;
    }

    // Assign the requested portion of data to the std::string
    *data = std::string(_txBuffer, dataSize);

    // Shift the remaining data in the TX buffer
    std::memmove(_txBuffer, _txBuffer + dataSize, _txPosition - dataSize);

    // Update the TX buffer position
    _txPosition -= dataSize;

    // Null-terminate the remaining buffer (optional for safety if treated as a string)
    _txBuffer[_txPosition] = '\0';

    return true;
}

bool Stream::popAllTxBuffer(char* data)
{
    return popFrontTxBuffer(data, _txPosition);
}

bool Stream::popAllTxBuffer(std::string* data)
{
    return popFrontTxBuffer(data, _txPosition);
}

bool Stream::popFrontRxBuffer(char* data, size_t dataSize)
{
    if(data == nullptr)
    {
        errorCode = 1;
        return false;
    }

    if (dataSize == 0) 
    {
        errorCode = 3; // "Error: Data size cannot be zero."
        return false;
    }

    if (dataSize > _rxPosition) 
    {
        errorCode = 2; // Not enough data in the buffer to pop
        return false;
    }

    // Copy the data to the output buffer
    std::memcpy(data, _rxBuffer, dataSize);

    // Shift the remaining data in the TX buffer
    std::memmove(_rxBuffer, _rxBuffer + dataSize, _rxPosition - dataSize);

    // Update the TX buffer position
    _rxPosition -= dataSize;

    // Null-terminate the remaining buffer (optional for string usage)
    _rxBuffer[_rxPosition] = '\0';

    return true;
}

bool Stream::popFrontRxBuffer(std::string* data, size_t dataSize)
{
    if(data == nullptr)
    {
        errorCode = 1;
        return false;
    }

    if (dataSize > _rxPosition) 
    {
        errorCode = 2;// Not enough data in the buffer to pop
        return false;
    }

    // Assign the requested portion of data to the std::string
    *data = std::string(_rxBuffer, dataSize);

    // Shift the remaining data in the TX buffer
    std::memmove(_rxBuffer, _rxBuffer + dataSize, _rxPosition - dataSize);

    // Update the TX buffer position
    _rxPosition -= dataSize;

    // Null-terminate the remaining buffer (optional for safety if treated as a string)
    _rxBuffer[_rxPosition] = '\0';

    return true;
}

bool Stream::popAllRxBuffer(char* data)
{
    return popFrontRxBuffer(data, _rxPosition);
}

bool Stream::popAllRxBuffer(std::string* data)
{
    return popFrontRxBuffer(data, _rxPosition);
}

size_t Stream::availableTx() 
{
    // return strlen(_txBuffer);
    return _txPosition;
}

size_t Stream::availableRx() 
{
    // return strlen(_rxBuffer);
    return _rxPosition;
}



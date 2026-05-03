// ####################################################################################################
// Include libraries:

#include "Stream.h"
#include <cctype>
#include <limits>
#include <cstdlib>

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

    uint32_t len = 0;

    if(max_size != 0)
    {
        // If max_size==0 means "unknown", keep this branch guarded.
        // Ensure there is a terminator inside the buffer window.
        data[max_size - 1] = '\0';

        // bounded strlen
        len = static_cast<uint32_t>(safe_strnlen(data, max_size));
    }
    else
    {
        len = static_cast<uint32_t>(std::strlen(data));
    }

    uint32_t start = 0;
    
    int32_t end = static_cast<int32_t>(len) - 1;  // Find the end of the string. Use signed index to avoid underflow

    // Trim leading whitespace
    while (start <= (uint32_t)end && std::isspace(static_cast<unsigned char>(data[start]))) 
    {
        ++start;
    }

    // If there's nothing left after trimming leading spaces, empty the string
    if (start > (uint32_t)end) {
        data[0] = '\0';  // Empty string
        return;
    }

    // Trim trailing whitespace
    while (end >= (int32_t)start && std::isspace(static_cast<unsigned char>(data[end]))) 
    {
        --end;
    }

    uint32_t new_len = (uint32_t)(end - (int32_t)start + 1);

    // safe overlap move
    std::memmove(data, data + start, new_len);
    data[new_len] = '\0';   // Null-terminate the string
}

void Stream_utility::trimString(const char* data, char* buffer, uint32_t max_size)
{
    if (data == nullptr || buffer == nullptr) return;

    uint32_t len = 0;

    if (max_size != 0)
        len = static_cast<uint32_t>(safe_strnlen(data, max_size));
    else
        len = static_cast<uint32_t>(std::strlen(data));
    
    if (len == 0) { buffer[0] = '\0'; return; }

    // if(max_size != 0)
    // {
    //     // Ensure null termination within the max_size limit
    //     buffer[max_size - 1] = '\0';
    // }

    uint32_t start = 0;
    
    int32_t end = static_cast<int32_t>(len) - 1;  // Find the end of the string. Use signed index to avoid underflow

    // Trim leading whitespace
    while (start <= static_cast<uint32_t>(end) && std::isspace(static_cast<unsigned char>(data[start])))
        ++start;

    // If there's nothing left after trimming leading spaces, empty the string
    if (start > static_cast<uint32_t>(end))
    {
        buffer[0] = '\0';
        return;
    }

    // Trim trailing whitespace
    while (end >= static_cast<int32_t>(start) && std::isspace(static_cast<unsigned char>(data[end])))
        --end;

    // Check if the buffer is large enough for the result
    uint32_t trimmed_len = static_cast<uint32_t>(end - static_cast<int32_t>(start) + 1);

    // cap to output buffer
    if (max_size != 0 && trimmed_len >= max_size)
        trimmed_len = max_size - 1;

    // Copy the trimmed string into the buffer and null-terminate
    std::memcpy(buffer, data + start, trimmed_len);
    buffer[trimmed_len] = '\0';  // Null-terminate the string
}

bool Stream_utility::splitString(const char* data, char delimiter, char* firstSection, size_t firstSize, char* secondSection, size_t secondSize) 
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

    size_t firstLength = delimiterPos - data;
    const char* secondStart = delimiterPos + 1;
    size_t secondLength = std::strlen(secondStart);

    if(firstSection != nullptr)
    {
        if (firstLength + 1 > firstSize)
            return false;

        std::memcpy(firstSection, data, firstLength);
        firstSection[firstLength] = '\0';       // Null-terminate the string
    }

    if(secondSection != nullptr)
    {
        if (secondLength + 1 > secondSize)
            return false;


        std::memcpy(secondSection, secondStart, secondLength + 1);
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
    if (!data || expectedColumnCount == 0 || *data == '\0')
        return false;

    const size_t n = std::strlen(data);
    char* fields = new char[n + 1];
    std::strcpy(fields, data);

    size_t fieldCount = 1;      // number of columns = commas+1

    char* cursor = fields;

    while (true)
    {
        char* comma = std::strchr(cursor, ',');
        if (!comma) break;

        *comma = '\0';

        trimString(cursor);
        if (isWhitespaceOnly(cursor)) { delete[] fields; return false; }

        fieldCount++;
        cursor = comma + 1;
    }

    trimString(cursor);
    if (isWhitespaceOnly(cursor)) { delete[] fields; return false; }

    delete[] fields;
    return (fieldCount == expectedColumnCount);
}

bool Stream_utility::isUInt8(const char* str) 
{
    if(isUintiger(str) == false)
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
    if(isUintiger(str) == false)
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
    unsigned long value = strtoul(str, &endPtr, 10);

    // Ensure the whole string is processed and within uint32_t range
    return (*endPtr == '\0' && value <= std::numeric_limits<uint32_t>::max());
}

bool Stream_utility::isUInt64(const char* str) 
{
    if(isUintiger(str) == false)
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
    if (!num) return false;

    if(isUInt32(str) == false)
    {
        return false;
    }

    char* end = nullptr;
    unsigned long v = std::strtoul(str, &end, 10);
    if (*end != '\0') 
    {
        // Invalid characters in input
        return false;
    }

    // Check if the value fits within the uint32_t range
    if (v > std::numeric_limits<uint32_t>::max())
        return false;

    *num = static_cast<uint32_t>(v); // Successfully convert to uint8_t
    
    return true;   
}

bool Stream_utility::stringToUint64(const char* str, uint64_t* num)
{
    if(isUInt64(str) == false)
    {
        return false;
    }
    
    char* end;
    uint64_t value = static_cast<uint64_t>(strtoull(str, &end, 10));
    if (*end != '\0') 
    {
        // Invalid characters in input
        return false;
    }

    *num = value;

    return true;   
}

bool Stream_utility::stringToInt8(const char* str, int8_t* num)
{
    if(isInt8(str) == false)
    {
        return false;
    }

    char* end;
    int32_t value = static_cast<int32_t>(strtol(str, &end, 10));
    if (*end != '\0') 
    {
        // Invalid characters in input
        return false;
    }

    // Check if the value fits within the int8_t range (-128 to 127)
    if (value < -128 || value > 127) {
        return false; // The value is out of range for int8_t
    }

    *num = (int8_t)(value); // Successfully convert to int8_t
    
    return true;
}

bool Stream_utility::stringToInt16(const char* str, int16_t* num)
{
    if(isInt16(str) == false)
    {
        return false;
    }

    char* end;
    int32_t value = static_cast<int32_t>(strtol(str, &end, 10));
    if (*end != '\0') 
    {
        // Invalid characters in input
        return false;
    }

    // Check if the value fits within the int16_t range (-32768 to 32767)
    if (value < -32768 || value > 32767) {
        return false; // The value is out of range for int16_t
    }

    *num = (int16_t)(value); // Successfully convert to int16_t
    
    return true;
}

bool Stream_utility::stringToInt32(const char* str, int32_t* num)
{
    if(isInt32(str) == false)
    {
        return false;
    }

    char* end;
    int32_t value = static_cast<int32_t>(strtol(str, &end, 10));
    if (*end != '\0') 
    {
        // Invalid characters in input
        return false;
    }

    *num = value;

    return true;
}

bool Stream_utility::stringToInt64(const char* str, int64_t* num)
{
    if(isInt64(str) == false)
    {
        return false;
    }

    char* end;
    int64_t value = static_cast<int64_t>(strtoll(str, &end, 10));
    if (*end != '\0') 
    {
        // Invalid characters in input
        return false;
    }

    *num = value;

    return true;
}

bool Stream_utility::stringToFloat(const char* str, float* num)
{
    if(isFloat(str) == false)
    {
        return false;
    }

    char* end;
    float value = static_cast<float>(strtof(str, &end));
    if (*end != '\0') 
    {
        // Invalid characters in input
        return false;
    }

    *num = value;
    return true;
}

bool Stream_utility::stringToDouble(const char* str, double* num)
{
    if(isDouble(str) == false)
    {
        return false;
    }

    char* end;
    double value = static_cast<double>(strtod(str, &end));
    if (*end != '\0') 
    {
        // Invalid characters in input
        return false;
    }

    *num = value;

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
            num->boolValue = (std::strcmp(str, "true") == 0 || std::strcmp(str, "TRUE") == 0 || std::strcmp(str, "1") == 0);
        break;
        case stringType:
            std::strncpy(num->stringValue, str, sizeof(num->stringValue) - 1);
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

#if defined(_PLATFORM_PC_)
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
#endif

void Stream_utility::dataValueToString(char *str, const dataValueUnion& value, const dataTypeEnum type)
{
    switch (type)
    {
        case uint8Type:
            sprintf(str, "%u", value.uint8Value);
            break;
        case uint16Type:
            sprintf(str, "%u", value.uint16Value);
            break;
        case uint32Type:
            sprintf(str, "%lu", value.uint32Value);
            break;
        case int8Type:
            sprintf(str, "%d", value.int8Value);
            break;
        case int16Type:
            sprintf(str, "%d", value.int16Value);
            break;
        case int32Type:
            sprintf(str, "%ld", value.int32Value);
            break;
        case floatType:
            sprintf(str, "%f", value.floatValue);
            break;
        case doubleType:
            sprintf(str, "%f", value.doubleValue);
            break;
        case boolType:
            if(value.boolValue == true)
            {
                sprintf(str, "true");
            }
            else
            {
                sprintf(str, "false");
            }
            break;
        case stringType:
            sprintf(str, "%s", value.stringValue);
            break;
        default:
            sprintf(str, "Unsupported Type");
            break;
    }
}

// ###########################################################################################################
// Stream class:

Stream::Stream(char* txBuffer, uint32_t txBufferSize, char* rxBuffer, uint32_t rxBufferSize, BufferType txType, BufferType rxType)
{
    _txBufferSize = txBufferSize;
    _rxBufferSize = rxBufferSize;

    _txBuffer = txBuffer;
    _rxBuffer = rxBuffer;

    _txPosition = 0;
    _rxPosition = 0;

    _txType = txType;
    _rxType = rxType;

    _txHead = _txTail = 0;
    _rxHead = _rxTail = 0;

    // Null-terminate the remaining buffer (optional for string usage)
    if (_txBuffer && _txBufferSize) memset(_txBuffer, 0, _txBufferSize);
    if (_rxBuffer && _rxBufferSize) memset(_rxBuffer, 0, _rxBufferSize);

    errorCode = STREAM_OK;
}

void Stream::setTxBuffer(char* txBuffer, uint32_t txBufferSize, BufferType txType)
{
    _txBufferSize = txBufferSize;
    _txBuffer = txBuffer;
    _txType = txType;
    clearTxBuffer();
}

void Stream::setRxBuffer(char* rxBuffer, uint32_t rxBufferSize, BufferType rxType)
{
    _rxBufferSize = rxBufferSize;
    _rxBuffer = rxBuffer;
    _rxType = rxType;
    clearRxBuffer();
}

void Stream::setBufferTypes(BufferType txType, BufferType rxType)
{
    _txType = txType;
    _rxType = rxType;
    clearTxBuffer();
    clearRxBuffer();
}

const char* Stream::getTxBuffer() const
{
    return _txBuffer;       // always base pointer
}

uint32_t Stream::getTxBufferSize() const
{
    return _txBufferSize;
}

const char* Stream::getRxBuffer() const
{
    return _rxBuffer;       // always base pointer
}

const char* Stream::txReadPtr() const
{
    if (_txType == BUFFER_RING)
    {
        if (!_txBuffer || _txBufferSize < 2) return nullptr;
        return &_txBuffer[_txTail];
    }
    return _txBuffer;
}

const char* Stream::rxReadPtr() const
{
    if (_rxType == BUFFER_RING)
    {
        if (!_rxBuffer || _rxBufferSize < 2) return nullptr;
        return &_rxBuffer[_rxTail];
    }
    return _rxBuffer;
}

uint32_t Stream::getRxBufferSize() const
{
    return _rxBufferSize;
}

void Stream::clearTxBuffer() 
{
    errorCode = STREAM_OK;
    if (_txBuffer && _txBufferSize) memset(_txBuffer, 0, _txBufferSize);    // Zero out TX buffers
    
    _txPosition = 0;
    _txHead = _txTail = 0;
}

void Stream::clearRxBuffer() 
{
    errorCode = STREAM_OK;
    if (_rxBuffer && _rxBufferSize) memset(_rxBuffer, 0, _rxBufferSize);    // Zero out RX buffer
    
    _rxPosition = 0;
    _rxHead = _rxTail = 0;
}

uint32_t Stream::txContiguousSize() const
{
    if (_txType != BUFFER_RING) return availableTx();
    if (!_txBuffer || _txBufferSize < 2) return 0;

    uint32_t tail1, tail2, head;
    do
    {
        tail1 = _txTail;
        head  = _txHead;
        tail2 = _txTail;
    } while (tail1 != tail2);

    uint32_t used = (head >= tail1) ? (head - tail1) : (_txBufferSize - (tail1 - head));
    uint32_t toEnd = _txBufferSize - tail1;
    return (used < toEnd) ? used : toEnd;
}

bool Stream::txPeekContiguous(const char*& ptr, uint32_t& len) const
{
    ptr = nullptr;
    len = 0;

    if (_txType != BUFFER_RING)
    {
        if (!_txBuffer || _txBufferSize < 2) return false;
        ptr = _txBuffer;
        len = _txPosition;
        return true;
    }

    if (!_txBuffer || _txBufferSize < 2) return false;

    uint32_t tail1, tail2, head;
    do
    {
        tail1 = _txTail;
        head  = _txHead;
        tail2 = _txTail;
    } while (tail1 != tail2);

    uint32_t used = (head >= tail1) ? (head - tail1) : (_txBufferSize - (tail1 - head));
    uint32_t toEnd = _txBufferSize - tail1;

    len = (used < toEnd) ? used : toEnd;
    ptr = (len == 0) ? nullptr : &_txBuffer[tail1];
    return true;
}

uint32_t Stream::rxContiguousSize() const
{
    if (_rxType != BUFFER_RING) return availableRx();
    if (!_rxBuffer || _rxBufferSize < 2) return 0;

    uint32_t tail1, tail2, head;
    do
    {
        tail1 = _rxTail;
        head  = _rxHead;
        tail2 = _rxTail;
    } while (tail1 != tail2);

    uint32_t used = (head >= tail1) ? (head - tail1) : (_rxBufferSize - (tail1 - head));
    uint32_t toEnd = _rxBufferSize - tail1;
    return (used < toEnd) ? used : toEnd;
}

uint32_t Stream::freeTx() const
{
    if(!_txBuffer || _txBufferSize < 2) return 0;
    uint32_t used = availableTx();
    uint32_t cap = _txBufferSize - 1;
    return (used >= cap) ? 0 : (cap - used);
}

uint32_t Stream::freeRx() const
{
    if(!_rxBuffer || _rxBufferSize < 2) return 0;
    uint32_t used = availableRx();
    uint32_t cap = _rxBufferSize - 1;
    return (used >= cap) ? 0 : (cap - used);
}

bool Stream::writeTxBuffer(const char* data, uint32_t dataSize) 
{
    errorCode = STREAM_OK;

    if (!_txBuffer || _txBufferSize < 2) { errorCode = STREAM_ERR_PARAM; return false; }
    if (dataSize == 0) { errorCode = STREAM_ERR_SIZE_ZERO; return false; }
    if (data == nullptr) { errorCode = STREAM_ERR_PARAM; return false; }

    // capacity is (size - 1)
    if (dataSize > (_txBufferSize - 1)) 
    {
        errorCode = STREAM_ERR_PARAM;  // "Error Stream: Data size exceeds TX buffer size.";
        return false;
    }

    clearTxBuffer();
    return pushBackTxBuffer(data, dataSize);
}

bool Stream::writeRxBuffer(const char* data, uint32_t dataSize) 
{
    errorCode = STREAM_OK;

    if (!_rxBuffer || _rxBufferSize < 2) { errorCode = STREAM_ERR_PARAM; return false; }
    if (dataSize == 0) { errorCode = STREAM_ERR_SIZE_ZERO; return false; }
    if (data == nullptr) { errorCode = STREAM_ERR_PARAM; return false; }

    if (dataSize > (_rxBufferSize - 1)) 
    {
        errorCode = STREAM_ERR_PARAM;  // "Error Stream: Data size exceeds RX buffer size.";
        return false;
    }
    
    clearRxBuffer();
    return pushBackRxBuffer(data, dataSize);
}

bool Stream::pushBackTxBuffer(const char* data, uint32_t dataSize)
{
    errorCode = STREAM_OK;

    if (data == nullptr) { errorCode = STREAM_ERR_PARAM; return false; }
    if (dataSize == 0) { errorCode = STREAM_ERR_SIZE_ZERO; return false; }
    if (!_txBuffer || _txBufferSize < 2) { errorCode = STREAM_ERR_PARAM; return false; }

    // If message is larger than capacity, keep only the last part
    if(dataSize > (_txBufferSize - 1))
    {
        data += (dataSize - (_txBufferSize - 1));
        dataSize = (_txBufferSize - 1);
        errorCode = STREAM_ERR_OVERFLOW_OR_SHORT; // indicate truncation
    }

    bool ret = (errorCode == STREAM_OK);

    // TX producer-safe (main loop)
    if (_txType == BUFFER_RING)
    {
        const uint32_t cap = _txBufferSize - 1;

        if (dataSize > cap)
        {
            data += (dataSize - cap);
            dataSize = cap;
            errorCode = STREAM_ERR_OVERFLOW_OR_SHORT;
            ret = false;
        }

        const uint32_t free = freeTx();
        if (dataSize > free)
        {
            errorCode = STREAM_ERR_OVERFLOW_OR_SHORT;
            return false;
        }

        const uint32_t head = _txHead;
        const uint32_t toEnd = _txBufferSize - head;
        const uint32_t first = (dataSize < toEnd) ? dataSize : toEnd;

        std::memcpy(&_txBuffer[head], data, first);
        if (dataSize > first)
            std::memcpy(&_txBuffer[0], data + first, dataSize - first);

        STREAM_DMB();
        _txHead = (head + dataSize) % _txBufferSize;
        return ret;
    }

    // LINEAR
    uint32_t free = freeTx();
    if(free < dataSize)
    {
        errorCode = STREAM_ERR_OVERFLOW_OR_SHORT;
        ret = false;
        uint32_t need = dataSize - free;
        if(need > _txPosition) need = _txPosition;
        removeFrontTxBuffer(need);
    }

    // re-check space
    free = freeTx();
    if(free < dataSize) { errorCode = STREAM_ERR_OVERFLOW_OR_SHORT; return false; }

    // Copy the data into the buffer
    std::memcpy(&_txBuffer[_txPosition], data, dataSize);
    _txPosition += dataSize;    // Update the position
    _txBuffer[_txPosition] = '\0';
    return ret;
}

bool Stream::pushBackRxBuffer(const char* data, uint32_t dataSize)
{
    errorCode = STREAM_OK;

    if (data == nullptr) { errorCode = STREAM_ERR_PARAM; return false; }
    if (dataSize == 0) { errorCode = STREAM_ERR_SIZE_ZERO; return false; }
    if (!_rxBuffer || _rxBufferSize < 2) { errorCode = STREAM_ERR_PARAM; return false; }    

    if(dataSize > (_rxBufferSize - 1))
    {
        data += (dataSize - (_rxBufferSize - 1));
        dataSize = (_rxBufferSize - 1);
        errorCode = STREAM_ERR_OVERFLOW_OR_SHORT; // indicate truncation
    }

    bool ret = (errorCode == STREAM_OK);

    // RX producer-safe (ISR)
    if (_rxType == BUFFER_RING)
    {
        const uint32_t cap = _rxBufferSize - 1;

        if (dataSize > cap)
        {
            data += (dataSize - cap);
            dataSize = cap;
            errorCode = STREAM_ERR_OVERFLOW_OR_SHORT;
            ret = false;
        }

        const uint32_t free = freeRx();
        if (dataSize > free)
        {
            errorCode = STREAM_ERR_OVERFLOW_OR_SHORT;
            return false;
        }

        const uint32_t head = _rxHead;
        const uint32_t toEnd = _rxBufferSize - head;
        const uint32_t first = (dataSize < toEnd) ? dataSize : toEnd;

        std::memcpy(&_rxBuffer[head], data, first);
        if (dataSize > first)
            std::memcpy(&_rxBuffer[0], data + first, dataSize - first);

        STREAM_DMB();
        _rxHead = (head + dataSize) % _rxBufferSize;
        return ret;
    }

    // LINEAR
    uint32_t free = freeRx();
    if(free < dataSize)
    {
        errorCode = STREAM_ERR_OVERFLOW_OR_SHORT;
        ret = false;
        uint32_t need = dataSize - free;
        if(need > _rxPosition) need = _rxPosition;
        removeFrontRxBuffer(need);
    }
    free = freeRx();
    if (free < dataSize) { errorCode = STREAM_ERR_OVERFLOW_OR_SHORT; return false; }

    std::memcpy(&_rxBuffer[_rxPosition], data, dataSize);
    _rxPosition += dataSize;
    _rxBuffer[_rxPosition] = '\0';
    return ret;
}

#if defined(_PLATFORM_PC_)
    bool Stream::pushBackTxBuffer(const std::string& data)
    {
        return pushBackTxBuffer(data.c_str(), data.size());
    }

    bool Stream::pushBackRxBuffer(const std::string& data)
    {
        return pushBackRxBuffer(data.c_str(), data.size());
    }
#endif

bool Stream::popFrontTxBuffer(char* data, uint32_t dataSize)
{
    errorCode = STREAM_OK;

    if (data == nullptr) { errorCode = STREAM_ERR_PARAM; return false; }
    if (dataSize == 0) { errorCode = STREAM_ERR_SIZE_ZERO; return false; }
    if (!_txBuffer || _txBufferSize < 2) { errorCode = STREAM_ERR_PARAM; return false; }

    bool ret = true;

    uint32_t avail = availableTx();
    if (dataSize > avail) 
    {
        dataSize = avail;
        errorCode = STREAM_ERR_OVERFLOW_OR_SHORT; // Not enough data in the buffer to pop
        ret = false;
    }

    if (_txType == BUFFER_RING)
    {
        const uint32_t tail = _txTail;
        const uint32_t toEnd = _txBufferSize - tail;
        const uint32_t first = (dataSize < toEnd) ? dataSize : toEnd;

        std::memcpy(data, &_txBuffer[tail], first);
        if (dataSize > first)
            std::memcpy(data + first, &_txBuffer[0], dataSize - first);

        STREAM_DMB();
        _txTail = (tail + dataSize) % _txBufferSize;
        return ret;
    }

    // LINEAR
    // IMPORTANT: Do NOT write a '\0' terminator into the caller's output buffer.
    // These APIs are byte-oriented (binary safe). The caller owns sizing/termination.
    std::memcpy(data, _txBuffer, dataSize);

    std::memmove(_txBuffer, _txBuffer + dataSize, _txPosition - dataSize);
    _txPosition -= dataSize;
    _txBuffer[_txPosition] = '\0';
    return ret;
}

bool Stream::removeFrontTxBuffer(uint32_t dataSize)
{
    errorCode = STREAM_OK;
    if (dataSize == 0) return true;

    if (!_txBuffer || _txBufferSize < 2) { errorCode = STREAM_ERR_PARAM; return false; }

    uint32_t avail = availableTx();
    if (dataSize > avail) { errorCode = STREAM_ERR_PARAM; return false; }

    if (_txType == BUFFER_RING)
    {
        const uint32_t tail = _txTail;
        _txTail = (tail + dataSize) % _txBufferSize;
        return true;
    }

    // LINEAR
    std::memmove(_txBuffer, _txBuffer + dataSize, _txPosition - dataSize);
    _txPosition -= dataSize;
    _txBuffer[_txPosition] = '\0';
    return true;
}

bool Stream::removeFrontRxBuffer(uint32_t dataSize)
{
    errorCode = STREAM_OK;
    if (dataSize == 0) return true;

    if (!_rxBuffer || _rxBufferSize < 2) { errorCode = STREAM_ERR_PARAM; return false; }

    uint32_t avail = availableRx();
    if (dataSize > avail) { errorCode = STREAM_ERR_PARAM; return false; }

    if (_rxType == BUFFER_RING)
    {
        const uint32_t tail = _rxTail;
        _rxTail = (tail + dataSize) % _rxBufferSize;
        return true;
    }

    // LINEAR
    std::memmove(_rxBuffer, _rxBuffer + dataSize, _rxPosition - dataSize);
    _rxPosition -= dataSize;
    _rxBuffer[_rxPosition] = '\0';
    return true;
}

#if defined(_PLATFORM_PC_)
    bool Stream::popFrontTxBuffer(std::string& out, uint32_t dataSize)
    {
        out.clear();
        if (dataSize == 0)
        {
            errorCode = STREAM_ERR_SIZE_ZERO;
            return false;
        }

        uint32_t avail = availableTx();
        bool ret = true;
        if (dataSize > avail)
        {
            dataSize = avail;
            errorCode = STREAM_ERR_OVERFLOW_OR_SHORT;
            ret = false;
        }

        if (dataSize == 0) return ret; // nothing to pop

        // temporary buffer
        std::string tmp;
        tmp.resize(dataSize);

        if (!popFrontTxBuffer(&tmp[0], dataSize))
            return false;

        out.swap(tmp);
        return ret;
    }
#endif

bool Stream::popAllTxBuffer(char* data, uint32_t maxSize)
{
    return popFrontTxBuffer(data, maxSize);
}

#if defined(_PLATFORM_PC_)
    bool Stream::popAllTxBuffer(std::string& data)
    {
        // works for linear + ring
        return popFrontTxBuffer(data, availableTx());
    }
#endif

bool Stream::popFrontRxBuffer(char* data, uint32_t dataSize)
{
    errorCode = STREAM_OK;

    if (data == nullptr) { errorCode = STREAM_ERR_PARAM; return false; }
    if (dataSize == 0) { errorCode = STREAM_ERR_SIZE_ZERO; return false; }
    if (!_rxBuffer || _rxBufferSize < 2) { errorCode = STREAM_ERR_PARAM; return false; }

    bool ret = true;
    uint32_t avail = availableRx();
    if(dataSize > avail)
    {
        dataSize = avail;
        errorCode = STREAM_ERR_OVERFLOW_OR_SHORT;
        ret = false;
    }

    if (dataSize == 0) return ret;

    if (_rxType == BUFFER_RING)
    {
        const uint32_t tail = _rxTail;
        const uint32_t toEnd = _rxBufferSize - tail;
        const uint32_t first = (dataSize < toEnd) ? dataSize : toEnd;

        std::memcpy(data, &_rxBuffer[tail], first);
        if (dataSize > first)
            std::memcpy(data + first, &_rxBuffer[0], dataSize - first);

        STREAM_DMB();
        _rxTail = (tail + dataSize) % _rxBufferSize;
        return ret;
    }

    // LINEAR
    // IMPORTANT: Do NOT write a '\0' terminator into the caller's output buffer.
    // These APIs are byte-oriented (binary safe). The caller owns sizing/termination.
    std::memcpy(data, _rxBuffer, dataSize);

    std::memmove(_rxBuffer, _rxBuffer + dataSize, _rxPosition - dataSize);
    _rxPosition -= dataSize;
    _rxBuffer[_rxPosition] = '\0';
    return ret;
}

#if defined(_PLATFORM_PC_)
    bool Stream::popFrontRxBuffer(std::string& out, uint32_t dataSize)
    {
        out.clear();
        if (dataSize == 0)
        {
            errorCode = STREAM_ERR_SIZE_ZERO;
            return false;
        }

        uint32_t avail = availableRx();
        bool ret = true;
        if (dataSize > avail)
        {
            dataSize = avail;
            errorCode = STREAM_ERR_OVERFLOW_OR_SHORT;
            ret = false;
        }

        if (dataSize == 0) return ret;

        std::string tmp;
        tmp.resize(dataSize);

        if (!popFrontRxBuffer(&tmp[0], dataSize))
            return false;

        out.swap(tmp);
        return ret;
    }
#endif

bool Stream::popAllRxBuffer(char* data, uint32_t maxSize)
{
    return popFrontRxBuffer(data, maxSize);
}

#if defined(_PLATFORM_PC_)
    bool Stream::popAllRxBuffer(std::string& data)
    {
        return popFrontRxBuffer(data, availableRx());
    }
#endif

uint32_t Stream::availableTx() const
{
    if (!_txBuffer || _txBufferSize < 2) return 0;

    if (_txType == BUFFER_RING)
    {
        // Snapshot tail stable against TX-complete ISR
        uint32_t tail1, tail2, head;
        do
        {
            tail1 = _txTail;
            head  = _txHead;
            tail2 = _txTail;
        } while (tail1 != tail2);

        uint32_t used = (head >= tail1) ? (head - tail1) : (_txBufferSize - (tail1 - head));
        if (used >= _txBufferSize) used = _txBufferSize - 1; // paranoia clamp
        return used;
    }

    return _txPosition;
}

uint32_t Stream::availableRx() const
{
    if (!_rxBuffer || _rxBufferSize < 2) return 0;

    if (_rxType == BUFFER_RING)
    {
        uint32_t tail1, tail2, head;
        do
        {
            tail1 = _rxTail;
            head  = _rxHead;
            tail2 = _rxTail;
        } while (tail1 != tail2);

        uint32_t used = (head >= tail1) ? (head - tail1) : (_rxBufferSize - (tail1 - head));
        if (used >= _rxBufferSize) used = _rxBufferSize - 1;
        return used;
    }

    return _rxPosition;
}

char Stream::peekRx(size_t index) const
{
    if (_rxBuffer == nullptr || _rxBufferSize < 2)
        return 0;

    uint32_t avail = availableRx();
    if (index >= avail)
        return 0;

    if (_rxType == BUFFER_RING)
    {
        uint32_t physicalIndex = (_rxTail + static_cast<uint32_t>(index)) % _rxBufferSize;
        return _rxBuffer[physicalIndex];
    }

    return _rxBuffer[index];
}

size_t Stream::findRx(char delimiter) const
{
    if (_rxBuffer == nullptr || _rxBufferSize < 2)
        return SIZE_MAX;

    uint32_t avail = availableRx();
    for (size_t i = 0; i < avail; ++i)
    {
        if (peekRx(i) == delimiter)
            return i;
    }

    return SIZE_MAX;
}

size_t Stream::copyRxUntil(char delimiter, char* dst, size_t dstSize) const
{
    if (dst == nullptr || dstSize == 0)
        return SIZE_MAX;

    uint32_t avail = availableRx();
    if (avail == 0)
    {
        dst[0] = '\0';
        return 0;
    }

    size_t copied = 0;

    for (size_t i = 0; i < avail; ++i)
    {
        char c = peekRx(i);

        if (c == delimiter)
        {
            if (copied < dstSize)
                dst[copied] = '\0';
            else
                dst[dstSize - 1] = '\0';

            return copied;
        }

        if (copied + 1 >= dstSize)
        {
            dst[dstSize - 1] = '\0';
            return SIZE_MAX;
        }

        dst[copied++] = c;
    }

    dst[copied < dstSize ? copied : dstSize - 1] = '\0';
    return SIZE_MAX;
}

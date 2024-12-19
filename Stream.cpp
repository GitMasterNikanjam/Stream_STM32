// ####################################################################################################
// Include libraries:

#include "Stream.h"

// #####################################################################################################
// Public General functions

char* Stream_utility::trimString(const char* data) 
{
    if(data == NULL)
    {
        return NULL;
    }

    char * output = new char[strlen(data) + 1];
    std::string str;
    str.assign(data);

    size_t start = str.find_first_not_of(' ');  // Find first non-space character
    size_t end = str.find_last_not_of(' ');     // Find last non-space character

    if((start == std::string::npos))
    {
        strcmp(output, "");
        return output;
    }
    
    strcpy(output, str.substr(start, end - start + 1).c_str());
    
    return output;
}

std::string Stream_utility::trimString(std::string data) 
{
    uint32_t size = data.length();
    char* strchar = new char[size + 1];
    strcpy(strchar, data.c_str());
    strcpy(strchar, trimString(strchar));
    data.assign(strchar);

    return data;
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
    if(isIntiger(str) == false)
    {
        return false;
    }

    unsigned long value = 0;
    if (std::sscanf(str, "%lu", &value) == 1 && value <= std::numeric_limits<uint32_t>::max()) 
    {
        return true;
    }
    return false;
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

bool Stream_utility::stringToInt32(const char* str, int32_t* num)
{
    if(isIntiger(str) == false)
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

bool Stream_utility::stringToFloat(const char* str, float* num)
{
    if(isFloat(str) == false)
    {
        return false;
    }

    // Use sscanf to convert the string to an integer
    if (std::sscanf(str, "%f", num) != 1) 
    {
        return false; // sscanf failed to parse a valid integer
    }

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

// ###########################################################################################################
// Stream class:

Stream::Stream()
{
    _txBufferSize = 256;
    _rxBufferSize = 256;

    _txBuffer = new char[_txBufferSize]; // Allocate and zero-initialize TX buffer
    _rxBuffer = new char[_rxBufferSize]; // Allocate and zero-initialize RX buffer

    errorCode = 0;
}

Stream::~Stream()
{
    delete[] _txBuffer;
    delete[] _rxBuffer;
}

void Stream::setTxBufferSize(size_t size) 
{
    delete[] _txBuffer;
    _txBufferSize = size;
    _txBuffer = new char[_txBufferSize]; // Reallocate TX buffer
}

void Stream::setRxBufferSize(size_t size) 
{
    delete[] _rxBuffer;
    _rxBufferSize = size;
    _rxBuffer = new char[_rxBufferSize]; // Reallocate RX buffer
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

    if(_rxPosition + dataSize <= _rxBufferSize)
    {
        for (size_t i = 0; i < dataSize; ++i) 
        {
            _rxBuffer[_rxPosition] = data[i];
            ++_rxPosition; // Move position forward after writing
        }
    }
    else
    {
        errorCode = 2;   // "RX Buffer Overflow");
        return false;  
    }

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
    return strlen(_txBuffer);
}

size_t Stream::availableRx() 
{
    return strlen(_rxBuffer);
}



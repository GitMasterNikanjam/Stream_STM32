#pragma once

/**
 * @file Stream.h
 * @brief Lightweight TX/RX buffer manager for embedded targets.
 *
 * This module provides:
 * - A Stream class that manages two user-provided buffers (TX and RX).
 * - Optional buffer behavior: linear buffer or ring (circular) buffer.
 * - Utility functions for parsing/trimming C strings and type conversion.
 *
 * @note This library is **non-owning**: it does NOT allocate/free TX/RX buffers.
 *       You must provide valid memory for buffers for the lifetime of the Stream object.
 *
 * @note No dynamic allocation is performed by Stream itself.
 *       (Exception: your Stream_utility::validateRow() currently uses `new[]` in .cpp.)
 *
 * @warning Concurrency / ISR note:
 * - If one context writes (ISR) and another reads (main loop), protect shared state
 *   or design as single-producer/single-consumer with careful access rules.
 */

// ####################################################################################################
// Include libraries:

#include <cstring>  ///< Provides functions for manipulating C-style strings (e.g., strcpy(), strlen(), strcat(), memcpy, memmove, memset).
#include <cstdio>   ///< Includes functions for input/output operations, such as formatted string printing (printf(), sprintf()).
#include <cstdint>  ///< Defines fixed-width integer types (int32_t, uint64_t, etc.) and limits for platform-independent integer handling.
#include <cstddef>  ///< size_t

#if defined(__linux__)
    #include <string>   // Provides the std::string class for working with dynamic strings in C++
#endif

// ###################################################################################################
// Define global macros:

// -------------------------------------------------------------------------------------------------
// Memory barrier for ISR <-> main-loop visibility (ring-buffer mode)
// On Cortex-M, a DMB ensures buffer writes are visible before publishing head/tail updates.
// You can override by defining STREAM_DMB() before including Stream.h.
// -------------------------------------------------------------------------------------------------
#ifndef STREAM_DMB
  #if (defined(__arm__) || defined(__thumb__) || defined(__ARM_ARCH)) && (defined(__GNUC__) || defined(__clang__))
    #define STREAM_DMB() __asm volatile("dmb 0xF" ::: "memory")
  #elif defined(__DMB)  // CMSIS intrinsic (if available)
    #define STREAM_DMB() __DMB()
  #else
    #define STREAM_DMB() do {} while(0)
  #endif
#endif

// ###################################################################################################
// Buffer type selection

/**
 * @enum BufferType
 * @brief Select internal buffer behavior.
 *
 * - BUFFER_LINEAR:
 *   Data always starts at index 0. Removing from front uses `memmove()` to shift.
 *
 * - BUFFER_RING:
 *   Circular buffer with head/tail indices. Removing from front does NOT use memmove.
 *
 * @note In ring mode, effective capacity is (bufferSize - 1) bytes.
 *       One byte is reserved so you can keep a '\0' terminator for string compatibility.
 */
enum BufferType : uint8_t 
{
    BUFFER_LINEAR = 0,      ///< Linear buffer (memmove on pop/remove)
    BUFFER_RING   = 1       ///< Ring buffer (head/tail, no memmove)
};

// ###################################################################################################
// Data type enumaration and value union :

/**
 * @enum dataTypeEnum
 * @brief Enum value for data type. eg: uint8, float, int32, ... .
 */
enum dataTypeEnum : uint8_t 
{
    noneType = 0,
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
 * @union dataValueUnion
 * @brief Stores one value of different supported types.
 *
 * This union is useful for passing values without heap allocation.
 *
 * @warning Union active-member rule:
 * Only one field is logically "active" at a time. Track the active type separately
 * (e.g., with a dataTypeEnum) when using it.
 *
 * @note stringValue holds up to 7 chars + '\0'. stringPointerValue is non-owning.
 */
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
    char stringValue[8];            ///< inline short string (max 7 chars + '\0')
    char* stringPointerValue;       ///< pointer to external string (non-owning)
    bool boolValue;

    /**
     * @brief Clear the union to all zeros.
     * @note Does NOT allocate heap memory.
     */
    void clear() { std::memset(this, 0, sizeof(*this)); }

    /**
     * @brief Set the union to hold a string pointer value.
     * @param p Pointer to external buffer/string (non-owning).
     */
    void setStringPointer(char* p)
    {
        clear();
        stringPointerValue = p;
    }
};

/**
 * @enum StreamError
 * @brief Error codes reported by Stream operations.
 *
 * Most Stream methods update `Stream::errorCode` when something goes wrong.
 * This enum documents the intended meaning of those values.
 *
 * @note Some functions may return `false` and still set `errorCode` to
 *       STREAM_ERR_OVERFLOW_OR_SHORT when they partially succeed (e.g. pop less
 *       than requested, or push after dropping oldest bytes in ring mode).
 */
enum StreamError : int8_t
{
    /** @brief No error. Operation completed as requested. */
    STREAM_OK = 0,

    /**
     * @brief Invalid parameter / invalid state.
     *
     * Typical reasons:
     * - Null buffer pointer (TX/RX not configured)
     * - Buffer size too small (e.g. size < 2 for capacity rule)
     * - Request exceeds capacity in APIs that require full fit
     * - Attempt to remove more bytes than available (in remove APIs)
     */
    STREAM_ERR_PARAM = 1,

    /**
     * @brief Overflow or short read/write condition.
     *
     * Typical reasons:
     * - Push: not enough free space (overflow). Some implementations may drop
     *   oldest bytes to make room and still return `false` or `true` depending on policy.
     * - Pop: requested more bytes than available, so fewer bytes were returned.
     */
    STREAM_ERR_OVERFLOW_OR_SHORT = 2,

    /**
     * @brief Invalid size argument (typically dataSize == 0).
     *
     * Used when a function requires a non-zero size but was called with zero.
     */
    STREAM_ERR_SIZE_ZERO = 3
};


// ####################################################################################################
// Stream_utility namespace

/**
 * @namespace Stream_utility
 * @brief Helper functions for string manipulation, validation, and conversion.
 *
 * These are mainly designed for embedded environments using C strings.
 */
namespace Stream_utility
{

/**
 * @brief Safe bounded strlen implementation (Keil-friendly).
 * @param str Input C-string.
 * @param max_len Maximum length to scan.
 * @return Length up to max_len (stops at '\0' or max_len).
 */
size_t safe_strnlen(const char* str, size_t max_len);

/**
 * @brief Function to trim leading and trailing spaces from a string.
 * @param data: is the Input string that wanted to trim
 * @param max_size: the maximum length of the data buffer or the maximum length to trim to.
 * @note - Trimmed string stored in data.
 * @note - If max_size is 0, it means the maximum size for trimming is disabled.
 */
void trimString(char* data, uint32_t max_size = 0);

/**
 * @brief Function to trim leading and trailing spaces from a string.
 * @param data: is the Input string that wanted to trim
 * @param buffer: is the buffer for store trimmed string.
 * @param max_size: the maximum length of the data buffer or the maximum length to trim to.
 * @note - Trimmed string stored in buffer.
 * @note - If max_size is 0, it means the maximum size for trimming is disabled.
 */
void trimString(const char* data, char* buffer, uint32_t max_size = 0);

/**
 * @brief Split string into two parts by delimiter.
 * @param[in]  data Input string.
 * @param[in]  delimiter Split character.
 * @param[out] firstSection Output first part (optional).
 * @param[in]  firstSize Size of firstSection buffer.
 * @param[out] secondSection Output second part (optional).
 * @param[in]  secondSize Size of secondSection buffer.
 * @return true if delimiter found and outputs fit.
 */
bool splitString(const char* data, char delimiter, char* firstSection = nullptr, size_t firstSize = 0, char* secondSection = nullptr, size_t secondSize = 0);

/**
 * @brief Function to check if a string is empty or contains only spaces.
 * @return true if succeeded.
 */
bool isWhitespaceOnly(const char* data);

/**
 * @brief Validate a comma-separated row has exactly expectedColumnCount non-empty fields.
 * @param[in] data Input CSV-like row.
 * @param[in] expectedColumnCount Expected number of comma-separated fields.
 * @return true if field count matches and no field is whitespace-only.
 * @warning Current implementation allocates a temporary copy using new[] in the .cpp.
 */
bool validateRow(const char* data, size_t expectedColumnCount);

// ---- type checks ----

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
 * @brief Convert decimal number to string with fixed precision.
 * @param value Input value.
 * @param precision Digits after decimal point.
 * @param[out] output Output buffer (must be large enough).
 */
void decimalToString(double value, uint8_t precision, char* output);

/**
 * @brief Convert decimal number to string with fixed precision.
 * @param value Input value.
 * @param precision Digits after decimal point.
 * @param[out] output Output buffer (must be large enough).
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
 * @brief Function to check a string are valid unsigned intiger number.
 * @note - '+' character at the first of string are acceptable.
 * @return true if succeeded.
 */
bool isUintiger(const char* str);

/**
 * @brief Function to check if a string ends with a given suffix.
 * @return true if succeeded.
 */
bool endsWith(const char* str, const char* suffix);

// ---- conversions ----

/**
 * @brief Convert string to Uint8 number.
 * @param num is the number for  store intiger value.
 * @return true if str is intiger.
 */
bool stringToUint8(const char* str, uint8_t* num);

/**
 * @brief Convert string to Uint16 number.
 * @param num is the number for  store intiger value.
 * @return true if str is intiger.
 */
bool stringToUint16(const char* str, uint16_t* num);

/**
 * @brief Convert string to Uint32 number.
 * @param num is the number for  store intiger value.
 * @return true if str is intiger.
 */
bool stringToUint32(const char* str, uint32_t* num);

/**
 * @brief Convert string to Uint64 number.
 * @param num is the number for  store intiger value.
 * @return true if str is intiger.
 */
bool stringToUint64(const char* str, uint64_t* num);

/**
 * @brief Convert string to Int8 number.
 * @param num is the number for  store intiger value.
 * @return true if str is intiger.
 */
bool stringToInt8(const char* str, int8_t* num);

/**
 * @brief Convert string to Int16 number.
 * @param num is the number for  store intiger value.
 * @return true if str is intiger.
 */
bool stringToInt16(const char* str, int16_t* num);

/**
 * @brief Convert string to Int32 number.
 * @param num is the number for  store intiger value.
 * @return true if str is intiger.
 */
bool stringToInt32(const char* str, int32_t* num);

/**
 * @brief Convert string to Int64 number.
 * @param num is the number for  store intiger value.
 * @return true if str is intiger.
 */
bool stringToInt64(const char* str, int64_t* num);

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
 * @brief Convert a string to a number according to type then store it.
 * @param str: is the string value that want converted.
 * @param num: is the dataValue union for store converted data.
 * @param typr: is the number type for converting.
 */
bool stringToNumber(const char* str, dataValueUnion* num, dataTypeEnum type);

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

#if defined(__linux__)
    /// @brief Helper function to convert dataValueUnion to string based on ParamType_t
    std::string dataValueToString(const dataValueUnion& value, const dataTypeEnum type);
#endif

/**
 * @brief Convert union to C-string representation.
 * @param[out] str Output buffer.
 * @param[in] value Input union.
 * @param[in] type Active type of union.
 */
void dataValueToString(char *str, const dataValueUnion& value, const dataTypeEnum type);

}

// ######################################################################################################
// Stream class

/**
 * @class Stream
 * @brief TX/RX buffer manager supporting linear and ring (circular) behavior.
 *
 * Stream does not allocate memory; you must provide TX/RX buffers.
 *
 * ## Capacity rule
 * For safety and optional '\0' termination, the effective capacity is:
 * - capacity = (bufferSize - 1)
 *
 * This means if bufferSize == 0 or 1, the buffer cannot store data.
 */
class Stream
{
public:

    /** @brief Last error code (see @ref StreamError). */
    int8_t errorCode = STREAM_OK;

    /**
     * @brief Construct Stream with optional buffers and buffer types.
     * @param txBuffer TX buffer pointer (non-owning).
     * @param txBufferSize TX buffer size in bytes.
     * @param rxBuffer RX buffer pointer (non-owning).
     * @param rxBufferSize RX buffer size in bytes.
     * @param txType TX buffer type (linear/ring).
     * @param rxType RX buffer type (linear/ring).
     */
    Stream(char* txBuffer = nullptr, uint32_t txBufferSize = 0, char* rxBuffer = nullptr, uint32_t rxBufferSize = 0, BufferType txType = BUFFER_LINEAR, BufferType rxType = BUFFER_LINEAR);

    /// @brief Destructor (non-owning; does not free buffers).
    ~Stream() = default;

    /**
     * @brief Prevent accidental shallow copy (common bug on embedded projects). 
     * @note ❌ “You are NOT allowed to copy a Stream object.”
     */
    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;

    /**
     * @brief Delete move. Keep it simple and safe.
     */
    Stream(Stream&&) = delete;
    Stream& operator=(Stream&&) = delete;

    /**
     * @brief Set TX buffer and its type. Clears existing TX state.
     * @param txBuffer Buffer pointer (must remain valid for Stream lifetime or until changed).
     * @param txBufferSize Buffer size in bytes.
     * @param txType Buffer mode (linear/ring).
     *
     * @note For storing any data, txBufferSize must be >= 2 (capacity is txBufferSize-1).
     */
    void setTxBuffer(char* txBuffer, uint32_t txBufferSize, BufferType txType = BUFFER_LINEAR);

    /**
     * @brief Set RX buffer and its type. Clears existing RX state.
     * @param rxBuffer Buffer pointer.
     * @param rxBufferSize Buffer size in bytes.
     * @param rxType Buffer mode (linear/ring).
     */
    void setRxBuffer(char* rxBuffer, uint32_t rxBufferSize, BufferType rxType = BUFFER_LINEAR);
    
    /**
     * @brief Change buffer types for TX and RX. Clears both states.
     * @warning Changing type discards current buffered data.
     */ 
    void setBufferTypes(BufferType txType, BufferType rxType);

    /**
     * @brief Return current TX buffer type.
     */
    BufferType getTxBufferType() const { return _txType; }

    /**
     * @brief Return current RX buffer type.
     */
    BufferType getRxBufferType() const { return _rxType; }

    /**
     * @brief Return TxBuffer size.
     */
    uint32_t getTxBufferSize() const;

    /**
     * @brief Return RxBuffer size.
     */
    uint32_t getRxBufferSize() const;

    /**
     * @brief Get base pointer to TX buffer memory (always buffer start).
     * @return Pointer to the start of the TX buffer (may be nullptr).
     *
     * @note This does NOT point to “first valid data” in ring mode.
     *       Use txReadPtr() for that purpose.
     */
    const char* getTxBuffer() const;
    
    /**
     * @brief Get base pointer to RX buffer memory (always buffer start).
     * @return Pointer to the start of the RX buffer (may be nullptr).
     *
     * @note This does NOT point to “first valid data” in ring mode.
     *       Use rxReadPtr() for that purpose.
     */
    const char* getRxBuffer() const;

    /**
     * @brief Pointer to first valid TX byte (read position).
     * @return In ring mode: &txBuffer[tail]. In linear mode: txBuffer base.
     *
     * @note In ring mode, data may wrap. Use txContiguousSize() to know how many bytes
     *       are contiguous from this pointer before wrapping to the buffer start.
     */
    const char* txReadPtr() const;

    /**
     * @brief Pointer to first valid RX byte to be read.
     * @return In ring mode: &rxBuffer[tail]. In linear mode: rxBuffer base.
     */
    const char* rxReadPtr() const;

    /**
     * @brief Contiguous TX bytes available from txReadPtr().
     * @return Number of bytes that can be read/transmitted in one linear chunk.
     *
     * @note If availableTx() > txContiguousSize() in ring mode, the remaining bytes are at
     *       the start of the buffer (getTxBuffer()).
     */
    uint32_t txContiguousSize() const;

    /**
     * @brief Atomically (lock-free) snapshot TX contiguous chunk.
     *
     * This avoids a race where txReadPtr() and txContiguousSize() are called separately
     * while the TX-complete ISR advances the tail.
     */
    bool txPeekContiguous(const char*& ptr, uint32_t& len) const;

    /**
     * @brief Number of contiguous valid bytes from rxReadPtr().
     *
     * In ring mode, valid data may wrap. This returns the chunk length until end-of-buffer.
     * In linear mode, this equals availableRx().
     */
    uint32_t rxContiguousSize() const;

    /**
     * @brief Bytes free for writing (without overflow).
     * @note Effective capacity is (bufferSize - 1).
     */
    uint32_t freeTx() const;

    /// @copydoc freeTx()
    uint32_t freeRx() const;

    /// @brief Clear TX buffer content and reset indices.
    void clearTxBuffer();

    /// @brief Clear RX buffer content and reset indices.
    void clearRxBuffer();

    /**
     * @brief Remove certain number elements from front of TX buffer.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Not enough data in the buffer to remove"
     *  */
    bool removeFrontTxBuffer(uint32_t dataSize = 1);

    /**
     * @brief Remove certain number elements from front of RX buffer.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Not enough data in the buffer to remove"
     *  */
    bool removeFrontRxBuffer(uint32_t dataSize = 1);

    /**
     * @brief Write data Tx buffer.
     * @note - Clear any previouse data on Tx buffer.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: Data size exceeds TX buffer size."
     */
    bool writeTxBuffer(const char* data, uint32_t dataSize);

    /**
     * @brief Write data Rx buffer.
     * @note - Clear any previouse data on Rx buffer.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: Data size exceeds RX buffer size."
     */
    bool writeRxBuffer(const char* data, uint32_t dataSize);

    /**
     * @brief Push back certain number charecter in to TxBuffer.
     * @param data is a char array pointer that you want to push back.
     * @param dataSize is the data length for push back.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Error Stream: TX Buffer Overflow"
     */
    bool pushBackTxBuffer(const char* data, uint32_t dataSize = 1);

    #if defined(__linux__)
        /**
         * @brief Push back certain number charecter in to TxBuffer.
         * @param data is a string pointer that you want to push back.
         * @return true if succeeded.
         * @note - Error code be 1 if: "Error Stream: data can not be null."
         * @note - Error code be 2 if: "Error Stream: TX Buffer Overflow"
         */
        bool pushBackTxBuffer(const std::string& data);
    #endif

    /**
     * @brief Push back certain number charecter in to RxBuffer.
     * @param data is a char array pointer that you want to push back.
     * @param dataSize is the data length for push back.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Error Stream: RX Buffer Overflow"
     */
    bool pushBackRxBuffer(const char* data, uint32_t dataSize = 1);

    #if defined(__linux__)
        /**
         * @brief Push back certain number charecter in to RxBuffer.
         * @param data is a string pointer that you want to push back.
         * @return true if succeeded.
         * @note - Error code be 1 if: "Error Stream: data can not be null."
         * @note - Error code be 2 if: "Error Stream: RX Buffer Overflow"
         */
        bool pushBackRxBuffer(const std::string& data);

        /**
         * @brief Pop certain number elements from front of TX buffer and remove them.
         * @param data is the string that poped front.
         * @return true if succeeded.
         * @note - Error code be 1 if: "Error Stream: data can not be null."
         * @note - Error code be 2 if: "Not enough data in the buffer to pop"
         *  */
        bool popFrontTxBuffer(std::string& data, uint32_t dataSize = 1);
    #endif

    /**
     * @brief Pop certain number elements from front of TX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Not enough data in the buffer to pop"
     *  */
    bool popFrontTxBuffer(char* data, uint32_t dataSize = 1);

    #if defined(__linux__)
        /**
         * @brief Pop certain number elements from front of RX buffer and remove them.
         * @param data is the string that poped front.
         * @return true if succeeded.
         * @note - Error code be 1 if: "Error Stream: data can not be null."
         * @note - Error code be 2 if: "Not enough data in the buffer to pop"
         *  */
        bool popFrontRxBuffer(std::string& data, uint32_t dataSize = 1);
    #endif

    /**
     * @brief Pop certain number elements from front RX buffer and remove them.
     * @param data is the string that poped front.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     * @note - Error code be 2 if: "Not enough data in the buffer to pop"
     *  */
    bool popFrontRxBuffer(char* data, uint32_t dataSize = 1);

    #if defined(__linux__)
        /**
         * @brief Pop all elements from front of TX buffer and remove them.
         * @param data is the string that poped front.
         * @return true if succeeded.
         * @note - Error code be 1 if: "Error Stream: data can not be null."
         *  */
        bool popAllTxBuffer(std::string& data);
    #endif

    /**
     * @brief Pop all elements from front of TX buffer and remove them.
     * @param data: is the string that poped front.
     * @param maxSize: is the max length that can poped from front of buffer.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     *  */
    bool popAllTxBuffer(char* data, uint32_t maxSize);

    #if defined(__linux__)
        /**
         * @brief Pop all elements from front of RX buffer and remove them.
         * @param data is the string that poped front.
         * @return true if succeeded.
         * @note - Error code be 1 if: "Error Stream: data can not be null."
         *  */
        bool popAllRxBuffer(std::string& data);
    #endif

    /**
     * @brief Pop all elements from front of RX buffer and remove them.
     * @param data: is the string that poped front.
     * @param maxSize: is the max length that can poped from front of buffer.
     * @return true if succeeded.
     * @note - Error code be 1 if: "Error Stream: data can not be null."
     *  */
    bool popAllRxBuffer(char* data, uint32_t maxSize);

    /**
     * @brief Return number of bytes currently stored in TX.
     */
    uint32_t availableTx() const;

    /**
     * @brief Return number of bytes currently stored in RX.
     */
    uint32_t availableRx() const;

private:

    /// @brief TX buffer base pointer
    char* _txBuffer = nullptr;

    /// @brief RX buffer base pointer
    char* _rxBuffer = nullptr;

    /// @brief TX buffer allocated size (bytes)
    uint32_t _txBufferSize = 0;

    /// @brief RX buffer allocated size (bytes)
    uint32_t _rxBufferSize = 0;

    /// @brief Linear mode: length of valid TX data
    uint32_t _txPosition = 0;

    /// @brief Linear mode: length of valid RX data
    uint32_t _rxPosition = 0;

    // Buffer type selection
    BufferType _txType = BUFFER_LINEAR;
    BufferType _rxType = BUFFER_LINEAR;

    // Ring-buffer state (only used when type == BUFFER_RING)
    volatile uint32_t _txHead = 0;     ///< written by producer only
    volatile uint32_t _txTail = 0;     ///< written by consumer only

    volatile uint32_t _rxHead = 0;     ///< written by producer only
    volatile uint32_t _rxTail = 0;     ///< written by consumer only
};



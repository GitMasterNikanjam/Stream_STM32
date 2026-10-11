// Host check: g++ -std=c++11 -Wall -Wextra src/Stream.cpp tests/stream_regression.cpp -o stream_regression
#include "../src/Stream.h"

#include <cassert>
#include <cstring>
#include <limits>
#include <string>

int main()
{
    using namespace Stream_utility;

    assert(isUInt32("4294967295") && !isUInt32("4294967296"));
    assert(isUInt64("18446744073709551615") && !isUInt64("18446744073709551616"));
    assert(isInt64("-9223372036854775808") && !isInt64("-9223372036854775809"));
    assert(isInt8("-128") && !isInt8("-129"));
    assert(isUInt16("65535") && !isUInt16("65536"));
    assert(!isNumber("+.") && !isIntiger("-") && !isUintiger("+"));
    assert(!isFloat("999999999999999999999999999999999999999999"));
    assert(!isDouble(std::string(400, '9').c_str()));

    assert(validateRow(" a, b ", 2));
    assert(!validateRow("a, ,b", 3));
    assert(!isWhitespaceOnly(nullptr));
    assert(safe_strnlen(nullptr, 5) == 0);
    assert(!endsWith("abc", nullptr));

    char first[16] = "a,b,c";
    char second[16] = {};
    assert(!splitString(first, '\0', first, sizeof(first), second, sizeof(second)));
    assert(splitString(first, ',', first, sizeof(first), second, sizeof(second)));
    assert(std::strcmp(first, "a") == 0 && std::strcmp(second, "b,c") == 0);
    char preserved[8] = "keep";
    assert(!splitString("a:too-long", ':', preserved, sizeof(preserved), second, 2));
    assert(std::strcmp(preserved, "keep") == 0);

    dataValueUnion value{};
    assert(!stringToNumber(nullptr, &value, boolType));
    assert(!stringToNumber("maybe", &value, boolType));
    assert(isBoolean("0") && isBoolean("1"));
    assert(checkValueType("0", boolType) && checkValueType("1", "bool"));
    assert(stringToNumber("1", &value, boolType) && value.boolValue);
    assert(stringToNumber("1234567", &value, stringType));
    assert(!stringToNumber("12345678", &value, stringType));
    assert(std::strcmp(value.stringValue, "1234567") == 0);
    assert(stringToNumber(value.stringValue, &value, stringType));
    assert(std::strcmp(value.stringValue, "1234567") == 0);
    assert(checkValueType("A", charType) && !checkValueType("AB", charType));
    assert(stringToNumber("A", &value, charType));
    assert(dataValueToString(value, charType) == "A");
    assert(!stringToInt16("1", nullptr));

    char formatted[32] = {};
    value.uint64Value = std::numeric_limits<uint64_t>::max();
    dataValueToString(formatted, value, uint64Type);
    assert(std::strcmp(formatted, "18446744073709551615") == 0);
    value.int64Value = std::numeric_limits<int64_t>::min();
    assert(dataValueToString(value, int64Type) == "-9223372036854775808");

    char tx[5], rx[5], output[8] = {};
    Stream stream(tx, sizeof(tx), rx, sizeof(rx));
    assert(stream.pushBackTxBuffer("abcd", 4));
    assert(!stream.pushBackTxBuffer("e", 1));
    assert(stream.errorCode == STREAM_ERR_OVERFLOW_OR_SHORT);
    assert(stream.popAllTxBuffer(output, sizeof(output)));
    assert(std::memcmp(output, "bcde", 4) == 0);
    assert(stream.pushBackRxBuffer("abcd", 4));
    assert(!stream.pushBackRxBuffer("e", 1));
    assert(stream.errorCode == STREAM_ERR_OVERFLOW_OR_SHORT);
    assert(stream.popAllRxBuffer(output, sizeof(output)));
    assert(std::memcmp(output, "bcde", 4) == 0);

    assert(stream.pushBackTxBuffer("ab", 2));
    std::string text;
    assert(!stream.popFrontTxBuffer(text, 3));
    assert(text == "ab" && stream.errorCode == STREAM_ERR_OVERFLOW_OR_SHORT);
    assert(stream.pushBackRxBuffer("xy", 2));
    assert(!stream.popFrontRxBuffer(text, 3));
    assert(text == "xy" && stream.errorCode == STREAM_ERR_OVERFLOW_OR_SHORT);
}

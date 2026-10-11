# Stream_STM32

`Stream_STM32` is a small C++ byte-stream buffer for STM32 projects. It provides transmit (TX) and receive (RX) buffers, with either linear or ring-buffer behavior. The application supplies the memory used by each buffer.

## Features

- Independent TX and RX buffers.
- Linear and ring buffer modes.
- Byte-oriented push, peek, search, copy, and pop operations.
- Optional `std::string` overloads on supported PC builds.
- String validation, splitting, trimming, numeric conversion, and value formatting helpers.
- No dynamic allocation for the core TX/RX buffers.

## Add the library to a project

Add `Libraries/Stream_STM32/src` to the compiler's include paths, add `src/Stream.cpp` to the build, and include the header:

```cpp
#include "Stream.h"
```

The library requires C++11 or newer. It is not header-only: `Stream.cpp` must be compiled and linked into the application.

The repository includes example projects for STM32F407VGT6 and STM32F103C8T6 under `examples/`.

## Create a stream

The constructor accepts separate TX and RX memory regions and their sizes. The buffers must remain alive for as long as the `Stream` object uses them. The constructor clears the supplied buffers.

```cpp
#include "Stream.h"

char txMemory[128];
char rxMemory[128];

Stream stream(txMemory, sizeof(txMemory),
              rxMemory, sizeof(rxMemory),
              BUFFER_RING, BUFFER_RING);
```

The default mode is `BUFFER_LINEAR`. You can choose each direction independently.

## Buffer modes and capacity

Both modes reserve one byte, so a buffer of `N` bytes can hold at most `N - 1` data bytes. A configured size below 2 has no usable capacity.

| Mode | Behavior | When full |
| --- | --- | --- |
| `BUFFER_LINEAR` | Data is stored from the beginning of the buffer. Removing bytes shifts remaining data toward the beginning. | A push keeps the newest bytes that fit, dropping older buffered data as needed. The operation reports overflow even if it inserted data. |
| `BUFFER_RING` | Data is stored in a circular region and can wrap around the end. Removing bytes advances the read position without shifting the rest. | A push that does not fit is rejected and existing data is preserved. |

The ring buffer reserves one slot to distinguish full from empty. Neither mode silently grows its memory.

## Basic use

```cpp
const char message[] = "hello";

if (!stream.pushBackTxBuffer(message, sizeof(message) - 1)) {
    // Check stream.errorCode; a linear buffer may have accepted only part
    // of the requested data.
}

// A driver or transmit-complete handler can read the queued TX bytes.
uint32_t queued = stream.availableTx();

char received[16];
if (stream.popFrontRxBuffer(received, sizeof(received), 5)) {
    // Five bytes were copied and removed. Byte output is not automatically
    // null-terminated; add a terminator yourself if treating it as text.
}
```

`pushBackTxBuffer()` and `pushBackRxBuffer()` append bytes. `writeTxBuffer()` and `writeRxBuffer()` replace the current contents after validating the input. For example:

```cpp
const char command[] = "status";
if (!stream.writeTxBuffer(command, sizeof(command) - 1)) {
    // Invalid input or an unconfigured buffer.
}
```

## Reading, peeking, and searching

- `availableTx()` / `availableRx()` return the number of queued bytes.
- `freeTx()` / `freeRx()` return the remaining usable capacity.
- `peekRx(index)` reads one byte without removing it. It returns `0` when the index is out of range, so use `availableRx()` to distinguish that case from an actual zero byte.
- `findRx(value)` returns the offset of the first matching byte, or `SIZE_MAX` if it is not present.
- `copyRxUntil(delimiter, destination, destinationSize)` copies bytes up to and including the delimiter. It returns the copied byte count, or `SIZE_MAX` if the delimiter is missing or the destination is too small. With no queued data it returns `0` and clears a valid destination.
- `popFrontTxBuffer()` / `popFrontRxBuffer()` copy and remove bytes from the front.
- `removeFrontTxBuffer()` / `removeFrontRxBuffer()` remove bytes without copying them out.
- `popAllTxBuffer()` / `popAllRxBuffer()` copy and remove up to the destination's maximum size. If the destination is smaller than the queued data, the remaining bytes stay in the stream.

Byte-buffer output is not guaranteed to end in `\0`. Treat it as bytes unless you explicitly add a terminator and have reserved space for it.

## TX and RX ring buffers with interrupts or DMA

Ring mode avoids shifting data, so it is useful for continuous UART traffic. A common arrangement is one producer and one consumer per direction:

- TX: application code queues bytes; the UART/DMA completion path consumes them.
- RX: the UART receive path queues bytes; application code consumes them.

The implementation includes ordering barriers for supported Cortex toolchains, but this is not a general multi-threaded or multi-producer buffer. Keep one producer and one consumer for a given direction. Do not change buffer configuration or clear a buffer while an interrupt or DMA operation is using it; coordinate those operations in the application.

Ring data may occupy two physical regions when it wraps. `txReadPtr()` / `rxReadPtr()` point to the first currently readable byte, and `txContiguousSize()` / `rxContiguousSize()` report how many bytes are contiguous from that pointer. Use `txPeekContiguous(const char*& ptr, uint32_t& length)` to obtain a TX pointer and contiguous length together. A DMA transfer should not be assumed to cover all queued data when the returned region ends at the buffer boundary; consume that region, then request the next one.

`getTxBuffer()` and `getRxBuffer()` return the base addresses of the configured memory, not necessarily the first logical byte in ring mode.

## Buffer configuration and clearing

- `setTxBuffer()` / `setRxBuffer()` assign and clear the supplied memory.
- `setBufferTypes()` changes the TX/RX modes and clears both buffers, discarding queued data.
- `clearTxBuffer()` / `clearRxBuffer()` discard queued data.

Only call these configuration or clear operations when the relevant producer, consumer, interrupt, and DMA activity is stopped or otherwise synchronized.

## Errors and partial operations

The most recent result is stored in the public `errorCode` field.

| Code | Meaning |
| --- | --- |
| `STREAM_OK` | Operation completed successfully. |
| `STREAM_ERR_PARAM` | Invalid pointer, size, configuration, or request. |
| `STREAM_ERR_OVERFLOW_OR_SHORT` | A push overflowed, or a read requested more bytes than were available. |
| `STREAM_ERR_SIZE_ZERO` | An operation requiring a nonzero size received zero. |

Check both the function's return value and `errorCode`. Some failed operations can still have an effect:

- A linear-buffer push can drop old bytes, insert the newest bytes that fit, and return `false` with `STREAM_ERR_OVERFLOW_OR_SHORT`.
- A ring-buffer push that cannot fit is rejected; the queued bytes remain unchanged.
- A pop requesting more bytes than are available copies and removes the available bytes, then returns `false` with `STREAM_ERR_OVERFLOW_OR_SHORT`.
- A write whose input is larger than the buffer's usable capacity is rejected before replacing the current contents.
- If a push input itself is larger than the usable capacity, only its last bytes can fit. The operation reports overflow/short status.

The status describes the most recent operation; another call can replace it.

## String and numeric helpers

The header also provides utility functions for fixed-size character buffers:

- `safe_strnlen()` measures a string up to a supplied limit.
- `trimString()` removes leading and trailing whitespace, in place or into a destination buffer.
- `splitString()` splits at the first delimiter. Optional output sizes must include room for the terminating null byte; it checks the requested outputs before writing either one.
- `isWhitespaceOnly()` checks whether a string contains only whitespace. An empty string is considered whitespace-only; a null pointer is invalid.
- `validateRow()` checks a comma-separated row for the expected number of non-empty fields.
- `endsWith()` checks a suffix.
- `isBoolean()`, `isNumber()`, `isIntiger()`, `isUintiger()`, and the signed/unsigned integer and floating-point validators check textual values and ranges. The API spelling `isIntiger` is retained for compatibility.
- `stringToUint8/16/32/64()`, `stringToInt8/16/32/64()`, `stringToFloat()`, and `stringToDouble()` convert validated text.
- `decimalToString()` formats a `float` or `double` with a requested number of decimal places.

The numeric text validators accept decimal notation, not exponent notation or surrounding whitespace. `isNumber()` allows an optional leading sign and at most one decimal point, and requires at least one digit. Boolean validation accepts `true`, `TRUE`, `false`, `FALSE`, `1`, and `0`.

`splitString()` rejects a null input and a null-character delimiter. If an output buffer is supplied, its size must be large enough for its text and trailing `\0`. A null output pointer means that part is not returned.

## Typed values

`dataTypeEnum` describes unsigned and signed integers, `float`, `double`, `char`, `string`, and `bool`. `dataValueUnion` stores the corresponding value. Its `stringValue` can hold up to seven characters plus the null terminator; `stringPointerValue` is a non-owning pointer.

- `stringToNumber(text, &value, type)` parses text into the requested type and returns success or failure.
- `checkValueType()` checks text against a type name or enum value.
- `dataValueToString()` formats a typed value into a caller-provided character buffer.
- A `std::string` formatting overload is available on PC builds.

The character-buffer formatting APIs do not take a destination size. The caller must provide enough space for the formatted result and its terminator. `decimalToString()` has the same caller-buffer requirement.

## PC builds

On Linux and ordinary Windows builds, the header enables PC-only `std::string` overloads for pushing and popping stream data and formatting typed values. These overloads are not enabled for the supported Arm compiler configurations. Embedded code can use the character-buffer overloads on all supported targets.

## Host regression test

The repository contains a standalone regression test in `tests/stream_regression.cpp`. From the `Libraries/Stream_STM32` directory, build and run it with a C++11 compiler:

```sh
g++ -std=c++11 -Wall -Wextra src/Stream.cpp tests/stream_regression.cpp -o stream_regression
./stream_regression
```

The generated executable is a local build artifact; it is not part of the library.

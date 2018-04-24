// Copyright 2018 Daniel Vitalewitsch Rutz <daniel.rutz@rebleyama.de>
//
// This library is free software;
// you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program.
// If not, see <http://www.gnu.org/licenses/>.

#ifndef PREALLOCSTRING_H
#define PREALLOCSTRING_H

#include <cstdarg>
#include <cstddef>
#include <cstring>

#include "Printf.h"

template<size_t _Template_Size>
struct PreAllocString
{
    static_assert(_Template_Size > 1, "The size must be larger 1 (char + null terminator)!");
    constexpr PreAllocString()
    : length{0}
    {
    }

    operator const char*() const { return arr; }

    operator const void*() const { return arr; }

    const char& operator[](const int idx)
    {
        if(idx < 0 || static_cast<size_t>(idx) > length) {
            // TODO: Sane error handling
            return arr[0];
        }
        return arr[idx];
    }

    /* Current number of characters in string */
    size_t GetLength() const { return length; }

    /* Maximum number of characters possible */
    constexpr size_t SizeOf() { return _Template_Size; }

    /* Empty the string, set length field to zero */
    void Empty()
    {
        length = 0;
        arr[0] = '\0';
    }

    PreAllocString& operator=(char rhs)
    {
        // If string is empty, it is of length 1 now
        if(length < 1) {
            length = 1;
            arr[1] = '\0';
        }
        arr[0] = rhs;

        return *this;
    }

    PreAllocString& operator=(const char* rhs)
    {
        if(rhs != nullptr) {
            size_t stringLen = strnlen(rhs, _Template_Size - 1);
            length           = stringLen;
            std::memcpy(arr, rhs, stringLen);
            arr[length] = '\0';
        }

        return *this;
    }

    PreAllocString& operator=(char* const rhs) { return *this = const_cast<const char*>(rhs); }

    PreAllocString& operator+=(char rhs)
    {
        if(length + 1 < _Template_Size) {
            arr[length] = rhs;
            ++length;
            arr[length] = '\0';
        }

        return *this;
    }

    PreAllocString& operator+=(char const* rhs)
    {
        if(rhs != nullptr) {
            size_t stringLen = strnlen(rhs, _Template_Size - 1);

            if(stringLen + length < _Template_Size) {
                std::memcpy(&arr[length], rhs, stringLen);
                length += stringLen;
                arr[length] = '\0';
            }
        }

        return *this;
    }

    void AddFormat(const char* format, ...)
    {
        if(format != nullptr) {
            va_list args;
            va_start(args, format);
            char* endOfPrintedString = Printf_args(&arr[length], &arr[_Template_Size - 1], format, args);
            length += endOfPrintedString - &arr[length];
            va_end(args);
        }
    }

    void AddWhiteSpace() { *this += ' '; }

private:
    char   arr[_Template_Size];
    size_t length;
};

#define CREATE(varName, _Template_Size) PreAllocString<_Template_Size> varName{};
#endif

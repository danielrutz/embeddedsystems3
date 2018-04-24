#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <tuple>

#include "Printf.h"

// A precise integer power implementation
template<typename T>
static intmax_t power(T basis, T exponent)
{
    intmax_t result = 1;
    for(T i = 0; i < exponent; i++) {
        result *= basis;
    }

    return result;
}

static size_t intToCStrInplaceImpl(char* dst, const char* end, unsigned int val, unsigned int radix)
{
    size_t len = 0;
    // highest power is the logarithm of the number with the base radix
    int maxPower = std::log(val) / std::log(radix);

    while(dst < end && maxPower >= 0) {  // avoid out of bounds
        const auto currentPower = power<unsigned int>(radix, maxPower);
        const auto num          = val / currentPower;  // calculate digit, integer division!
        val                     = val % currentPower;  // and remainder
        // ASCII
        if(num >= 10) {  // Hex Number
            dst[0] = num - 10 + 'a';
        } else {
            dst[0] = num + '0';
        }
        --maxPower;
        ++dst;
        ++len;
    }

    return len;
}

static size_t intToDecimalCStrInplace(char* dst, const char* end, unsigned int val)
{
    return intToCStrInplaceImpl(dst, end, val, 10);
}

static size_t intToDecimalCStrInplace(char* dst, const char* end, signed int val)
{
    size_t len = 0;

    // negative numbers
    if(dst < end) {
        if(val < 0) {
            dst[0] = '-';
            ++len;
            ++dst;
        }
    } else
        return 0;

    return intToDecimalCStrInplace(dst, end, static_cast<unsigned int>(std::abs(val))) + len;  // number is definitely positive because of absolute
}

static size_t intToBinaryCStrInplace(char* dst, const char* end, unsigned int val)
{
    if(dst + 1 < end) {
        dst[0] = '0';
        dst[1] = 'b';
        dst += 2;
    } else
        return 0;

    return intToCStrInplaceImpl(dst, end, val, 2) + 2;
}

static size_t intToHexCStrInplace(char* dst, const char* end, unsigned int val)
{
    if(dst + 1 < end) {
        dst[0] = '0';
        dst[1] = 'x';
        dst += 2;
    } else
        return 0;

    return intToCStrInplaceImpl(dst, end, val, 16) + 2;
}

char* Printf_args(char* dst, const void* end, const char* fmt, va_list args)
{
    const char* cend = static_cast<const char*>(end);

    // Handle invalid input
    if(dst == nullptr || cend == nullptr || cend < dst || fmt == nullptr)
        return nullptr;

    for(; dst < cend && fmt[0] != '\0'; ++fmt) {
        if(fmt[0] == '%') {
            ++fmt;
            const char* strarg;
            int         length;
            switch(fmt[0]) {
                case '%':
                    dst[0] = '%';
                    ++dst;
                    break;

                case 'c':
                    dst[0] = va_arg(args, int);
                    ++dst;
                    break;

                case 'd': dst += intToDecimalCStrInplace(dst, cend, va_arg(args, int)); break;

                case 'x': dst += intToHexCStrInplace(dst, cend, va_arg(args, unsigned int)); break;

                case 'u': dst += intToDecimalCStrInplace(dst, cend, va_arg(args, unsigned int)); break;

                case 'b': dst += intToBinaryCStrInplace(dst, cend, va_arg(args, unsigned int)); break;

                case 's':
                    strarg = va_arg(args, const char*);
                    if(strarg == nullptr) {  // invalid pointer
                        return nullptr;
                    }
                    length = std::strlen(strarg);
                    std::strncpy(dst, strarg, cend - dst);
                    // move pointer either by length of strarg, or move it to dst, depending on what is smaller
                    dst += std::min<uintptr_t>(cend - dst, length);
                    break;

                default: return nullptr;  // the user definitely is an idiot and should be killed instantly (with eternal purgatory)
            }
        } else {
            dst[0] = fmt[0];
            ++dst;
        }
    }
    dst[0] = '\0';
    return dst;
}

char* Printf(char* dst, const void* end, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char* result = Printf_args(dst, end, fmt, args);
    va_end(args);
    return result;
}

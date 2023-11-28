#include "utils.hpp"
#include <charconv>
#include <stdexcept>
#include <string_view>
#include <cstdint>

// stoUll, but for std::string_view
uint64_t string_to_uint64(std::string_view str)
{
        uint64_t number;
        auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), number); // returned type is from_chars_result struct
        if (ec != std::errc())
        {
                throw std::invalid_argument("Invalid integer");
        }

        return number;
}


// stoll, but for std::string_view
int64_t string_to_int64(std::string_view str)
{
        int64_t number;
        auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), number);
        if (ec != std::errc())
        {
                throw std::invalid_argument("Invalid integer");
        }

        return number;
}
#include "utils.hpp"
#include <iostream>
#include <charconv>
#include <stdexcept>
#include <string_view>
#include <cstdint>
#include <iomanip>

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

std::string hash_to_hex_string(const std::string &piece_hash)
{
        std::ostringstream oss;
        for (char c : piece_hash)
        {
                unsigned char byte = static_cast<unsigned char>(c); // cast to unsigned char to avoid sign extension
                int value = static_cast<int>(byte);
                oss << std::hex << std::setw(2) << std::setfill('0') << value;
        }
        return oss.str();
}
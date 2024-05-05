#include "utils.hpp"
#include <string>
#include <cctype>
#include <array>

std::string url_encode(const std::string &hex_string)
{
        std::string result;
        result.reserve(hex_string.length() + hex_string.length() / 2);

        std::array<bool, 256> unreserved{};
        for (size_t i = '0'; i <= '9'; ++i)
                unreserved[i] = true;
        for (size_t i = 'A'; i <= 'Z'; ++i)
                unreserved[i] = true;
        for (size_t i = 'a'; i <= 'z'; ++i)
                unreserved[i] = true;
        unreserved['-'] = true;
        unreserved['_'] = true;
        unreserved['.'] = true;
        unreserved['~'] = true;

        for (size_t i = 0; i < hex_string.length(); i += 2)
        {
                std::string byte_str = hex_string.substr(i, 2);
                size_t byte_val = std::stoul(byte_str, nullptr, 16);
                if (unreserved[byte_val])
                {
                        result += static_cast<char>(byte_val);
                }
                else
                {
                        result += "%" + byte_str;
                }
        }
        return result;
}
#include <string>
#include <iostream>
#include <iomanip>
#include <ios>

std::string hash_in_hex(const std::string &piece_hash)
{
    std::string result;
    for (char c : piece_hash)
    {
        unsigned char byte = static_cast<unsigned char>(c);
        int value = static_cast<int>(byte);
        std::stringstream ss;
        ss << std::hex << std::setw(2) << std::setfill('0') << value;
        result += ss.str();
    }
    return result;
}